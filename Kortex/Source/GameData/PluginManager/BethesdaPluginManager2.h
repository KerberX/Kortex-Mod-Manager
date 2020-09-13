#pragma once
#include <Kortex/Kortex.hpp>
#include "BethesdaPluginManager.h"
#include "BethesdaPlugin2.h"

namespace Kortex::PluginManager
{
	class Bethesda2DisplayModel;

	class BethesdaPluginManager2: public KxRTTI::ExtendInterface<BethesdaPluginManager2, BethesdaPluginManager>
	{
		KxRTTI_DeclareIID(BethesdaPluginManager2, {0xdbbc84e0, 0xa6a7, 0x4ff0, {0x86, 0x7e, 0x99, 0x4c, 0xd9, 0xaf, 0x55, 0x40}});

		friend class Bethesda2DisplayModel;

		protected:
			void OnInit() override;
			void OnExit() override;
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;

			bool CheckExtension(const kxf::String& name) const override;
			void LoadNativeOrderBG() override;
			void LoadNativeActiveBG() override;
			void SaveNativeOrderBG() const override;

			kxf::String OnWriteToLoadOrder(const IGamePlugin& plugin) const override;
			kxf::String OnWriteToActiveOrder(const IGamePlugin& plugin) const override;

			intptr_t CountLightActiveBefore(const IGamePlugin& plugin) const;
			intptr_t OnGetPluginDisplayPriority(const IGamePlugin& plugin) const override;

		public:
			BethesdaPluginManager2();
			virtual ~BethesdaPluginManager2();

		public:
			std::unique_ptr<IGamePlugin> CreatePlugin(const kxf::String& fullPath, bool isActive) override;
			std::unique_ptr<PluginManager::IDisplayModel> CreateDisplayModel() override;
	};
}
