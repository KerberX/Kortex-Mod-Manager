#include "stdafx.h"
#include <Kortex/Application.hpp>
#include <Kortex/PluginManager.hpp>
#include <Kortex/SaveManager.hpp>
#include <Kortex/ModManager.hpp>
#include <Kortex/Application.hpp>
#include <Kortex/GameInstance.hpp>
#include "GameMods/ModManager/Workspace.h"
#include "UI/ImageViewerDialog.h"
#include "Utility/Common.h"
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxAuiNotebook.h>
#include <kxf::UI::Framework/KxFileBrowseDialog.h>
#include <kxf::UI::Framework/KxTextFile.h>
#include <kxf::UI::Framework/KxCrypto.h>
#include <kxf::UI::Framework/KxShell.h>

namespace Kortex::Application::OName
{
	KortexDefOption(FileFilters);
}

namespace
{
	using namespace Kortex;
	using namespace Kortex::Application;

	kxf::String FilterNameToSignature(const Utility::LabeledValue& filter)
	{
		kxf::StringInputStream stream(filter.GetValue());
		return wxS("Filter-") + KxCrypto::CRC32(stream);
	}

	auto GetDisplayModelOptions()
	{
		return GetAInstanceOptionOf<ISaveManager>(OName::Workspace, OName::DisplayModel);
	}
	auto GetFiltersOptions()
	{
		return GetAInstanceOptionOf<ISaveManager>(OName::Workspace, OName::FileFilters);
	}
	auto GetFilterOption(const Utility::LabeledValue& filter)
	{
		return GetAInstanceOptionOf<ISaveManager>(OName::Workspace, OName::FileFilters, FilterNameToSignature(filter));
	}
}

namespace Kortex::SaveManager
{
	bool Workspace::OnCreateWorkspace()
	{
		m_MainSizer = new wxBoxSizer(wxVERTICAL);
		SetSizer(m_MainSizer);

		CreateViewPane();
		m_MainSizer->Add(m_DisplayModel->GetView(), 1, wxEXPAND);

		// Load options
		GetDisplayModelOptions().LoadDataViewLayout(m_DisplayModel->GetView());
		m_ActiveFilters.clear();

		// Load filters
		auto filterOptions = GetFiltersOptions();
		for (const auto& filter: ISaveManager::GetInstance()->GetConfig().GetFileFilters())
		{
			if (GetFilterOption(filter).GetAttributeBool(OName::Enabled, true))
			{
				m_ActiveFilters.insert(filter.GetValue());
			}
		}

		ScheduleReload();
		return true;
	}
	bool Workspace::OnOpenWorkspace()
	{
		if (!OpenedOnce())
		{
			OnReloadWorkspace();
		}
		return true;
	}
	bool Workspace::OnCloseWorkspace()
	{
		IMainWindow::GetInstance()->ClearStatus(1);
		return true;
	}
	void Workspace::OnReloadWorkspace()
	{
		UpdateFilters();
	}

	Workspace::~Workspace()
	{
		if (IsCreated())
		{
			GetDisplayModelOptions().SaveDataViewLayout(m_DisplayModel->GetView());

			kxf::XMLNode filterNode = GetFiltersOptions().GetNode();
			filterNode.ClearNode();

			for (const auto& filter: ISaveManager::GetInstance()->GetConfig().GetFileFilters())
			{
				kxf::XMLNode node = filterNode.NewElement(FilterNameToSignature(filter));
				node.SetAttribute(OName::Enabled, FiltersMenu_IsFilterActive(filter.GetValue()));
			}
		}
	}

	void Workspace::CreateViewPane()
	{
		m_DisplayModel = new DisplayModel();
		m_DisplayModel->CreateView(this);
	}
	void Workspace::UpdateFilters()
	{
		KxStringVector filterList;
		for (const kxf::String& filter: m_ActiveFilters)
		{
			filterList.push_back(filter);
		}
		ISaveManager::GetInstance()->UpdateActiveFilters(filterList);
	}

	bool Workspace::FiltersMenu_IsAllFiltersActive() const
	{
		return m_ActiveFilters.size() == ISaveManager::GetInstance()->GetConfig().GetFileFilters().size();
	}
	void Workspace::FiltersMenu_AllFiles(kxf::UI::MenuEvent& event)
	{
		if (FiltersMenu_IsAllFiltersActive())
		{
			m_ActiveFilters.clear();
		}
		else
		{
			for (const Utility::LabeledValue& filter: ISaveManager::GetInstance()->GetConfig().GetFileFilters())
			{
				m_ActiveFilters.insert(filter.GetValue());
			}
		}
		ScheduleReload();
	}
	void Workspace::FiltersMenu_SpecificFilter(kxf::UI::MenuEvent& event)
	{
		// Add or remove this filter
		const kxf::UI::MenuItem* item = event.GetItem();
		const Utility::LabeledValue* pFilter = static_cast<const Utility::LabeledValue*>(item->GetClientData());
		if (item->IsChecked())
		{
			m_ActiveFilters.insert(pFilter->GetValue());
		}
		else
		{
			m_ActiveFilters.erase(pFilter->GetValue());
		}
		ScheduleReload();
	}

	void Workspace::OnSyncPluginsList(const IBethesdaGameSave& save)
	{
		if (IPluginManager* manager = IPluginManager::GetInstance())
		{
			manager->SyncWithPluginsList(save.GetPlugins(), PluginManager::SyncListMode::ActivateAll);
			manager->Save();
			manager->ScheduleWorkspacesReload();
		}
	}
	void Workspace::OnSavePluginsList(const IBethesdaGameSave& save)
	{
		KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_SAVE);
		dialog.SetDefaultExtension("txt");
		dialog.SetFileName(save.GetFileItem().GetName().BeforeLast('.'));
		dialog.AddFilter("*.txt", KTr("FileFilter.Text"));
		dialog.AddFilter("*", KTr("FileFilter.AllFiles"));

		if (dialog.ShowModal() == wxID_OK)
		{
			KxTextFile::WriteToFile(dialog.GetResult(), save.GetPlugins());
		}
	}
	bool Workspace::OnRemoveSave(IGameSave& save)
	{
		if (BroadcastProcessor::Get().ProcessEventEx(SaveEvent::EvtRemoving, save).Do().IsAllowed())
		{
			const Config& config = ISaveManager::GetInstance()->GetConfig();
			const KxFileItem& primaryInfo = save.GetFileItem();

			KxFile(primaryInfo.GetFullPath()).RemoveFile(true);
			if (config.HasMultiFileSaveConfig())
			{
				KxFileItem secondaryInfo = primaryInfo;
				secondaryInfo.SetName(primaryInfo.GetName().BeforeLast('.') + '.' + config.GetSecondarySaveExtension());
				KxFile(secondaryInfo.GetFullPath()).RemoveFile(true);
			}

			BroadcastProcessor::Get().ProcessEvent(SaveEvent::EvtRemoved);
			return true;
		}
		return false;
	}

	kxf::String Workspace::GetID() const
	{
		return "SaveManager::Workspace";
	}
	kxf::String Workspace::GetName() const
	{
		return KTr("SaveManager.NameShort");
	}
	IWorkspaceContainer* Workspace::GetPreferredContainer() const
	{
		IWorkspaceContainer* result = nullptr;
		IWorkspace::CallIfCreated<ModManager::Workspace>([&](ModManager::Workspace& workspace)
		{
			result = &workspace.GetWorkspaceContainer();
		});
		return result;
	}

	void Workspace::OnSelection(const IGameSave* save)
	{
		const int statusIndex = 1;
		IMainWindow* mainWindow = IMainWindow::GetInstance();
		mainWindow->ClearStatus(statusIndex);

		if (save)
		{
			if (save->IsOK())
			{
				mainWindow->SetStatus(save->GetDisplayName(), statusIndex);
			}
			else
			{
				mainWindow->SetStatus(KTr("SaveManager.InvalidFile"), statusIndex, ImageResourceID::CrossCircleFrame);
			}
		}
	}
	void Workspace::OnContextMenu(const IGameSave* save)
	{
		kxf::UI::Menu menu;

		IPluginManager* pluginManager = IPluginManager::GetInstance();
		PluginManager::Workspace* pluginWorkspace = PluginManager::Workspace::GetInstance();

		const bool isMultiSelect = m_DisplayModel->GetView()->GetSelectedCount() > 1;
		const IBethesdaGameSave* bethesdaSave = save ? save->QueryInterface<IBethesdaGameSave>() : nullptr;
		const bool hasPlugins = bethesdaSave && bethesdaSave->HasPlugins();

		// Plugins list
		if (pluginManager && pluginWorkspace && bethesdaSave)
		{
			const KxStringVector pluginsList = bethesdaSave->GetPlugins();

			kxf::UI::Menu* pluginsMenu = new kxf::UI::Menu();
			kxf::UI::MenuItem* pluginsMenuItem = menu.Add(pluginsMenu, kxf::String::Format(wxS("%1 (%2)"), KTr("SaveManager.Tab.PluginsList"), pluginsList.size()));
			pluginsMenuItem->Enable(!pluginsList.empty());

			if (!pluginManager->HasPlugins())
			{
				pluginManager->Load();
			}

			for (const kxf::String& name: pluginsList)
			{
				kxf::UI::MenuItem* item = pluginsMenu->AddItem(name);
				item->SetBitmap(ImageProvider::GetBitmap(pluginWorkspace->GetStatusImageForPlugin(pluginManager->FindPluginByName(name))));
			}
		}

		// Basic info
		{
			kxf::UI::Menu* basicInfoMenu = new kxf::UI::Menu();
			kxf::UI::MenuItem* basicInfoMenuItem = menu.Add(basicInfoMenu, KTr("SaveManager.Tab.BasicInfo"));
			basicInfoMenuItem->Enable(false);

			if (save)
			{
				const auto& basicInfo = save->GetBasicInfo();
				basicInfoMenuItem->Enable(!basicInfo.empty());

				for (const Utility::LabeledValue& entry: basicInfo)
				{
					kxf::UI::MenuItem* item = basicInfoMenu->AddItem(kxf::String::Format(wxS("%1: %2"), entry.GetLabel(), entry.GetValue()));
				}
				basicInfoMenu->Bind(kxf::UI::MenuEvent::EvtSelect, [](kxf::UI::MenuEvent& event)
				{
					Utility::CopyTextToClipboard(event.GetItem()->GetItemLabelText());
				});
			}
		}
		menu.AddSeparator();

		// Sync
		{
			kxf::UI::MenuItem* item = menu.AddItem(KTr("SaveManager.SyncPluginsList"));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::PlugDisconnect));
			item->Enable(!isMultiSelect && bethesdaSave && hasPlugins && pluginManager);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, bethesdaSave](kxf::UI::MenuEvent& event)
			{
				OnSyncPluginsList(*bethesdaSave);
			});
		}

		// Save
		{
			kxf::UI::MenuItem* item = menu.Add(new kxf::UI::MenuItem(wxID_SAVE, KTr("SaveManager.SavePluginsList")));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Disk));
			item->Enable(!isMultiSelect && bethesdaSave && hasPlugins);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, bethesdaSave](kxf::UI::MenuEvent& event)
			{
				OnSavePluginsList(*bethesdaSave);
			});
		}
		menu.AddSeparator();

		// File filter
		{
			kxf::UI::Menu* filtersMenu = new kxf::UI::Menu();
			menu.Add(filtersMenu, KTr("FileFilter"));

			// All files
			{
				kxf::UI::MenuItem* item = filtersMenu->Add(new kxf::UI::MenuItem(KTr("FileFilter.AllFiles"), wxEmptyString, wxITEM_CHECK));
				item->Check(FiltersMenu_IsAllFiltersActive());
				item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::FiltersMenu_AllFiles, this);
			}

			filtersMenu->AddSeparator();

			// Specific
			for (const Utility::LabeledValue& filter: ISaveManager::GetInstance()->GetConfig().GetFileFilters())
			{
				kxf::UI::MenuItem* item = filtersMenu->Add(new kxf::UI::MenuItem(filter.GetLabel(), wxEmptyString, wxITEM_CHECK));
				item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::FiltersMenu_SpecificFilter, this);
				item->SetClientData(const_cast<Utility::LabeledValue*>(&filter));
				item->Check(FiltersMenu_IsFilterActive(filter.GetValue()));
			}
		}
		menu.AddSeparator();

		// Open location
		{
			kxf::UI::MenuItem* item = menu.AddItem(save ? KTr("Generic.FileLocation") : KTr("Generic.FolderLocation"));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::FolderOpen));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, save](kxf::UI::MenuEvent& event)
			{
				if (save)
				{
					KxShell::OpenFolderAndSelectItem(save->GetFileItem().GetFullPath());
				}
				else
				{
					KxShell::Execute(this, ISaveManager::GetInstance()->GetConfig().GetLocation(), wxS("open"));
				}
			});
		}

		// Reload items
		{
			kxf::UI::MenuItem* item = menu.AddItem(KTr(wxID_REFRESH));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::ArrowCircleDouble));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				ScheduleReload();
			});
		}

		// Remove
		{
			kxf::UI::MenuItem* item = menu.Add(new kxf::UI::MenuItem(wxID_REMOVE, KTr(wxID_REMOVE)));
			item->Enable(save && !IModManager::GetInstance()->GetFileSystem().IsEnabled());
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				for (KxDataView2::Node* node: m_DisplayModel->GetView()->GetSelections())
				{
					OnRemoveSave(m_DisplayModel->GetItem(*node));
				}
				ScheduleReload();
			});
		}

		menu.Show(m_DisplayModel->GetView());
	}
}
