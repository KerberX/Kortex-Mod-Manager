#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IManager.h"
#include "Common.h"
#include "IModNetwork.h"
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/KxCURL.h>
#include <kxf::UI::Framework/KxURI.h>
class IMainWindow;
class kxf::UI::AuiToolBarItem;
class kxf::UI::AuiToolBarEvent;
class KxCURLSession;
class KxIWebSocketClient;

namespace Kortex
{
	class ModNetworkRepository;
}

namespace Kortex
{
	namespace NetworkManager
	{
		class Config;
	}
	namespace NetworkManager::Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	};

	class INetworkManager:
		public ManagerWithTypeInfo<IManager, NetworkManager::Internal::TypeInfo>,
		public Application::ManagerWithToolbarButton,
		public kxf::SingletonPtr<INetworkManager>
	{
		friend class IMainWindow;

		protected:
			enum class NetworkSoftware
			{
				LibCURL,
				WebSocket,
			};

		protected:
			kxf::String GetUserAgentString(NetworkSoftware networkSoftware) const;

			void OnInit() override;
			void OnExit() override;
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;
			
		public:
			INetworkManager();

		public:
			virtual kxf::String GetCacheDirectory() const = 0;
			
			virtual IModNetwork::RefVector GetModNetworks() = 0;
			std::vector<ModNetworkRepository*> GetModRepositories();

			virtual IModNetwork* GetDefaultModNetwork() const = 0;
			virtual IModNetwork* GetModNetworkByName(const kxf::String& name) const = 0;

			virtual void OnAuthStateChanged() = 0;

		public:
			virtual std::unique_ptr<KxIWebSocketClient> NewWebSocketClient(const KxURI& address);
			virtual std::unique_ptr<KxCURLSession> NewCURLSession(const KxURI& address);
			virtual std::unique_ptr<wxFileSystemHandler> NewWxFSHandler();
	};
}
