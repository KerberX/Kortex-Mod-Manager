#pragma once
#include <Kortex/Kortex.hpp>
#include <KxFramework/KxSingleton.h>

namespace Kortex
{
	namespace Internal
	{
		extern const SimpleModuleInfo GameDataTypeInfo;
	}

	class IPluginManager;
	class ISaveManager;
	class IScreenshotsGallery;

	class GameDataModule:
		public ModuleWithTypeInfo<IModule, Internal::GameDataTypeInfo>,
		public KxSingletonPtr<GameDataModule>
	{
		private:
			std::unique_ptr<IPluginManager> m_PluginManager;
			std::unique_ptr<ISaveManager> m_SaveManager;
			std::unique_ptr<IScreenshotsGallery> m_ScreenshotsGallery;

		protected:
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node) override;
			virtual void OnInit() override;
			virtual void OnExit() override;

		private:
			std::unique_ptr<IPluginManager> CreatePluginManager(const kxf::XMLNode& node) const;

		public:
			GameDataModule();

		public:
			virtual ManagerRefVector GetManagers() override;
	};
}
