#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include <kxf::UI::Framework/kxf::UI::SplitterWindow.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/KxPanel.h>
#include <kxf::UI::Framework/KxTextBox.h>
#include <kxf::UI::Framework/KxTreeList.h>
#include <kxf::UI::Framework/KxButton.h>
#include <kxf::UI::Framework/kxf::UI::Menu.h>
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex
{
	class IGameSave;
	class ISaveManager;
}

namespace Kortex::SaveManager
{
	class IBethesdaGameSave;
	class DisplayModel;

	class Workspace: public Application::DefaultWindowWorkspace<KxPanel>, public kxf::SingletonPtr<Workspace>
	{
		private:
			std::unordered_set<kxf::String> m_ActiveFilters;

			wxBoxSizer* m_MainSizer = nullptr;
			wxBoxSizer* m_ViewSizer = nullptr;
			kxf::UI::SplitterWindow* m_Splitter = nullptr;
			DisplayModel* m_DisplayModel = nullptr;

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;
			void OnReloadWorkspace() override;

		public:
			~Workspace();

		private:
			void CreateViewPane();
			void UpdateFilters();

			bool FiltersMenu_IsAllFiltersActive() const;
			bool FiltersMenu_IsFilterActive(const kxf::String& filter) const
			{
				return m_ActiveFilters.count(filter);
			}
			void FiltersMenu_AllFiles(kxf::UI::MenuEvent& event);
			void FiltersMenu_SpecificFilter(kxf::UI::MenuEvent& event);

			void OnSyncPluginsList(const IBethesdaGameSave& save);
			void OnSavePluginsList(const IBethesdaGameSave& save);
			bool OnRemoveSave(IGameSave& save);

		public:
			kxf::String GetID() const override;
			kxf::String GetName() const override;
			kxf::ResourceID GetIcon() const override
			{
				return ImageResourceID::Jar;
			}
			IWorkspaceContainer* GetPreferredContainer() const override;

		public:
			void OnSelection(const IGameSave* save);
			void OnContextMenu(const IGameSave* save);
	};
}
