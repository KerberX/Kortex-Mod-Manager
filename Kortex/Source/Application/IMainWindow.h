#pragma once
#include <Kortex/Kortex.hpp>
#include "Options/Option.h"
#include "Resources/IImageProvider.h"
#include "Resources/ImageResourceID.h"
#include "IWorkspaceContainer.h"
#include <kxf/General/Singleton.h>
#include <kxf/UI/Controls/AUI/AuiToolBar.h>
#include <kxf/UI/Controls/StatusBarEx.h>
#include <kxf/UI/Windows/Frame.h>
#include <kxf/UI/Menus/Menu.h>

namespace Kortex
{
	class IManager;
	class VirtualFSEvent;
}

namespace Kortex
{
	class IMainWindow: public kxf::SingletonPtr<IMainWindow>, public Application::WithOptions<IMainWindow>
	{
		public:
			static kxf::Size GetDialogBestSize(const wxWindow* dialog);

		public:
			virtual kxf::UI::Frame& GetFrame() = 0;
			const kxf::UI::Frame& GetFrame() const
			{
				return const_cast<IMainWindow&>(*this).GetFrame();
			}

			virtual kxf::UI::AuiToolBar& GetMainToolBar() = 0;
			virtual kxf::UI::AuiToolBar& GetQuickToolBar() = 0;
			virtual kxf::UI::StatusBarEx& GetStatusBar() = 0;
			virtual kxf::UI::Menu& GetWorkspacesMenu() = 0;

			virtual IWorkspaceContainer& GetWorkspaceContainer() = 0;
			const IWorkspaceContainer& GetWorkspaceContainer() const
			{
				return const_cast<IMainWindow&>(*this).GetWorkspaceContainer();
			}

			virtual void ClearStatus(int index = 0) = 0;
			virtual void SetStatus(const kxf::String& label, int index = 0, const kxf::ResourceID& image = {}) = 0;
			virtual void SetStatusProgress(int current) = 0;
			virtual void SetStatusProgress(int64_t current, int64_t total) = 0;

			virtual kxf::UI::AuiToolBarItem* AddToolBarItem(IWorkspace& workspace) = 0;
			virtual kxf::UI::MenuItem* AddToolBarMenuItem(IWorkspace& workspace) = 0;

			void InitializeWorkspaces();
	};
}
