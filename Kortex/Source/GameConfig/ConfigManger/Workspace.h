#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include "DisplayModel.h"
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/KxButton.h>
#include <kxf::UI::Framework/KxPanel.h>

namespace Kortex::GameConfig
{
	class Workspace:
		public Application::DefaultWindowWorkspace<KxPanel>,
		public kxf::SingletonPtr<Workspace>
	{
		private:
			wxBoxSizer* m_MainSizer = nullptr;
			DisplayModel m_DisplayModel;

			KxButton* m_SaveButton = nullptr;
			KxButton* m_DiscardButton = nullptr;

		private:
			void OnSaveButton(wxCommandEvent& event);
			void OnDiscardButton(wxCommandEvent& event);

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
				return ImageResourceID::GearPencil;
			}
			IWorkspaceContainer* GetPreferredContainer() const override;

		public:
			void OnChangesMade();
			void OnChangesSaved();
			void OnChangesDiscarded();
	};
}
