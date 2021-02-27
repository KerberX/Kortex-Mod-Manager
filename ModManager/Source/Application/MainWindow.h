#pragma once
#include "Framework.hpp"
#include "IMainWindow.h"

namespace Kortex::Application
{
	class KORTEX_API MainWindow: private kxf::UI::Frame, public kxf::RTTI::Implementation<MainWindow, IMainWindow, kxf::EvtHandler>
	{
		private:
			kxf::UI::Frame* m_Frame = nullptr;
			kxf::UI::AuiToolBar* m_MainToolBar = nullptr;
			kxf::UI::AuiToolBar* m_QuickToolBar = nullptr;
			kxf::UI::StatusBarEx* m_StatusBar = nullptr;
			kxf::UI::Menu m_WorkspacesMenu;
			IWorkspaceContainer* m_WorkspaceContainer = nullptr;

		public:
			MainWindow();
			~MainWindow();

		public:
			kxf::UI::Frame& GetFrame() override
			{
				return *m_Frame;
			}
			const kxf::UI::Frame& GetFrame() const override
			{
				return *m_Frame;
			}

			kxf::UI::AuiToolBar& GetMainToolBar() override
			{
				return *m_MainToolBar;
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
				return *m_WorkspaceContainer;
			}
			const IWorkspaceContainer& GetWorkspaceContainer() const override
			{
				return *m_WorkspaceContainer;
			}

			void ClearStatus(int index = 0)
			{
			}
			void SetStatus(const kxf::String& label, int index = 0, const kxf::ResourceID& image = {})
			{
			}
			void SetStatusProgress(int current)
			{
			}
			void SetStatusProgress(int64_t current, int64_t total)
			{
			}

			kxf::UI::AuiToolBarItem* AddToolBarItem(IWorkspace& workspace) override
			{
				return nullptr;
			}
			kxf::UI::MenuItem* AddToolBarMenuItem(IWorkspace& workspace) override
			{
				return nullptr;
			}
	};
}