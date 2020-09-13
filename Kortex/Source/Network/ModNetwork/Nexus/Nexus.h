#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/Common.h"
#include "Network/IModNetwork.h"
#include "NexusUpdateChecker.h"
#include "NexusRepository.h"
#include "NexusUtility.h"
#include "NexusAuth.h"
#include "NexusNetworkReply.h"
#include <KxFramework/KxSingleton.h>
#include <KxFramework/KxUUID.h>
class KxCURLEvent;
class KxCURLSession;
class KxCURLReplyBase;

namespace Kortex::NetworkManager
{
	class NexusModNetwork:
		public KxRTTI::ExtendInterface<NexusModNetwork, IModNetwork>,
		public KxSingletonPtr<NexusModNetwork>
	{
		KxRTTI_DeclareIID(NexusModNetwork, {0x8da1cedb, 0x3c16, 0x4ca8, {0x8b, 0x4f, 0xf6, 0x4b, 0x19, 0xf, 0xcb, 0xe3}});

		friend class NexusAuth;
		friend class NexusRepository;
		friend class NexusUpdateChecker;

		private:
			NexusUtility m_Utility;
			NexusRepository m_Repository;
			NexusAuth m_Auth;
			NexusUpdateChecker m_UpdateChecker;

		private:
			void OnAuthenticated();

		protected:
			kxf::String GetAPIURL() const;
			kxf::String GetAPIKey() const;
			std::unique_ptr<KxCURLSession> NewCURLSession(const kxf::String& address, const kxf::String& apiKey = {}) const;

			void OnInit() override;
			void OnExit() override;
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& networkNode) override;

		public:
			NexusModNetwork();

		public:
			kxf::ResourceID GetIcon() const override;
			kxf::String GetName() const override;

			kxf::String TranslateGameIDToNetwork(const GameID& id = {}) const override;
			kxf::String TranslateGameIDToNetwork(const ModRepositoryRequest& request) const
			{
				return TranslateGameIDToNetwork(request.GetGameID());
			}
			GameID TranslateGameIDFromNetwork(const kxf::String& id) const override;
			void ConvertDescriptionText(kxf::String& description) const override;

			KxURI GetModPageBaseURI(const GameID& id = {}) const override;
			KxURI GetModPageURI(const ModRepositoryRequest& request) const override;

		public:
			std::optional<NexusGameReply> GetGameInfo(const GameID& id = {}) const;
			std::vector<NexusGameReply> GetGamesList() const;
			
		public:
			void OnToolBarMenu(KxMenu& menu) override;
	};
}
