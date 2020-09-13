#pragma once
#include <Kortex/Kortex.hpp>
#include "Options/Option.h"
#include "Resources/IImageProvider.h"
#include "Resources/Imagekxf::ResourceID.h"
#include "IWorkspaceContainer.h"
#include <KxFramework/KxSingleton.h>
#include <KxFramework/KxAuiToolBar.h>
#include <KxFramework/KxStatusBarEx.h>
#include <KxFramework/KxFrame.h>
#include <KxFramework/KxMenu.h>

namespace Kortex
{
	class IManager;
	class VirtualFSEvent;
}

namespace Kortex
{
	class IMainWindow: public KxSingletonPtr<IMainWindow>, public Application::WithOptions<IMainWindow>
	{
		public:
			static wxSize GetDialogBestSize(const wxWindow* dialog);

		public:
			virtual KxFrame& GetFrame() = 0;
			const KxFrame& GetFrame() const
			{
				return const_cast<IMainWindow&>(*this).GetFrame();
			}

			virtual KxAuiToolBar& GetMainToolBar() = 0;
			virtual KxAuiToolBar& GetQuickToolBar() = 0;
			virtual KxStatusBarEx& GetStatusBar() = 0;
			virtual KxMenu& GetWorkspacesMenu() = 0;

			virtual IWorkspaceContainer& GetWorkspaceContainer() = 0;
			const IWorkspaceContainer& GetWorkspaceContainer() const
			{
				return const_cast<IMainWindow&>(*this).GetWorkspaceContainer();
			}

			virtual void ClearStatus(int index = 0) = 0;
			virtual void SetStatus(const kxf::String& label, int index = 0, const kxf::ResourceID& image = {}) = 0;
			virtual void SetStatusProgress(int current) = 0;
			virtual void SetStatusProgress(int64_t current, int64_t total) = 0;

			virtual KxAuiToolBarItem* AddToolBarItem(IWorkspace& workspace) = 0;
			virtual KxMenuItem* AddToolBarMenuItem(IWorkspace& workspace) = 0;

			void InitializeWorkspaces();
	};
}
