#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IModule.h"
#include "Common.h"
#include <kxf::UI::Framework/KxSingleton.h>
class kxf::UI::AuiToolBarEvent;
class kxf::UI::AuiToolBarItem;
class kxf::UI::MenuEvent;
class kxf::UI::Menu;

namespace Kortex
{
	namespace Internal
	{
		extern const SimpleModuleInfo NetworkModuleTypeInfo;
	};

	class INetworkManager;
	class IDownloadManager;

	class NetworkModule:
		public ModuleWithTypeInfo<IModule, Internal::NetworkModuleTypeInfo>,
		public kxf::SingletonPtr<NetworkModule>
	{
		private:
			std::unique_ptr<INetworkManager> m_NetworkManager;
			std::unique_ptr<IDownloadManager> m_DownloadManager;

		private:
			virtual void OnInit() override;
			virtual void OnExit() override;
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node) override;

		public:
			NetworkModule();

		public:
			ManagerRefVector GetManagers() override;
	};
}
