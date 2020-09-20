#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/Common.h"
#include "Network/IModNetwork.h"
#include <kxf::UI::Framework/KxSingleton.h>
class KxCURLSession;

namespace Kortex::NetworkManager
{
	class LoversLabModNetwork:
		public KxRTTI::ExtendInterface<LoversLabModNetwork, IModNetwork>,
		public kxf::SingletonPtr<LoversLabModNetwork>
	{
		KxRTTI_DeclareIID(LoversLabModNetwork, {0xfd953e5d, 0x2c04, 0x4e82, {0x9f, 0x59, 0x6c, 0xc6, 0x91, 0xb6, 0xa3, 0x73}});

		private:
			kxf::String GetAPIURL() const;

		protected:
			void OnInit() override
			{
			}
			void OnExit() override
			{
			}
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& networkNode) override
			{
			}

		public:
			LoversLabModNetwork();

		public:
			kxf::ResourceID GetIcon() const override;
			kxf::String GetName() const override;

			kxf::String TranslateGameIDToNetwork(const GameID& id = {}) const override
			{
				return {};
			}
			GameID TranslateGameIDFromNetwork(const kxf::String& id) const override
			{
				return {};
			}

			KxURI GetModPageBaseURI(const GameID& id = {}) const override;
			KxURI GetModPageURI(const ModRepositoryRequest& request) const override;
	};
}
