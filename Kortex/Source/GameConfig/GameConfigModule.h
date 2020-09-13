#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IModule.h"
#include <KxFramework/KxSingleton.h>

namespace Kortex
{
	class IGameConfigManager;

	namespace Internal
	{
		extern const SimpleModuleInfo GameConfigModuleTypeInfo;
	};

	class GameConfigModule:
		public ModuleWithTypeInfo<IModule, Internal::GameConfigModuleTypeInfo>,
		public KxSingletonPtr<GameConfigModule>
	{
		friend class IMainWindow;

		private:
			std::unique_ptr<IGameConfigManager> m_GameConfigManager;

		private:
			virtual void OnInit() override;
			virtual void OnExit() override;
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node) override;

		private:
			std::unique_ptr<IGameConfigManager> CreateGameConfigManager(const kxf::XMLNode& node) const;

		public:
			GameConfigModule();

		public:
			ManagerRefVector GetManagers() override;
	};
}
