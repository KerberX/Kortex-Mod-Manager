#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/KxPanel.h>

namespace Kortex::DownloadManager
{
	class DisplayModel;

	class Workspace: public Application::DefaultWindowWorkspace<KxPanel>, public kxf::SingletonPtr<Workspace>
	{
		private:
			wxBoxSizer* m_MainSizer = nullptr;
			DisplayModel* m_DisplayModel = nullptr;
			kxf::UI::AuiToolBar* m_ToolBar = nullptr;

		private:
			void OnSelectConcurrentDownloadsCount(wxCommandEvent& event);

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;
			void OnReloadWorkspace() override;

		public:
			Workspace() = default;
			~Workspace();

		public:
			kxf::String GetID() const override;
			kxf::String GetName() const override;
			kxf::ResourceID GetIcon() const override
			{
				return ImageResourceID::Arrow270;
			}
			IWorkspaceContainer* GetPreferredContainer() const override;

		public:
			DisplayModel* GetDisplayModel() const
			{
				return m_DisplayModel;
			}
	};
}
