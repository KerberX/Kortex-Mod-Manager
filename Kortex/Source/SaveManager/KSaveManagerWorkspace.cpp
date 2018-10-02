#include "stdafx.h"
#include "KSaveManagerWorkspace.h"
#include "KSaveManager.h"
#include "KSaveManagerView.h"
#include "KSaveFile.h"
#include "UI/KImageViewerDialog.h"
#include "Profile/KProfile.h"
#include "PluginManager/KPluginManager.h"
#include "PluginManager/KPluginManagerWorkspace.h"
#include "ModManager/KModManager.h"
#include "KThemeManager.h"
#include "KApp.h"
#include "KAux.h"
#include <KxFramework/KxFile.h>
#include <KxFramework/KxAuiNotebook.h>
#include <KxFramework/KxFileBrowseDialog.h>
#include <KxFramework/KxTextFile.h>
#include <wx/clipbrd.h>

KSaveManagerWorkspace::KSaveManagerWorkspace(KMainWindow* mainWindow, KSaveManager* manager)
	:KWorkspace(mainWindow), m_Manager(manager),
	m_SavesListViewOptions(this, "SavesListView"), m_FileFiltersOptions(this, "FileFilters")
{
	m_MainSizer = new wxBoxSizer(wxVERTICAL);
}
KSaveManagerWorkspace::~KSaveManagerWorkspace()
{
	if (IsWorkspaceCreated())
	{
		KProgramOptionSerializer::SaveDataViewLayout(m_ViewModel->GetView(), m_SavesListViewOptions);

		for (const auto& v: KSaveManagerConfig::GetInstance()->GetFileFilters())
		{
			m_FileFiltersOptions.SetAttribute(v.GetValue(), FiltersMenu_IsFilterActive(v.GetValue()));
		}
	}
}
bool KSaveManagerWorkspace::OnCreateWorkspace()
{
	CreateViewPane();
	m_MainSizer->Add(m_ViewModel->GetView(), 1, wxEXPAND);

	// Load options
	KProgramOptionSerializer::LoadDataViewLayout(m_ViewModel->GetView(), m_SavesListViewOptions);
	m_ViewModel->UpdateRowHeight();

	const KSaveManagerConfig* profileConfig = KSaveManagerConfig::GetInstance();
	m_ActiveFilters.clear();

	KxStringVector filters;
	for (const auto& v: profileConfig->GetFileFilters())
	{
		const wxString& value = v.GetValue();
		if (m_FileFiltersOptions.GetAttributeBool(value, true))
		{
			m_ActiveFilters.insert(value);
			filters.push_back(value);
		}
	}

	ReloadWorkspace();
	return true;
}

void KSaveManagerWorkspace::CreateViewPane()
{
	m_ViewModel = new KSaveManagerView(m_Manager, this);
	m_ViewModel->Create(this);
	m_ViewModel->SetDataVector();
}
void KSaveManagerWorkspace::CreateContextMenu(KxMenu& menu, const KSaveFile* saveEntry)
{
	KPluginManager* pluginManager = KPluginManager::GetInstance();
	KPluginManagerWorkspace* pluginWorkspace = KPluginManagerWorkspace::GetInstance();
	
	const size_t selectedItemCount = m_ViewModel->GetView()->GetSelectedItemsCount();
	const bool isMultiSelect = selectedItemCount > 1;

	// Plugins list
	if (pluginManager && pluginWorkspace && saveEntry)
	{
		const KxStringVector& list = saveEntry->GetPluginsList();

		KxMenu* pluginsListMenu = new KxMenu();
		KxMenuItem* pPluginsListMenuItem = menu.Add(pluginsListMenu, wxString::Format("%s (%zu)", T("SaveManager.Tab.PluginsList"), list.size()));
		pPluginsListMenuItem->Enable(!list.empty());

		for (const wxString& name: list)
		{
			KxMenuItem* item = pluginsListMenu->Add(new KxMenuItem(name));
			item->SetBitmap(KGetBitmap(pluginWorkspace->GetStatusImageForPlugin(pluginManager->FindPluginByName(name))));
		}
	}

	// Basic info
	{
		KxMenu* basicInfoMenu = new KxMenu();
		KxMenuItem* pBasicInfoMenuItem = menu.Add(basicInfoMenu, T("SaveManager.Tab.BasicInfo"));
		pBasicInfoMenuItem->Enable(saveEntry && !saveEntry->GetBasicInfo().empty());

		if (saveEntry)
		{
			for (const KLabeledValue& entry: saveEntry->GetBasicInfo())
			{
				KxMenuItem* item = basicInfoMenu->Add(new KxMenuItem(wxString::Format("%s: %s", entry.GetLabel(), entry.GetValue())));
			}

			basicInfoMenu->Bind(KxEVT_MENU_SELECT, [](KxMenuEvent& event)
			{
				if (wxTheClipboard->Open())
				{
					KxMenuItem* item = event.GetItem();
					wxTheClipboard->SetData(new wxTextDataObject(item->GetItemLabelText()));
					wxTheClipboard->Close();
				}
			});
		}
	}
	menu.AddSeparator();

	// Sync
	{
		KxMenuItem* item = menu.Add(new KxMenuItem(KxID_APPLY, T("SaveManager.SyncPluginsList")));
		item->SetBitmap(KGetBitmap(KIMG_PLUG_DISCONNECT));
		item->Enable(!isMultiSelect && saveEntry && saveEntry->HasPluginsList() && pluginManager);
	}

	// Save
	{
		KxMenuItem* item = menu.Add(new KxMenuItem(KxID_SAVE, T("SaveManager.SavePluginsList")));
		item->SetBitmap(KGetBitmap(KIMG_DISK));
		item->Enable(!isMultiSelect && saveEntry && saveEntry->HasPluginsList());
	}
	menu.AddSeparator();

	// File filter
	{
		const KSaveManagerConfig* profileConfig = KSaveManagerConfig::GetInstance();

		KxMenu* filtersMenu = new KxMenu();
		menu.Add(filtersMenu, T("FileFilter"));

		// All files
		KxMenuItem* allFilesItem = filtersMenu->Add(new KxMenuItem(T("FileFilter.AllFiles"), wxEmptyString, wxITEM_CHECK));
		allFilesItem->Check(FiltersMenu_IsAllFiltersActive());
		allFilesItem->Bind(KxEVT_MENU_SELECT, &KSaveManagerWorkspace::FiltersMenu_AllFiles, this);

		filtersMenu->AddSeparator();

		// Specific
		for (const KLabeledValue& v: profileConfig->GetFileFilters())
		{
			KxMenuItem* item = filtersMenu->Add(new KxMenuItem(v.GetLabel(), wxEmptyString, wxITEM_CHECK));
			item->Bind(KxEVT_MENU_SELECT, &KSaveManagerWorkspace::FiltersMenu_SpecificFilter, this);
			item->SetClientData(const_cast<KLabeledValue*>(&v));
			item->Check(FiltersMenu_IsFilterActive(v.GetValue()));
		}
	}
	menu.AddSeparator();

	// Reload data
	{
		KxMenuItem* item = menu.Add(new KxMenuItem(KxID_REFRESH, T(KxID_REFRESH)));
		item->SetBitmap(KGetBitmap(KIMG_ARROW_CIRCLE_DOUBLE));
	}
	// Remove
	{
		KxMenuItem* item = menu.Add(new KxMenuItem(KxID_REMOVE, T(KxID_REMOVE)));
		item->Enable(selectedItemCount != 0);
	}
}

bool KSaveManagerWorkspace::FiltersMenu_IsAllFiltersActive() const
{
	return m_ActiveFilters.size() == KSaveManagerConfig::GetInstance()->GetFileFilters().size();
}
void KSaveManagerWorkspace::FiltersMenu_AllFiles(KxMenuEvent& event)
{
	if (FiltersMenu_IsAllFiltersActive())
	{
		m_ActiveFilters.clear();
	}
	else
	{
		for (const auto& v: KSaveManagerConfig::GetInstance()->GetFileFilters())
		{
			m_ActiveFilters.insert(v.GetValue());
		}
	}

	LoadData();
}
void KSaveManagerWorkspace::FiltersMenu_SpecificFilter(KxMenuEvent& event)
{
	// Add or remove this filter
	const KxMenuItem* item = event.GetItem();
	const KLabeledValue* pFilter = static_cast<const KLabeledValue*>(item->GetClientData());
	if (item->IsChecked())
	{
		m_ActiveFilters.insert(pFilter->GetValue());
	}
	else
	{
		m_ActiveFilters.erase(pFilter->GetValue());
	}

	LoadData();
}

void KSaveManagerWorkspace::OnSyncPluginsList(const KSaveFile* saveEntry)
{
	if (KPluginManager* manager = KPluginManager::GetInstance())
	{
		manager->SyncWithPluginsList(saveEntry->GetPluginsList(), KPluginManager::SyncListMode::EnableAll);
		manager->Save();

		if (KWorkspace* workspace = manager->GetWorkspace())
		{
			workspace->ScheduleReload();
		}
	}
}
void KSaveManagerWorkspace::OnSavePluginsList(const KSaveFile* saveEntry)
{
	KxFileBrowseDialog dialog(GetMainWindow(), KxID_NONE, KxFBD_SAVE);
	dialog.SetDefaultExtension("txt");
	dialog.SetFileName(saveEntry->GetFileInfo().GetName().BeforeLast('.'));
	dialog.AddFilter("*.txt", T("FileFilter.Text"));
	dialog.AddFilter("*", T("FileFilter.AllFiles"));

	if (dialog.ShowModal() == KxID_OK)
	{
		KxTextFile::WriteToFile(dialog.GetResult(), saveEntry->GetPluginsList());
	}
}
void KSaveManagerWorkspace::OnRemoveSave(KSaveFile* saveEntry)
{
	if (saveEntry)
	{
		const KSaveManagerConfig* config = KSaveManagerConfig::GetInstance();
		const KxFileFinderItem& primaryInfo = saveEntry->GetFileInfo();

		KxFile(primaryInfo.GetFullPath()).RemoveFile(true);
		if (config->HasMultiFileSaveConfig())
		{
			KxFileFinderItem tSecondaryInfo = primaryInfo;
			tSecondaryInfo.SetName(primaryInfo.GetName().BeforeLast('.') + '.' + config->GetSecondarySaveExtension());
			KxFile(tSecondaryInfo.GetFullPath()).RemoveFile(true);
		}
	}
}

bool KSaveManagerWorkspace::OnOpenWorkspace()
{
	if (KPluginManager* pPM = KPluginManager::GetInstance())
	{
		pPM->LoadIfNeeded();
	}
	return true;
}
bool KSaveManagerWorkspace::OnCloseWorkspace()
{
	KMainWindow::GetInstance()->ClearStatus(1);
	return true;
}
void KSaveManagerWorkspace::OnReloadWorkspace()
{
	LoadData();
}

wxString KSaveManagerWorkspace::GetID() const
{
	return "KSaveManagerWorkspace";
}
wxString KSaveManagerWorkspace::GetName() const
{
	return T("SaveManager.Name");
}
wxString KSaveManagerWorkspace::GetNameShort() const
{
	return T("SaveManager.NameShort");
}

void KSaveManagerWorkspace::LoadData()
{
	KxStringVector filterList;
	for (const wxString& filter: m_ActiveFilters)
	{
		filterList.push_back(filter);
	}
	m_ViewModel->SetDataVector(KSaveManager::GetInstance()->GetSavesLocation(), filterList);
}

void KSaveManagerWorkspace::ProcessSelection(const KSaveFile* saveEntry)
{
	const int statusIndex = 1;
	KMainWindow* mainWindow = KMainWindow::GetInstance();
	mainWindow->ClearStatus(statusIndex);

	if (saveEntry)
	{
		if (saveEntry->IsOK())
		{
			mainWindow->SetStatus(saveEntry->GetFileInfo().GetName(), statusIndex);
		}
		else
		{
			mainWindow->SetStatus(T("SaveManager.InvalidFile"), statusIndex, KIMG_CROSS_CIRCLE_FRAME);
		}
	}
}
void KSaveManagerWorkspace::ProcessContextMenu(const KSaveFile* saveEntry)
{
	KxMenu menu;
	CreateContextMenu(menu, saveEntry);

	switch (menu.Show(this))
	{
		case KxID_APPLY:
		{
			OnSyncPluginsList(saveEntry);
			break;
		}
		case KxID_SAVE:
		{
			OnSavePluginsList(saveEntry);
			break;
		}
		case KxID_REFRESH:
		{
			ReloadWorkspace();
			break;
		}
		case KxID_REMOVE:
		{
			KxDataViewItem::Vector items;
			m_ViewModel->GetView()->GetSelections(items);
			if (!items.empty())
			{
				for (KxDataViewItem& item: items)
				{
					OnRemoveSave(m_ViewModel->GetDataEntry(m_ViewModel->GetRow(item)));
				}
				LoadData();
			}
			break;
		}
	};
}
