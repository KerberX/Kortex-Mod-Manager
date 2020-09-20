#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "IModNetwork.h"
#include "ModRepositoryRequest.h"
#include "ModRepositoryReply.h"
#include "ModRepositoryLimits.h"
#include <kxf::UI::Framework/KxComponentSystem.h>
#include <kxf::UI::Framework/KxFileItem.h>
#include <optional>
class kxf::UI::Menu;

namespace Kortex
{
	class DownloadItem;
}

namespace Kortex
{
	class ModNetworkRepository: public KxComponentOf<IModNetwork>
	{
		public:
			virtual ModRepositoryLimits GetRequestLimits() const = 0;
			virtual void OnDownloadMenu(kxf::UI::Menu& menu, DownloadItem* download = nullptr) = 0;
			virtual bool QueryDownload(const KxFileItem& fileItem, const DownloadItem& download, ModFileReply& fileReply) = 0;
			
			virtual bool QueueDownload(const kxf::String& link) = 0;
			virtual wxAny GetDownloadTarget(const kxf::String& link) = 0;

			virtual std::optional<ModInfoReply> GetModInfo(const ModRepositoryRequest& request) const = 0;
			virtual std::optional<ModEndorsementReply> EndorseMod(const ModRepositoryRequest& request, ModEndorsement state) = 0;
			
			virtual std::optional<ModFileReply> GetModFileInfo(const ModRepositoryRequest& request) const = 0;
			virtual std::vector<ModFileReply> GetModFiles(const ModRepositoryRequest& request) const = 0;
			virtual std::vector<ModDownloadReply> GetFileDownloads(const ModRepositoryRequest& request) const = 0;
	};
}
