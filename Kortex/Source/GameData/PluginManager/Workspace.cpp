#include "stdafx.h"
#include <Kortex/Application.hpp>
#include <Kortex/ApplicationOptions.hpp>
#include <Kortex/PluginManager.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/Application.hpp>
#include "GameMods/IModManager.h"
#include "GameMods/ModEvent.h"
#include "GameMods/ModManager/Workspace.h"
#include "UI/ImageViewerDialog.h"
#include "Utility/OperationWithProgress.h"
#include <kxf::UI::Framework/KxSearchBox.h>
#include <kxf::UI::Framework/KxNotebook.h>
#include <kxf::UI::Framework/KxAuiNotebook.h>
#include <kxf::UI::Framework/KxHTMLWindow.h>
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/KxFileBrowseDialog.h>
#include <kxf::UI::Framework/KxTextFile.h>
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxShell.h>

namespace
{
	using namespace Kortex;
	using namespace Kortex::Application;
	using namespace Kortex::PluginManager;

	auto GetDisplayModelOptions()
	{
		return Application::GetAInstanceOptionOf<IPluginManager>(OName::Workspace, OName::DisplayModel);
	}

	namespace MenuCounter
	{
		enum PluginType: uint32_t
		{
			Normal = 0,
			Master = 1 << 0,
			Light = 1 << 1,

			Total = ~PluginType() - 1,
			Active = ~PluginType()
		};
		bool CheckType(const IBethesdaGamePlugin& plugin, uint32_t type)
		{
			if (type == PluginType::Active)
			{
				return true;
			}

			bool ok = true;
			if (type & PluginType::Normal)
			{
				ok = ok && plugin.IsNormal();
			}
			if (type & PluginType::Master)
			{
				ok = ok && plugin.IsMaster();
			}
			if (type & PluginType::Light)
			{
				ok = ok && plugin.IsLight();
			}
			return ok;
		}

		class CounterData: public wxClientData
		{
			private:
			uint32_t m_Type = PluginType::Normal;

			public:
			CounterData(uint32_t type)
				:m_Type(type)
			{
			}

			public:
			uint32_t GetType() const
			{
				return m_Type;
			}
		};
	}
}

namespace Kortex::PluginManager
{
	ImageResourceID Workspace::GetStatusImageForPlugin(const IGamePlugin* plugin)
	{
		if (plugin)
		{
			return plugin->IsActive() ? ImageResourceID::TickCircleFrameEmpty : ImageResourceID::InformationFrameEmpty;
		}
		else
		{
			return ImageResourceID::CrossCircleEmpty;
		}
	}

	void Workspace::CreateModelView()
	{
		m_ModelView = new PluginViewModel();
		m_ModelView->Create(this);
		m_ModelView->SetDataVector(IPluginManager::GetInstance()->GetPlugins());
	}
	void Workspace::OnModSerach(wxCommandEvent& event)
	{
		if (m_ModelView->SetSearchMask(event.GetEventType() == wxEVT_SEARCHCTRL_SEARCH_BTN ? event.GetString() : wxEmptyString))
		{
			m_ModelView->RefreshItems();
		}
	}
	void Workspace::UpdatePluginTypeCounter(kxf::UI::MenuItem* item)
	{
		using namespace MenuCounter;

		const Config& pluginsConfig = IPluginManager::GetInstance()->GetConfig();
		CounterData* clientData = static_cast<CounterData*>(item->GetClientObject());
		if (clientData)
		{
			size_t count = 0;
			if (clientData->GetType() == PluginType::Total)
			{
				count = IPluginManager::GetInstance()->GetPlugins().size();
			}
			else
			{
				for (const auto& plugin: IPluginManager::GetInstance()->GetPlugins())
				{
					if (plugin->IsActive())
					{
						bool typeOK = false;
						const IBethesdaGamePlugin* bethesdaPlugin = nullptr;
						if (plugin->QueryInterface(bethesdaPlugin))
						{
							typeOK = CheckType(*bethesdaPlugin, clientData->GetType());
						}

						if (typeOK || clientData->GetType() == PluginType::Active)
						{
							count++;
						}
					}
				}
			}

			if (clientData->GetType() == PluginType::Total)
			{
				item->SetItemLabel(kxf::String::Format("%1: %2", KTr("PluginManager.PluginCounter.Total"), count));
			}
			else if (clientData->GetType() == PluginType::Active)
			{
				if (pluginsConfig.HasPluginLimit())
				{
					item->SetItemLabel(kxf::String::Format("%1: %2/%3", KTr("PluginManager.PluginCounter.Active"), count, pluginsConfig.GetPluginLimit()));
					item->SetBitmap(ImageProvider::GetBitmap(count >= static_cast<size_t>(pluginsConfig.GetPluginLimit()) ? ImageResourceID::Exclamation : ImageResourceID::TickCircleFrame));
				}
			}
			else
			{
				kxf::String label;
				if (clientData->GetType() == PluginType::Active)
				{
					label = KTr("Generic.All");
				}
				else if (BethesdaPluginManager* bethesda = IPluginManager::GetInstance()->QueryInterface<BethesdaPluginManager>())
				{
					label = bethesda->GetPluginTypeName(clientData->GetType() & PluginType::Master, clientData->GetType() & PluginType::Light);
				}
				item->SetItemLabel(kxf::String::Format("%1: %2", label, count));
			}
		}
	}
	void Workspace::OnRunLootAPI(kxf::UI::MenuEvent& event)
	{
		auto operation = new Utility::OperationWithProgressDialog<KxFileOperationEvent>(true, this);
		operation->OnRun([operation]()
		{
			KxStringVector sortedList;
			if (LibLoot::GetInstance()->SortPlugins(sortedList, operation))
			{
				IPluginManager::GetInstance()->SyncWithPluginsList(sortedList, SyncListMode::DoNotChange);
				IPluginManager::GetInstance()->Save();
			}
		});
		operation->OnEnd([this]()
		{
			BroadcastProcessor::Get().ProcessEvent(PluginEvent::EvtReordered);
		});
		operation->SetDialogCaption(event.GetItem()->GetItemLabelText());
		operation->Run();
	}

	void Workspace::OnBeginReload(ModEvent& event)
	{
		Disable();
		m_ModelView->SetDataVector();
	}
	void Workspace::OnEndReload(ModEvent& event)
	{
		m_ModelView->SetDataVector(IPluginManager::GetInstance()->GetPlugins());
		Enable();
	}

	bool Workspace::OnCreateWorkspace()
	{
		CreateModelView();
		m_MainSizer = new wxBoxSizer(wxVERTICAL);
		m_MainSizer->Add(m_ModelView->GetView(), 1, wxEXPAND);

		m_SearchBox = new KxSearchBox(this, wxID_NONE);
		m_SearchBox->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &Workspace::OnModSerach, this);
		m_SearchBox->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &Workspace::OnModSerach, this);
		m_MainSizer->Add(m_SearchBox, 0, wxEXPAND|wxTOP, LayoutConstants::VerticalSpacing);

		SetSizer(m_MainSizer);
		GetDisplayModelOptions().LoadDataViewLayout(m_ModelView->GetView());

		m_BroadcastReciever.Bind(ModEvent::EvtBeginReload, &Workspace::OnBeginReload, this);
		m_BroadcastReciever.Bind(ModEvent::EvtEndReload, &Workspace::OnEndReload, this);
		return true;
	}
	bool Workspace::OnOpenWorkspace()
	{
		if (!OpenedOnce())
		{
			IPluginManager::GetInstance()->Load();
			m_ModelView->RefreshItems();
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
		m_ModelView->RefreshItems();
		ProcessSelection();
	}
	
	Workspace::~Workspace()
	{
		if (IsCreated())
		{
			IGameInstance::GetActive()->GetActiveProfile()->SyncWithCurrentState();
			GetDisplayModelOptions().SaveDataViewLayout(m_ModelView->GetView());
		}
	}

	kxf::String Workspace::GetID() const
	{
		return "KPluginManagerWorkspace";
	}
	kxf::String Workspace::GetName() const
	{
		return KTr("PluginManager.NameShort");
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

	void Workspace::OnCreateViewContextMenu(kxf::UI::Menu& menu, const IGamePlugin* plugin)
	{
		using namespace MenuCounter;

		// Plugin type counter
		auto AddCounter = [this, &menu](uint32_t type)
		{
			kxf::UI::MenuItem* item = menu.Add(new kxf::UI::MenuItem(wxEmptyString));
			item->SetClientObject(new MenuCounter::CounterData(type));
			item->Enable(false);
		};
		menu.Bind(KxEVT_MENU_OPEN, [this](kxf::UI::MenuEvent& event)
		{
			for (const auto& item: event.GetMenu()->GetMenuItems())
			{
				UpdatePluginTypeCounter(static_cast<kxf::UI::MenuItem*>(item));
			}
			event.Skip();
		});

		AddCounter(PluginType::Total);
		AddCounter(PluginType::Active);

		if (dynamic_cast<BethesdaPluginManager*>(IPluginManager::GetInstance()))
		{
			AddCounter(PluginType::Normal);
			AddCounter(PluginType::Master);
		}
		if (dynamic_cast<BethesdaPluginManager2*>(IPluginManager::GetInstance()))
		{
			AddCounter(PluginType::Light);
		}
	}
	void Workspace::OnCreateSortingToolsMenu(kxf::UI::Menu& menu, const IGamePlugin* plugin)
	{
		kxf::UI::Menu* sortingMenu = nullptr;
		const Config& pluginsConfig = IPluginManager::GetInstance()->GetConfig();
		if (pluginsConfig.HasSortingTools() || LibLoot::GetInstance())
		{
			sortingMenu = new kxf::UI::Menu();
			menu.AddSeparator();

			kxf::UI::MenuItem* item = menu.Add(sortingMenu, KTr("PluginManager.Sorting"));
			item->Enable(IModManager::GetInstance()->GetFileSystem().IsEnabled());
		}

		if (sortingMenu && IModManager::GetInstance()->GetFileSystem().IsEnabled())
		{
			// LibLoot
			bool hasLoot = false;
			if (LibLoot::GetInstance() != nullptr)
			{
				hasLoot = true;

				kxf::UI::MenuItem* item = sortingMenu->Add(new kxf::UI::MenuItem("LOOT API"));
				item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnRunLootAPI, this);
			}

			// Sorting tools
			{
				const auto& sortingTools = pluginsConfig.GetSortingTools();
				if (hasLoot && !sortingTools.empty())
				{
					sortingMenu->AddSeparator();
				}

				for (const SortingToolItem& toolItem: sortingTools)
				{
					kxf::UI::MenuItem* item = sortingMenu->Add(new kxf::UI::MenuItem(toolItem.GetName()));
					item->SetBitmap(KxShell::GetFileIcon(toolItem.GetExecutable(), true));
					item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, toolItem](kxf::UI::MenuEvent& event)
					{
						IPluginManager::GetInstance()->Save();
						IPluginManager::GetInstance()->RunSortingTool(toolItem);
					});
				}
			}
		}
	}
	void Workspace::OnCreateImportExportMenu(kxf::UI::Menu& menu, const IGamePlugin* plugin)
	{
		menu.AddSeparator();
		{
			kxf::UI::MenuItem* item = menu.Add(new kxf::UI::MenuItem(KTr("PluginManager.Tools.ImportList")));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_OPEN);
				dialog.AddFilter("*.txt", KTr("FileFilter.Text"));
				dialog.AddFilter("*", KTr("FileFilter.AllFiles"));
				if (dialog.ShowModal() == wxID_OK)
				{
					IPluginManager::GetInstance()->SyncWithPluginsList(KxTextFile::ReadToArray(dialog.GetResult()));
				}
			});
		}
		{
			kxf::UI::MenuItem* itemAll = menu.Add(new kxf::UI::MenuItem(KTr("PluginManager.Tools.ExportList")));
			kxf::UI::MenuItem* itemActive = menu.Add(new kxf::UI::MenuItem(KTr("PluginManager.Tools.ExportListActive")));

			auto Event = [this, itemActive](kxf::UI::MenuEvent& event)
			{
				KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_SAVE);
				dialog.AddFilter("*.txt", KTr("FileFilter.Text"));
				dialog.AddFilter("*", KTr("FileFilter.AllFiles"));
				dialog.SetDefaultExtension("txt");

				if (dialog.ShowModal() == wxID_OK)
				{
					bool activeOnly = event.GetId() == itemActive->GetId();
					KxTextFile::WriteToFile(dialog.GetResult(), IPluginManager::GetInstance()->GetPluginsList(activeOnly));
				}
			};
			itemAll->Bind(kxf::UI::MenuEvent::EvtSelect, Event);
			itemActive->Bind(kxf::UI::MenuEvent::EvtSelect, Event);
		}
	}

	void Workspace::ProcessSelection(const IGamePlugin* plugin)
	{
		const int statusIndex = 1;
		IMainWindow* mainWindow = IMainWindow::GetInstance();
		mainWindow->ClearStatus(statusIndex);

		if (plugin)
		{
			if (plugin->IsOK())
			{
				mainWindow->SetStatus(plugin->GetName(), statusIndex);
			}
			else
			{
				mainWindow->SetStatus(wxEmptyString, statusIndex, ImageResourceID::CrossCircleFrame);
			}
		}
	}
	void Workspace::HighlightPlugin(const IGamePlugin* plugin)
	{
		if (m_ModelView)
		{
			KxDataViewCtrl* view = m_ModelView->GetView();
			if (plugin)
			{
				KxDataViewItem item = m_ModelView->GetItemByEntry(plugin);
				view->Select(item);
				view->EnsureVisible(item);
			}
			else
			{
				view->UnselectAll();
			}
		}
	}
}
