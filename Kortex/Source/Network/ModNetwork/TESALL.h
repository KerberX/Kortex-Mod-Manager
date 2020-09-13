#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/Common.h"
#include "Network/IModNetwork.h"
#include <KxFramework/KxSingleton.h>
class KxCURLSession;

namespace Kortex::NetworkManager
{
	class TESALLModNetwork:
		public KxRTTI::ExtendInterface<TESALLModNetwork, IModNetwork>,
		public KxSingletonPtr<TESALLModNetwork>
	{
		KxRTTI_DeclareIID(TESALLModNetwork, {0xc4255aa9, 0x9087, 0x49e5, {0xb3, 0xf5, 0x36, 0x67, 0x21, 0x60, 0x6f, 0xbc}});

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
			TESALLModNetwork();

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
