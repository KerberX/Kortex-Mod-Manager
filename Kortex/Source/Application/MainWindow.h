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
			KxStatusBarEx* m_StatusBar = nullptr;
		
			// ToolBar
			KxAuiToolBar* m_ToolBar = nullptr;
			KxAuiToolBarItem* m_ToolBar_MainMenu = nullptr;
			size_t m_ToolBar_InsertionIndex = KxAuiToolBarItem::npos;

			KxAuiToolBar* m_QuickToolBar = nullptr;
			KxAuiToolBarItem* m_QuickToolBar_QuickSettingsMenu = nullptr;
			KxAuiToolBarItem* m_QuickToolBar_Help = nullptr;

		private:
			void CreateToolBar();
			void CreateStatusBar();
			void CreateBaseLayout();
			virtual WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		private:
			void CreateWorkspaces();
			void CreateMainMenu(KxMenu& mainMenu);
			void AddLocationsMenu(KxMenu& mainMenu);

			void OnQSMButton(KxAuiToolBarEvent& event);
			void OnWindowClose(wxCloseEvent& event);
			void OnChangeInstance(KxMenuEvent& event);

			void OnMainFSToggled(bool isActive);
			void OnMainFSToggled(VirtualFSEvent& event);
	
		public:
			MainWindow()
				:m_WorkspaceContainer(*this)
			{
			}
			bool Create(wxWindow* parent = nullptr);

		public:
			KxFrame& GetFrame() override
			{
				return *this;
			}
			KxAuiToolBar& GetMainToolBar() override
			{
				return *m_ToolBar;
			}
			KxAuiToolBar& GetQuickToolBar() override
			{
				return *m_QuickToolBar;
			}
			KxStatusBarEx& GetStatusBar() override
			{
				return *m_StatusBar;
			}
			KxMenu& GetWorkspacesMenu() override
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

			KxAuiToolBarItem* AddToolBarItem(IWorkspace& workspace) override;
			KxMenuItem* AddToolBarMenuItem(IWorkspace& workspace) override;
	};
}