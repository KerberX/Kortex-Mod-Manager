#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/Common.h"
#include "Network/ModNetworkRepository.h"
#include "NexusNetworkReply.h"
#include <kxf::UI::Framework/KxJSON.h>
class KxCURLEvent;

namespace Kortex::NetworkManager::NXMHandler
{
	class OptionStore;
}

namespace Kortex::NetworkManager
{
	class NexusModNetwork;
	class NexusUtility;
	class NexusAuth;

	class NexusRepository: public ModNetworkRepository
	{
		friend class NexusModNetwork;

		public:
			enum class ModActivity
			{
				Default = 0,
				Day,
				Week,
				Month
			};
			using GetModFiles2Result = std::pair<std::unordered_map<ModFileID, ModFileReply>, std::unordered_map<ModFileID, NexusModFileUpdateReply>>;

		private:
			NexusModNetwork& m_Nexus;
			NexusUtility& m_Utility;
			NexusAuth& m_Auth;

			mutable wxCriticalSection m_LimitsDataCS;
			ModRepositoryLimitsData m_LimitsData;

			std::unique_ptr<NXMHandler::OptionStore> m_NXMHandlerOptions;

		protected:
			kxf::String ConvertEndorsementState(const ModEndorsement& state) const;
			void OnResponseHeader(KxCURLEvent& event);

			AppOption GetNXMHandlerOptions() const;
			void LoadNXMHandlerOptions();

		public:
			NexusRepository(NexusModNetwork& nexus, NexusUtility& utility, NexusAuth& auth);
			~NexusRepository();

		public:
			ModRepositoryLimits GetRequestLimits() const override;
			bool IsAutomaticUpdateCheckAllowed() const;
			bool ParseDownloadName(const kxf::String& name, ModFileReply& result);

			bool QueryDownload(const KxFileItem& fileItem, const DownloadItem& download, ModFileReply& fileReply) override;
			void OnToolBarMenu(kxf::UI::Menu& menu);
			void OnDownloadMenu(kxf::UI::Menu& menu, DownloadItem* download = nullptr) override;

			bool QueueDownload(const kxf::String& link) override;
			wxAny GetDownloadTarget(const kxf::String& link) override;

			std::optional<ModInfoReply> GetModInfo(const ModRepositoryRequest& request) const override;
			std::optional<ModEndorsementReply> EndorseMod(const ModRepositoryRequest& request, ModEndorsement state) override;

			std::optional<ModFileReply> GetModFileInfo(const ModRepositoryRequest& request) const override;
			std::vector<ModFileReply> GetModFiles(const ModRepositoryRequest& request) const override;
			std::optional<GetModFiles2Result> GetModFiles2(const ModRepositoryRequest& request, bool files, bool updates) const;
			std::vector<ModDownloadReply> GetFileDownloads(const ModRepositoryRequest& request) const override;

		public:
			KxURI ConstructNXM(const NetworkModInfo& modInfo, const GameID& id = {}, const NexusNXMLinkData& linkData = {}) const;
			bool ParseNXM(const kxf::String& link, GameID& gameID, NetworkModInfo& modInfo, NexusNXMLinkData& linkData) const;
			bool ParseNXM(const KxURI& uri, GameID& gameID, NetworkModInfo& modInfo, NexusNXMLinkData& linkData) const
			{
				return ParseNXM(uri.BuildUnescapedURI(), gameID, modInfo, linkData);
			}
			
			void ConfigureNXMHandler();
	};
}
