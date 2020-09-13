#pragma once
#include <Kortex/Kortex.hpp>
#include "GameData/IPluginManager.h"
#include "BasePluginManager.h"
#include "BethesdaPlugin.h"
#include "Application/IWorkspace.h"
#include <KxFramework/KxLibrary.h>

namespace Kortex::PluginManager
{
	class LootAPIConfig
	{
		private:
			KxLibrary m_Librray;
			kxf::String m_Branch;
			kxf::String m_Repository;
			kxf::String m_FolderName;
			kxf::String m_LocalGamePath;

		public:
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node);

		public:
			bool IsOK() const;

			kxf::String GetBranch() const;
			kxf::String GetRepository() const;
			kxf::String GetFolderName() const;
			kxf::String GetLocalGamePath() const;
	};
}

namespace Kortex::PluginManager
{
	class LibLoot;
	class BethesdaDisplayModel;

	class BethesdaPluginManager: public KxRTTI::ExtendInterface<BethesdaPluginManager, BasePluginManager>
	{
		KxRTTI_DeclareIID(BethesdaPluginManager, {0xa3eb91ce, 0x513d, 0x4da8, {0xa4, 0x42, 0xf5, 0xb7, 0x10, 0xd4, 0x98, 0x78}});

		protected:
			LootAPIConfig m_LibLootConfig;
			std::unique_ptr<LibLoot> m_LootAPI;

			kxf::String m_PluginsLocation;
			kxf::String m_ActiveListFile;
			kxf::String m_OrderListFile;
			kxf::String m_ActiveFileHeader;
			kxf::String m_OrderFileHeader;

			bool m_ShouldChangeFileModificationDate = false;
			bool m_ShouldSortByFileModificationDate = false;

		protected:
			void OnInit() override;
			void OnExit() override;
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;

			void SortByDate();
			virtual bool CheckExtension(const kxf::String& name) const;

			IGamePlugin::RefVector CollectDependentPlugins(const IGamePlugin& plugin, bool firstOnly) const;
			bool HasDependentPlugins(const IGamePlugin& plugin) const override;
			IGamePlugin::RefVector GetDependentPlugins(const IGamePlugin& plugin) const override;

		protected:
			virtual void LoadNativeOrderBG();
			virtual void LoadNativeActiveBG();
			virtual void SaveNativeOrderBG() const;

			virtual kxf::String OnWriteToLoadOrder(const IGamePlugin& plugin) const;
			virtual kxf::String OnWriteToActiveOrder(const IGamePlugin& plugin) const;
			void CreateWorkspaces() override;

		public:
			BethesdaPluginManager();
			virtual ~BethesdaPluginManager();

		public:
			const LootAPIConfig& GetLibLootConfig() const
			{
				return m_LibLootConfig;
			}
			IWorkspace::RefVector EnumWorkspaces() const override;

			std::unique_ptr<PluginManager::IDisplayModel> CreateDisplayModel() override;
			std::unique_ptr<IPluginReader> CreatePluginReader() override;
			virtual std::unique_ptr<IGamePlugin> CreatePlugin(const kxf::String& fullPath, bool isActive);

			kxf::String GetPluginsLocation() const override
			{
				return m_PluginsLocation;
			}
			kxf::String GetPluginTypeName(const IGamePlugin& plugin) const override;
			kxf::String GetPluginTypeName(bool isMaster, bool isLight) const;
			virtual kxf::String GetPluginRootRelativePath(const kxf::String& fileName) const
			{
				return m_PluginsLocation + wxS('\\') + fileName;
			}
			const IGameMod* FindOwningMod(const IGamePlugin& plugin) const override;

			void Save() const override;
			void Load() override;
			void LoadNativeOrder() override;

			bool ShouldChangeFileModificationDate() const
			{
				return m_ShouldChangeFileModificationDate;
			}
			bool ShouldSortByFileModificationDate() const
			{
				return m_ShouldSortByFileModificationDate;
			}
	};
}
