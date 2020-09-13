#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/Common.h"
#include "Network/IModNetwork.h"
#include "NexusNetworkReply.h"
#include <KxFramework/KxJSON.h>
#include <KxFramework/KxHTTPStatusCode.h>
class KxCURLReplyBase;

namespace Kortex::NetworkManager
{
	class NexusModNetwork;

	class NexusUtility: public KxComponentOf<NexusModNetwork>
	{
		public:
			using TJsonValue = typename nlohmann::json::value_type;

		private:
			NexusModNetwork& m_Nexus;

		private:
			KxHTTPStatusValue DoTestRequestError(const KxCURLReplyBase& reply, const kxf::String& message, bool noErrorReport) const;

		public:
			NexusUtility(NexusModNetwork& nexus)
				:m_Nexus(nexus)
			{
			}

		public:
			void ConvertChangeLog(kxf::String& changeLog) const;
			void ConvertDisplayName(kxf::String& name) const;
			void ConvertUnicodeEscapes(kxf::String& source) const;

			wxDateTime ReadDateTime(const TJsonValue& json) const;
			void ReadFileInfo(const TJsonValue& json, ModFileReply& info) const;
			void ReadGameInfo(const TJsonValue& json, NexusGameReply& info) const;

			void ReportRequestError(const kxf::String& message) const;
			void ReportRequestQuoteReached() const;
			KxHTTPStatusValue TestRequestError(const KxCURLReplyBase& reply, const kxf::String& message = {}) const;
			KxHTTPStatusValue TestRequestErrorSilent(const KxCURLReplyBase& reply) const;
	};
}
