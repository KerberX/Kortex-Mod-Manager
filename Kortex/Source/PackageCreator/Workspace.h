#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include "Application/BookWorkspaceContainer.h"
#include "WorkspaceDocument.h"
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/kxf::UI::SplitterWindow.h>
#include <kxf::UI::Framework/kxf::UI::Simplebook.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/KxPanel.h>
#include <kxf::UI::Framework/KxTextBox.h>
#include <kxf::UI::Framework/KxTreeList.h>
#include <kxf::UI::Framework/KxButton.h>
#include <kxf::UI::Framework/kxf::UI::Menu.h>

namespace Kortex
{
	class IGameMod;
	class ModPackageProject;
}
namespace Kortex::PackageDesigner
{
	class WorkspaceDocument;

	class PageBase;
	class PageInfo;
	class PageFileData;
	class PageInterface;
	class PageRequirements;
	class PageComponents;
}

namespace Kortex::PackageDesigner
{
	class Workspace;
	class WorkspaceContainer: public Application::BookWorkspaceContainer
	{
		friend class Workspace;

		private:
			Workspace& m_Workspace;
			kxf::UI::Simplebook* m_BookCtrl = nullptr;
			KxTreeList* m_PagesList = nullptr;

		private:
			void Create(wxWindow* listParent, wxWindow* viewParent);
			void OnPageSelected(wxTreeListEvent& event);
			
		protected:
			void ShowWorkspace(IWorkspace& workspace) override;
			void HideWorkspace(IWorkspace& workspace) override;
		
		public:
			WorkspaceContainer(Workspace& workspace)
				:m_Workspace(workspace)
			{
			}

		public:
			wxWindow& GetWindow() override
			{
				return *m_BookCtrl;
			}
			wxWindow& GetListWindow()
			{
				return *m_PagesList;
			}

			bool AddWorkspace(IWorkspace& workspace) override;
			bool RemoveWorkspace(IWorkspace& workspace) override;
	};
}

namespace Kortex::PackageDesigner
{
	class Workspace: public Application::DefaultWindowWorkspace<KxPanel>, public kxf::SingletonPtr<Workspace>
	{
		friend class PageBase;
		friend class WorkspaceDocument;

		private:
			WorkspaceDocument m_WorkspaceDocument;

			kxf::UI::AuiToolBar* m_MenuBar = nullptr;
			kxf::UI::AuiToolBarItem* m_MenuBar_Project = nullptr;
			kxf::UI::AuiToolBarItem* m_MenuBar_Import = nullptr;
			kxf::UI::AuiToolBarItem* m_MenuBar_Build = nullptr;

			kxf::UI::SplitterWindow* m_SplitterLeftRight = nullptr;
			KxPanel* m_RightPane = nullptr;

			// Pages
			WorkspaceContainer m_PagesContainer;
			PageInfo* m_PageInfo = nullptr;
			PageFileData* m_PageFileData = nullptr;
			PageInterface* m_PageInterface = nullptr;
			PageRequirements* m_PageRequirements = nullptr;
			PageComponents* m_PageComponents = nullptr;

		private:
			void CreateMenuBar(wxSizer* sizer);
			void CreateProjectMenu();
			void CreateImportMenu();
			void CreateBuildMenu();

			void DoLoadAllPages();
			void OnNewProject(kxf::UI::MenuEvent& event);
			void OnOpenProject(kxf::UI::MenuEvent& event);
			void OnSaveProject(kxf::UI::MenuEvent& event);
			void OnImportProject(kxf::UI::MenuEvent& event);
			void OnExportProject(kxf::UI::MenuEvent& event);
			void OnBuildProject(kxf::UI::MenuEvent& event);
			void OnBuildProjectPreview(kxf::UI::MenuEvent& event);

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;
			void OnReloadWorkspace() override;

		public:
			Workspace();
			~Workspace();

		public:
			kxf::String GetID() const override;
			kxf::String GetName() const override;
			kxf::ResourceID GetIcon() const override
			{
				return ImageResourceID::Box;
			}
			IWorkspaceContainer* GetPreferredContainer() const override;

			void RefreshWindowTitleForProject()
			{
				// TODO: update window title
			}
			PageBase* GetCurrentPage() const;
			ModPackageProject& ImportProjectFromPackage(const kxf::String& path);
			ModPackageProject& CreateProjectFromModEntry(const IGameMod& modEntry);
	};
}
