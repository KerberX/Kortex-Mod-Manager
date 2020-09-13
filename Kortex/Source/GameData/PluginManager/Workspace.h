#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include <KxFramework/KxSplitterWindow.h>
#include <KxFramework/KxAuiToolBar.h>
#include <KxFramework/KxPanel.h>
#include <KxFramework/KxTextBox.h>
#include <KxFramework/KxTreeList.h>
#include <KxFramework/KxMenu.h>
#include <KxFramework/KxButton.h>
#include <KxFramework/KxSingleton.h>
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

	class Workspace: public Application::DefaultWindowWorkspace<KxPanel>, public KxSingletonPtr<Workspace>
	{
		public:
			static Imagekxf::ResourceID GetStatusImageForPlugin(const IGamePlugin* plugin = nullptr);

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
			void UpdatePluginTypeCounter(KxMenuItem* item);
			void OnRunLootAPI(KxMenuEvent& event);

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
				return Imagekxf::ResourceID::PlugDisconnect;
			}
			IWorkspaceContainer* GetPreferredContainer() const override;

		public:
			PluginViewModel* GetModel() const
			{
				return m_ModelView;
			}
		
			void OnCreateViewContextMenu(KxMenu& menu, const IGamePlugin* plugin);
			void OnCreateSortingToolsMenu(KxMenu& menu, const IGamePlugin* plugin);
			void OnCreateImportExportMenu(KxMenu& menu, const IGamePlugin* plugin);

			void ProcessSelection(const IGamePlugin* plugin = nullptr);
			void HighlightPlugin(const IGamePlugin* plugin = nullptr);
	};
}
