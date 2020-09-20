#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/INetworkManager.h"
class kxf::UI::AuiToolBarEvent;
class kxf::UI::AuiToolBarItem;
class kxf::UI::MenuEvent;
class kxf::UI::Menu;

namespace Kortex
{
	class ModNetworkAuth;
	class ModNetworkRepository;
}

namespace Kortex::NetworkManager
{
	class DefaultNetworkManager: public INetworkManager
	{
		private:
			IModNetwork::Vector m_ModNetworks;
			IModNetwork* m_DefaultModNetwork = nullptr;

			kxf::UI::AuiToolBarItem* m_LoginButton = nullptr;
			kxf::UI::Menu* m_Menu = nullptr;

		private:
			virtual void OnInit() override;
			virtual void OnExit() override;
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;

			void ValidateAuth();
			bool AdjustDefaultModNetwork();

		private:
			void OnSetToolbarButton(kxf::UI::AuiToolBarItem& button) override;
			void UpdateButton();
			void CreateMenu();
			void QueueUIUpdate();

			void OnSignInOut(kxf::UI::MenuEvent& event);
			void OnSelectDefaultModSource(kxf::UI::MenuEvent& event);
			void OnToolbarButton(kxf::UI::AuiToolBarEvent& event) override;

		public:
			kxf::String GetCacheDirectory() const override;
			
			IModNetwork::RefVector GetModNetworks() override;
			IModNetwork* GetDefaultModNetwork() const override;
			IModNetwork* GetModNetworkByName(const kxf::String& name) const override;
			
			void OnAuthStateChanged() override;
	};
}
