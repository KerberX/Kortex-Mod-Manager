#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "ModRepositoryReply.h"
#include "ModRepositoryRequest.h"
#include "GameInstance/GameID.h"
#include "Application/Resources/Imagekxf::ResourceID.h"
#include <KxFramework/KxComponentSystem.h>
#include <KxFramework/KxSecretStore.h>
#include <KxFramework/kxf::Version.h>
#include <KxFramework/KxURI.h>
#include <Kx/RTTI.hpp>
class KxMenu;
class kxf::XMLNode;

namespace Kortex
{
	class DownloadItem;
	class IGameInstance;
	class IGameMod;
}

namespace Kortex
{
	class IModNetwork: public KxRTTI::Interface<IModNetwork>, public KxComponentContainer
	{
		KxRTTI_DeclareIID(IModNetwork, {0xc58037c8, 0x9e52, 0x45df, {0xac, 0xd5, 0xa5, 0xb7, 0x4e, 0x3f, 0x88, 0x5f}});

		friend class INetworkManager;
		friend class NetworkModule;

		public:
			using Vector = std::vector<std::unique_ptr<IModNetwork>>;
			using RefVector = std::vector<IModNetwork*>;
			using ModsRefVector = std::vector<IGameMod*>;

		public:
			static kxf::ResourceID GetGenericIcon();

		private:
			void DoOnInit();
			void DoOnExit();

			virtual void OnInit() = 0;
			virtual void OnExit() = 0;

		protected:
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& networkNode) = 0;
			KxURI GetIPBModPageURI(ModID modID, const kxf::String& modSignature = {}) const;

		public:
			bool IsDefault() const;
			kxf::String GetCacheDirectory() const;
			kxf::String GetLocationInCache(const kxf::String& relativePath) const;
			
			virtual kxf::ResourceID GetIcon() const = 0;
			virtual kxf::String GetName() const = 0;
			
			virtual kxf::String TranslateGameIDToNetwork(const GameID& id = {}) const = 0;
			virtual GameID TranslateGameIDFromNetwork(const kxf::String& id) const = 0;
			virtual void ConvertDescriptionText(kxf::String& description) const
			{
			}
			
			virtual KxURI GetModPageBaseURI(const GameID& id = {}) const = 0;
			virtual KxURI GetModPageURI(const ModRepositoryRequest& request) const = 0;
			KxURI GetModPageURI(const IGameMod& mod) const;
			KxURI GetModPageURI(const DownloadItem& download) const;

		public:
			virtual void OnToolBarMenu(KxMenu& menu)
			{
			}
			virtual void OnModListMenu(KxMenu& menu, const ModsRefVector& selectedMods, IGameMod* focusedMod)
			{
			}
	};
}
