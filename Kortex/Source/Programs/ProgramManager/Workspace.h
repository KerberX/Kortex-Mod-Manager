#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/KxPanel.h>

namespace Kortex
{
	class VirtualFSEvent;
}

namespace Kortex::ProgramManager
{
	class DisplayModel;

	class Workspace: public Application::DefaultWindowWorkspace<KxPanel>, public kxf::SingletonPtr<Workspace>
	{
		private:
			BroadcastReciever m_BroadcastReciever;
			wxBoxSizer* m_MainSizer = nullptr;
			DisplayModel* m_ViewModel = nullptr;

		private:
			void OnMainFSToggled(VirtualFSEvent& event);

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
				return ImageResourceID::ApplicationRun;
			}
			IWorkspaceContainer* GetPreferredContainer() const override;
	};
}
