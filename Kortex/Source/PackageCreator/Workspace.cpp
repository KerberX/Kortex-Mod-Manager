#include "stdafx.h"
#include <Kortex/Application.hpp>
#include <Kortex/ModManager.hpp>
#include "Workspace.h"
#include "PageBase.h"
#include "PageInfo.h"
#include "PageFileData.h"
#include "PageInterface.h"
#include "PageRequirements.h"
#include "PageComponents.h"
#include "FOModImportExportDialog.h"
#include "PackageProject/Common.h"
#include "PackageProject/Serializer/NativeSerializer.h"
#include "PackageProject/Serializer/FOModSerializer.h"
#include "ModPackages/ModPackage.h"
#include "GameMods/ModManager/Workspace.h"
#include <kxf::UI::Framework/KxFileBrowseDialog.h>
#include <kxf::UI::Framework/KxProgressDialog.h>
#include <kxf::UI::Framework/KxTextFile.h>
#include <kxf::UI::Framework/KxFileStream.h>

namespace Kortex::Application::OName
{
	KortexDefOption(Splitter);
	KortexDefOption(Pages);
}

namespace
{
	template<class T> auto GetUIOption(T&& option)
	{
		using namespace Kortex;
		using namespace Kortex::Application;

		return Application::GetAInstanceOptionOf<PackageDesigner::Workspace>(std::forward<T>(option));
	}
}

namespace Kortex::PackageDesigner
{
	void WorkspaceContainer::Create(wxWindow* listParent, wxWindow* viewParent)
	{
		// List
		m_PagesList = new KxTreeList(listParent, wxID_NONE, KxTreeList::DefaultStyle|wxTL_NO_HEADER);
		m_PagesList->GetDataView()->ToggleWindowStyle(wxBORDER_NONE);
		m_PagesList->SetImageList(&ImageProvider::GetImageList());
		m_PagesList->SetRowHeight(m_PagesList->FromDIP(36));

		m_PagesList->AddColumn(wxEmptyString);
		m_PagesList->Bind(wxEVT_TREELIST_SELECTION_CHANGED, &WorkspaceContainer::OnPageSelected, this);
		IThemeManager::GetActive().Apply(m_PagesList);

		// View
		m_BookCtrl = new kxf::UI::Simplebook(viewParent, wxID_NONE);
		IThemeManager::GetActive().Apply(m_BookCtrl);
	}
	void WorkspaceContainer::OnPageSelected(wxTreeListEvent& event)
	{
		if (KxTreeListItem item(*m_PagesList, event.GetItem()); item.IsOK())
		{
			if (auto clientData = static_cast<Application::WorkspaceClientData*>(item.GetData()))
			{
				clientData->GetWorkspace().SwitchHere();
			}
		}
	}

	void WorkspaceContainer::ShowWorkspace(IWorkspace& workspace)
	{
		if (auto index = GetWorkspaceIndex(workspace))
		{
			BookWorkspaceContainer::ShowWorkspace(workspace);

			KxTreeListItem root = m_PagesList->GetRoot();
			for (KxTreeListItem item = root.GetFirstChild(); item.IsOK(); item = item.GetNextSibling())
			{
				auto clientData = static_cast<Application::WorkspaceClientData*>(item.GetData());
				if (clientData && &clientData->GetWorkspace() == &workspace)
				{
					item.SetSelection();
					break;
				}
			}
		}
	}
	void WorkspaceContainer::HideWorkspace(IWorkspace& workspace)
	{
		return BookWorkspaceContainer::HideWorkspace(workspace);
	}

	bool WorkspaceContainer::AddWorkspace(IWorkspace& workspace)
	{
		if (Application::BookWorkspaceContainer::AddWorkspace(workspace))
		{
			KxTreeListItem item = m_PagesList->GetRoot().Add(workspace.GetName(), new Application::WorkspaceClientData(workspace));
			if (auto icon = workspace.GetIcon().TryAsInt())
			{
				// TODO: KxTreeList is broken, replace with proper control such as KxDataView2
				m_PagesList->wxTreeListCtrl::SetItemImage(item, *icon, *icon);
			}

			return true;
		}
		return false;
	}
	bool WorkspaceContainer::RemoveWorkspace(IWorkspace& workspace)
	{
		if (Application::BookWorkspaceContainer::RemoveWorkspace(workspace))
		{
			KxTreeListItem root = m_PagesList->GetRoot();
			for (KxTreeListItem item = root.GetFirstChild(); item.IsOK(); item = item.GetNextSibling())
			{
				auto clientData = static_cast<Application::WorkspaceClientData*>(item.GetData());
				if (clientData && &clientData->GetWorkspace() == &workspace)
				{
					item.Remove();
					return true;
				}
			}
		}
		return false;
	}
}

namespace Kortex::PackageDesigner
{
	bool Workspace::OnCreateWorkspace()
	{
		m_SplitterLeftRight = new kxf::UI::SplitterWindow(this, wxID_NONE);
		m_SplitterLeftRight->SetMinimumPaneSize(ImageProvider::GetImageList().GetSize().GetWidth() * 1.5);

		// Right pane
		wxBoxSizer* rightPaneSizer = new wxBoxSizer(wxVERTICAL);
		m_RightPane = new KxPanel(m_SplitterLeftRight, wxID_NONE);
		m_RightPane->SetSizer(rightPaneSizer);
		IThemeManager::GetActive().Apply(m_RightPane);

		// Left pane
		m_PagesContainer.Create(m_SplitterLeftRight, m_RightPane);
		
		// Menu bar
		CreateMenuBar(rightPaneSizer);

		// Layout
		m_SplitterLeftRight->SplitVertically(&m_PagesContainer.GetListWindow(), m_RightPane);
		rightPaneSizer->Add(m_MenuBar, 0, wxEXPAND);
		rightPaneSizer->Add(&m_PagesContainer.GetWindow(), 1, wxEXPAND|wxLEFT, LayoutConstants::HorizontalSpacing);

		// Create pages
		m_PageInfo = &m_PagesContainer.MakeWorkspace<PageInfo>(*this, m_WorkspaceDocument);
		m_PageFileData = &m_PagesContainer.MakeWorkspace<PageFileData>(*this, m_WorkspaceDocument);
		m_PageInterface = &m_PagesContainer.MakeWorkspace<PageInterface>(*this, m_WorkspaceDocument);
		m_PageRequirements = &m_PagesContainer.MakeWorkspace<PageRequirements>(*this, m_WorkspaceDocument);
		m_PageComponents = &m_PagesContainer.MakeWorkspace<PageComponents>(*this, m_WorkspaceDocument);
		return true;
	}
	bool Workspace::OnOpenWorkspace()
	{
		if (!OpenedOnce())
		{
			m_WorkspaceDocument.NewProject();

			using namespace Application;
			GetUIOption(OName::Splitter).LoadSplitterLayout(m_SplitterLeftRight);
			GetUIOption(OName::Pages).LoadWorkspaceContainerLayout(m_PagesContainer);
		}
		return true;
	}
	bool Workspace::OnCloseWorkspace()
	{
		return m_WorkspaceDocument.AskForSave() == wxID_OK;
	}
	void Workspace::OnReloadWorkspace()
	{
	}

	Workspace::Workspace()
		:m_WorkspaceDocument(*this), m_PagesContainer(*this)
	{
		IMainWindow::GetInstance()->AddToolBarMenuItem(*this);
	}
	Workspace::~Workspace()
	{
		if (IsCreated())
		{
			using namespace Application;
			GetUIOption(OName::Splitter).SaveSplitterLayout(m_SplitterLeftRight);
			GetUIOption(OName::Pages).SaveWorkspaceContainerLayout(m_PagesContainer);
		}
	}

	void Workspace::CreateMenuBar(wxSizer* sizer)
	{
		m_MenuBar = new kxf::UI::AuiToolBar(m_RightPane, wxID_NONE, kxf::UI::AuiToolBar::DefaultStyle|wxAUI_TB_PLAIN_BACKGROUND|wxAUI_TB_TEXT);
		m_MenuBar->SetBackgroundColour(GetBackgroundColour());
		m_MenuBar->SetBorderColor(IThemeManager::GetActive().GetColor(Theme::ColorIndex::Border, Theme::ColorFlag::Background));
		m_MenuBar->SetToolPacking(0);
		m_MenuBar->SetToolSeparation(0);
		m_MenuBar->SetMargins(0, 0, 0, 2);
		IThemeManager::GetActive().Apply(static_cast<wxWindow*>(m_MenuBar));

		m_MenuBar_Project = m_MenuBar->AddTool(KTr("PackageCreator.MenuBar.Project"), wxNullBitmap);
		m_MenuBar_Import = m_MenuBar->AddTool(KTr("PackageCreator.MenuBar.Import"), wxNullBitmap);
		m_MenuBar_Build = m_MenuBar->AddTool(KTr("PackageCreator.MenuBar.Build"), wxNullBitmap);

		m_MenuBar->UpdateUI();
		CreateProjectMenu();
		CreateImportMenu();
		CreateBuildMenu();
	}
	void Workspace::CreateProjectMenu()
	{
		kxf::UI::Menu* menu = new kxf::UI::Menu();
		m_MenuBar_Project->AssignDropdownMenu(menu);
		m_MenuBar_Project->SetOptionEnabled(KxAUI_TBITEM_OPTION_LCLICK_MENU);
		kxf::UI::MenuItem* item = nullptr;

		item = menu->Add(new kxf::UI::MenuItem(wxID_NEW, KTr("PackageCreator.MenuProject.New")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::DocumentNew));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnNewProject, this);

		item = menu->Add(new kxf::UI::MenuItem(wxID_OPEN, KTr("PackageCreator.MenuProject.Open")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Document));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnOpenProject, this);
		menu->AddSeparator();

		item = menu->Add(new kxf::UI::MenuItem(wxID_SAVE, KTr("PackageCreator.MenuProject.Save")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Disk));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnSaveProject, this);

		item = menu->Add(new kxf::UI::MenuItem(wxID_SAVEAS, KTr("PackageCreator.MenuProject.SaveAs")));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnSaveProject, this);

		item = menu->Add(new kxf::UI::MenuItem(wxID_HIGHEST + ToInt(PackageProject::PackageType::FOModXML), KTr("PackageCreator.MenuProject.SaveAsFOMod")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::DocumentExport));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnExportProject, this);

		item = menu->Add(new kxf::UI::MenuItem(wxID_HIGHEST + ToInt(PackageProject::PackageType::Native), KTr("PackageCreator.MenuProject.SaveAsKMP")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::DocumentExport));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnExportProject, this);
	}
	void Workspace::CreateImportMenu()
	{
		kxf::UI::Menu* menu = new kxf::UI::Menu();
		m_MenuBar_Import->AssignDropdownMenu(menu);
		m_MenuBar_Import->SetOptionEnabled(KxAUI_TBITEM_OPTION_LCLICK_MENU);
		kxf::UI::MenuItem* item = nullptr;

		item = menu->Add(new kxf::UI::MenuItem(wxID_HIGHEST + 0, KTr("PackageCreator.MenuImport.FOModXML")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::DocumentImport));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnImportProject, this);

		item = menu->Add(new kxf::UI::MenuItem(wxID_HIGHEST + 1, KTr("PackageCreator.MenuImport.Package")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::DocumentImport));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnImportProject, this);
	}
	void Workspace::CreateBuildMenu()
	{
		kxf::UI::Menu* menu = new kxf::UI::Menu();
		m_MenuBar_Build->AssignDropdownMenu(menu);
		m_MenuBar_Build->SetOptionEnabled(KxAUI_TBITEM_OPTION_LCLICK_MENU);
		kxf::UI::MenuItem* item = nullptr;

		item = menu->Add(new kxf::UI::MenuItem(KTr("PackageCreator.MenuBuild.Build")));
		item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Compile));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnBuildProject, this);

		item = menu->Add(new kxf::UI::MenuItem(KTr("PackageCreator.MenuBuild.Preview")));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, &Workspace::OnBuildProjectPreview, this);
	}

	void Workspace::DoLoadAllPages()
	{
		m_PageInfo->EnsureCreated();
		m_PageFileData->EnsureCreated();
		m_PageInterface->EnsureCreated();
		m_PageRequirements->EnsureCreated();
		m_PageComponents->EnsureCreated();

		m_PageInfo->OnLoadProject(m_WorkspaceDocument.GetProject()->GetInfo());
		m_PageFileData->OnLoadProject(m_WorkspaceDocument.GetProject()->GetFileData());
		m_PageInterface->OnLoadProject(m_WorkspaceDocument.GetProject()->GetInterface());
		m_PageRequirements->OnLoadProject(m_WorkspaceDocument.GetProject()->GetRequirements());
		m_PageComponents->OnLoadProject(m_WorkspaceDocument.GetProject()->GetComponents());
	}
	void Workspace::OnNewProject(kxf::UI::MenuEvent& event)
	{
		if (m_WorkspaceDocument.AskForSave() == wxID_OK)
		{
			m_WorkspaceDocument.NewProject();
			DoLoadAllPages();
		}
	}
	void Workspace::OnOpenProject(kxf::UI::MenuEvent& event)
	{
		wxWindowUpdateLocker lock(this);

		if (m_WorkspaceDocument.AskForSave() == wxID_OK)
		{
			KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_OPEN);
			dialog.AddFilter("*.kmpproj", KTr("FileFilter.ModProject"));
			dialog.AddFilter("*", KTr("FileFilter.AllFiles"));

			if (dialog.ShowModal() == wxID_OK)
			{
				m_WorkspaceDocument.OpenProject(dialog.GetResult());
				DoLoadAllPages();
			}
		}
	}
	void Workspace::OnSaveProject(kxf::UI::MenuEvent& event)
	{
		if (event.GetItem()->GetId() == wxID_SAVEAS || !m_WorkspaceDocument.HasProjectFilePath() || !wxFileName(m_WorkspaceDocument.GetProjectFilePath()).Exists(wxFILE_EXISTS_REGULAR))
		{
			KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_SAVE);
			dialog.SetDefaultExtension("kmpproj");
			dialog.SetFileName(m_WorkspaceDocument.GetProjectName());
			dialog.AddFilter("*.kmpproj", KTr("FileFilter.ModProject"));
			dialog.AddFilter("*", KTr("FileFilter.AllFiles"));

			if (dialog.ShowModal() == wxID_OK)
			{
				m_WorkspaceDocument.SaveProject(dialog.GetResult());
			}
		}
		else
		{
			m_WorkspaceDocument.SaveProject();
		}
	}
	void Workspace::OnImportProject(kxf::UI::MenuEvent& event)
	{
		wxWindowUpdateLocker lock(this);
		if (m_WorkspaceDocument.AskForSave() == wxID_OK)
		{
			kxf::UI::MenuItem* item = event.GetItem();
			switch (item->GetId() - wxID_HIGHEST)
			{
				case 0:
				{
					FOModImportExportDialog dialog(this, false);
					if (dialog.ShowModal() == wxID_OK)
					{
						kxf::String info = KxTextFile::ReadToString(dialog.GetInfoFile());
						kxf::String sModuleConfig = KxTextFile::ReadToString(dialog.GetModuleConfigFile());

						PackageProject::FOModSerializer serailizer(info, sModuleConfig, dialog.GetProjectFolder());
						m_WorkspaceDocument.ImportProject(serailizer);
					}
					break;
				}
				case 1:
				{
					KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_OPEN);
					dialog.AddFilter("*.kmp;*.smi;*.7z;*.zip;*.fomod", KTr("FileFilter.AllSupportedFormats"));
					dialog.AddFilter("*", KTr("FileFilter.AllFiles"));
					if (dialog.ShowModal() == wxID_OK)
					{
						ImportProjectFromPackage(dialog.GetResult());
					}
					break;
				}
			};
		}
	}
	void Workspace::OnExportProject(kxf::UI::MenuEvent& event)
	{
		PackageProject::PackageType type = (PackageProject::PackageType)(event.GetItem()->GetId() - wxID_HIGHEST);
		switch (type)
		{
			case PackageProject::PackageType::Native:
			{
				KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_SAVE);
				dialog.SetDefaultExtension("xml");
				dialog.SetFileName(m_WorkspaceDocument.GetProjectName());
				dialog.AddFilter("*.xml", KTr("FileFilter.XML"));
				dialog.AddFilter("*", KTr("FileFilter.AllFiles"));

				if (dialog.ShowModal() == wxID_OK)
				{
					PackageProject::NativeSerializer serializer(false);
					m_WorkspaceDocument.ExportProject(serializer);
					KxTextFile::WriteToFile(dialog.GetResult(), serializer.GetData());
				}
				break;
			}
			case PackageProject::PackageType::FOModXML:
			{
				FOModImportExportDialog dialog(this, true);
				if (dialog.ShowModal() == wxID_OK)
				{
					PackageProject::FOModSerializer serializer(dialog.GetProjectFolder());
					serializer.ExportToNativeFormat(true);
					m_WorkspaceDocument.ExportProject(serializer);

					if (!dialog.GetInfoFile().IsEmpty())
					{
						KxTextFile::WriteToFile(dialog.GetInfoFile(), serializer.GetInfoXML());
					}
					if (!dialog.GetModuleConfigFile().IsEmpty())
					{
						KxTextFile::WriteToFile(dialog.GetModuleConfigFile(), serializer.GetModuleConfigXML());
					}
				}
				break;
			}
		};
	}
	void Workspace::OnBuildProject(kxf::UI::MenuEvent& event)
	{
		m_WorkspaceDocument.BuildProject();
	}
	void Workspace::OnBuildProjectPreview(kxf::UI::MenuEvent& event)
	{
		m_WorkspaceDocument.BuildProject(true);
	}

	kxf::String Workspace::GetID() const
	{
		return "PackageDesigner::Workspace";
	}
	kxf::String Workspace::GetName() const
	{
		return KTr("PackageManager.CreatorName");
	}
	IWorkspaceContainer* Workspace::GetPreferredContainer() const
	{
		return &IMainWindow::GetInstance()->GetWorkspaceContainer();
	}

	PageBase* Workspace::GetCurrentPage() const
	{
		return static_cast<PageBase*>(m_PagesContainer.GetCurrentWorkspace());
	}
	ModPackageProject& Workspace::ImportProjectFromPackage(const kxf::String& path)
	{
		EnsureCreated();

		m_WorkspaceDocument.ImportProjectFromPackage(path);
		return *m_WorkspaceDocument.GetProject();
	}
	ModPackageProject& Workspace::CreateProjectFromModEntry(const IGameMod& modEntry)
	{
		EnsureCreated();

		m_WorkspaceDocument.CreateProjectFromModEntry(modEntry);
		return *m_WorkspaceDocument.GetProject();
	}
}
