#pragma once
#include <Kortex/Kortex.hpp>
#include "IMainWindow.h"
#include "IWorkspace.h"
#include "BookWorkspaceContainer.h"
#include "Utility/LabeledValue.h"
#include <kxf/UI/Windows/Frame.h>
#include <kxf/UI/Windows/Panel.h>
#include <kxf/UI/Windows/SplitterWindow.h>
#include <kxf/UI/Controls/ImageView.h>
#include <kxf/UI/Controls/ProgressBar.h>
#include <kxf/UI/Controls/StatusBarEx.h>
#include <kxf/UI/Controls/Simplebook.h>
#include <kxf/UI/Menus/Menu.h>

namespace Kortex
{
	class VirtualFSEvent;
	class IWorkspace;
}

namespace Kortex::Application
{
	class MainWindow;
	class MainWindowWorkspaceContainer: public BookWorkspaceContainer
	{
		friend class MainWindow;

		private:
			MainWindow& m_MainWindow;
			kxf::UI::Simplebook* m_BookCtrl = nullptr;

		private:
			void Create();
			
		protected:
			void ShowWorkspace(IWorkspace& workspace) override;
			void HideWorkspace(IWorkspace& workspace) override;
		
		public:
			MainWindowWorkspaceContainer(MainWindow& mainWindow)
				:m_MainWindow(mainWindow)
			{
			}

		public:
			wxWindow& GetWindow() override
			{
				return *m_BookCtrl;
			}
	};
}

namespace Kortex::Application
{
	class MainWindow: public kxf::UI::Frame, public IMainWindow
	{
		friend class MainWindowWorkspaceContainer;

		public:
			static const void* GetUniqueID();

		private:
			BroadcastReciever m_BroadcastReciever;
			MainWindowWorkspaceContainer m_WorkspaceContainer;
			Utility::LabeledValue::Vector m_Locations;
			kxf::UI::Menu m_WorkspacesMenu;

			wxBoxSizer* m_MainSizer = nullptr;
			wxBoxSizer* m_ToolBarSizer = nullptr;

			// StatusBar
			kxf::UI::StatusBarEx* m_StatusBar = nullptr;
		
			// ToolBar
			kxf::UI::AuiToolBar* m_ToolBar = nullptr;
			kxf::UI::AuiToolBarItem* m_ToolBar_MainMenu = nullptr;
			size_t m_ToolBar_InsertionIndex = std::numeric_limits<size_t>::max();

			kxf::UI::AuiToolBar* m_QuickToolBar = nullptr;
			kxf::UI::AuiToolBarItem* m_QuickToolBar_QuickSettingsMenu = nullptr;
			kxf::UI::AuiToolBarItem* m_QuickToolBar_Help = nullptr;

		private:
			void CreateToolBar();
			void CreateStatusBar();
			void CreateBaseLayout();
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		private:
			void CreateWorkspaces();
			void CreateMainMenu(kxf::UI::Menu& mainMenu);
			void AddLocationsMenu(kxf::UI::Menu& mainMenu);

			void OnQSMButton(kxf::UI::AuiToolBarEvent& event);
			void OnWindowClose(wxCloseEvent& event);
			void OnChangeInstance(kxf::UI::MenuEvent& event);

			void OnMainFSToggled(bool isActive);
			void OnMainFSToggled(VirtualFSEvent& event);
	
		public:
			MainWindow()
				:m_WorkspaceContainer(*this)
			{
			}
			bool Create(wxWindow* parent = nullptr);

		public:
			kxf::UI::Frame& GetFrame() override
			{
				return *this;
			}
			kxf::UI::AuiToolBar& GetMainToolBar() override
			{
				return *m_ToolBar;
			}
			kxf::UI::AuiToolBar& GetQuickToolBar() override
			{
				return *m_QuickToolBar;
			}
			kxf::UI::StatusBarEx& GetStatusBar() override
			{
				return *m_StatusBar;
			}
			kxf::UI::Menu& GetWorkspacesMenu() override
			{
				return m_WorkspacesMenu;
			}
			IWorkspaceContainer& GetWorkspaceContainer() override
			{
				return m_WorkspaceContainer;
			}

			void ClearStatus(int index = 0) override;
			void SetStatus(const kxf::String& label, int index = 0, const kxf::ResourceID& image = {}) override;
			void SetStatusProgress(int current) override;
			void SetStatusProgress(int64_t current, int64_t total) override;

			kxf::UI::AuiToolBarItem* AddToolBarItem(IWorkspace& workspace) override;
			kxf::UI::MenuItem* AddToolBarMenuItem(IWorkspace& workspace) override;
	};
}
