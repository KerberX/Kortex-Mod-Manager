#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include <kxf::UI::Framework/kxf::UI::SplitterWindow.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/KxPanel.h>
#include <kxf::UI::Framework/KxTextBox.h>
#include <kxf::UI::Framework/KxTreeList.h>
#include <kxf::UI::Framework/kxf::UI::Menu.h>
#include <kxf::UI::Framework/KxButton.h>
#include <kxf::UI::Framework/KxSingleton.h>
class KxSearchBox;
class KxHTMLWindow;

namespace Kortex
{
	class IGamePlugin;
	class ModEvent;
}

namespace Kortex::PluginManager
{
	class PluginViewModel;

	class Workspace: public Application::DefaultWindowWorkspace<KxPanel>, public kxf::SingletonPtr<Workspace>
	{
		public:
			static ImageResourceID GetStatusImageForPlugin(const IGamePlugin* plugin = nullptr);

		private:
			BroadcastReciever m_BroadcastReciever;
			
			wxBoxSizer* m_MainSizer = nullptr;
			PluginViewModel* m_ModelView = nullptr;
			KxSearchBox* m_SearchBox = nullptr;

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;
			void OnReloadWorkspace() override;

		private:
			void CreateModelView();
			void OnModSerach(wxCommandEvent& event);
			void UpdatePluginTypeCounter(kxf::UI::MenuItem* item);
			void OnRunLootAPI(kxf::UI::MenuEvent& event);

			void OnBeginReload(ModEvent& event);
			void OnEndReload(ModEvent& event);

		public:
			Workspace() = default;
			~Workspace();

		public:
			kxf::String GetID() const override;
			kxf::String GetName() const override;
			kxf::ResourceID GetIcon() const override
			{
				return ImageResourceID::PlugDisconnect;
			}
			IWorkspaceContainer* GetPreferredContainer() const override;

		public:
			PluginViewModel* GetModel() const
			{
				return m_ModelView;
			}
		
			void OnCreateViewContextMenu(kxf::UI::Menu& menu, const IGamePlugin* plugin);
			void OnCreateSortingToolsMenu(kxf::UI::Menu& menu, const IGamePlugin* plugin);
			void OnCreateImportExportMenu(kxf::UI::Menu& menu, const IGamePlugin* plugin);

			void ProcessSelection(const IGamePlugin* plugin = nullptr);
			void HighlightPlugin(const IGamePlugin* plugin = nullptr);
	};
}
