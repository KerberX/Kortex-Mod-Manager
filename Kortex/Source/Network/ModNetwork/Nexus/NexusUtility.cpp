#include "stdafx.h"
#include "NexusUtility.h"
#include "Nexus.h"
#include "Network/INetworkManager.h"
#include "Application/INotificationCenter.h"
#include <KxFramework/KxIndexedEnum.h>
#include <KxFramework/KxCURL.h>

namespace
{
	struct CategoryDef: public KxIndexedEnum::Definition<CategoryDef, Kortex::ModFileCategory, kxf::String>
	{
		using ModFileCategory = Kortex::ModFileCategory;

		inline static const TItem ms_Index[] =
		{
			{ModFileCategory::Main, wxS("MAIN")},
			{ModFileCategory::Misc, wxS("MISCELLANEOUS")},
			{ModFileCategory::Optional, wxS("OPTIONAL")},
			{ModFileCategory::OldVersion, wxS("OLD_VERSION")},
		};
	};
}

namespace Kortex::NetworkManager
{
	KxHTTPStatusValue NexusUtility::DoTestRequestError(const KxCURLReplyBase& reply, const kxf::String& message, bool noErrorReport) const
	{
		if (reply.GetResponseCode() == KxHTTPStatusCode::TooManyRequests)
		{
			if (!noErrorReport)
			{
				ReportRequestQuoteReached();
			}
		}
		else if (!reply.IsOK())
		{
			if (!noErrorReport)
			{
				// If we get code 401 "Unauthorized", then API key has changed while program is running.
				// Notify auth component and network manager about that but don't show regular error message.
				if (reply.GetResponseCode() == KxHTTPStatusCode::Unauthorized)
				{
					const_cast<NexusUtility*>(this)->GetComponent<ModNetworkAuth>().OnAuthReset();
				}
				else
				{
					ReportRequestError(message.IsEmpty() ? reply.GetErrorMessage() : message);
				}
			}
		}
		return reply.GetResponseCode();
	}

	void NexusUtility::ConvertChangeLog(kxf::String& changeLog) const
	{
		changeLog.Replace(wxS("<br>"), wxS("\r\n"));
		changeLog.Replace(wxS("<br/>"), wxS("\r\n"));
		changeLog.Replace(wxS("<br />"), wxS("\r\n"));
		changeLog.Replace(wxS("</br>"), wxS("\r\n"));

		changeLog.Replace(wxS("\n\r\n"), wxS("\r\n"));
		KxString::Trim(changeLog, true, true);
	}
	void NexusUtility::ConvertDisplayName(kxf::String& name) const
	{
		name.Replace(wxS("_"), wxS(" "));
		KxString::Trim(name, true, true);
	}
	void NexusUtility::ConvertUnicodeEscapes(kxf::String& source) const
	{
		// Find and replace all '\uABCD' 6-char hex patterns to corresponding Unicode codes.
		// This is almost the same as 'ModImporterMO::DecodeUTF8'. Need to generalize and merge these functions.
		constexpr size_t prefixLength = 2;
		constexpr size_t sequenceLength = 6;
		constexpr size_t valueLength = sequenceLength - prefixLength;

		for (size_t i = 0; i < source.Length(); i++)
		{
			size_t pos = source.find(wxS("\\u"), i);
			if (pos != kxf::String::npos)
			{
				unsigned long long value = 0;
				if (source.Mid(pos + prefixLength, valueLength).ToULongLong(&value, 16) && value != 0)
				{
					wxUniChar c(value);
					source.replace(pos, sequenceLength, c);
				}
			}
		}
	}

	wxDateTime NexusUtility::ReadDateTime(const TJsonValue& json) const
	{
		wxDateTime date;
		date.ParseISOCombined(json.get<kxf::String>());
		return date.FromUTC(date.IsDST());
	}
	void NexusUtility::ReadFileInfo(const TJsonValue& json, ModFileReply& info) const
	{
		info.ID = json["file_id"].get<ModID::TValue>();
		info.IsPrimary = json["is_primary"];
		info.Name = json["file_name"].get<kxf::String>();
		info.DisplayName = json["name"].get<kxf::String>();
		info.Version = json["version"].get<kxf::String>();
		info.ChangeLog = json["changelog_html"].get<kxf::String>();
		info.UploadDate = ReadDateTime(json["uploaded_time"]);

		ConvertDisplayName(info.DisplayName);
		ConvertChangeLog(info.ChangeLog);

		// WTF?! Why file size is in kilobytes instead of bytes?
		// Ok, I convert it here, though final size may be a bit smaller.
		// At least download manager can request correct file size upon downloading.
		info.Size = json["size"].get<int64_t>() * 1024;

		// Values: 'MAIN', 'OPTIONAL', <TBD>.
		info.Category = CategoryDef::FromString(json["category_name"].get<kxf::String>(), ModFileCategory::Unknown);
	}
	void NexusUtility::ReadGameInfo(const TJsonValue& json, NexusGameReply& info) const
	{
		info.ID = json["id"];
		info.Name = json["name"].get<kxf::String>();
		info.Genre = json["genre"].get<kxf::String>();
		info.ForumURL = json["forum_url"].get<kxf::String>();
		info.NexusURL = json["nexusmods_url"].get<kxf::String>();
		info.DomainName = json["domain_name"].get<kxf::String>();

		info.FilesCount = json["file_count"];
		info.DownloadsCount = json["downloads"];
		info.ModsCount = json["mods"];
		info.ApprovedDate = wxDateTime((time_t)json["approved_date"]);
	}

	void NexusUtility::ReportRequestError(const kxf::String& message) const
	{
		try
		{
			KxJSONObject json = KxJSON::Load(message);
			INotificationCenter::NotifyUsing<INetworkManager>(json["message"].get<kxf::String>(), KxICON_ERROR);
		}
		catch (...)
		{
			INotificationCenter::NotifyUsing<INetworkManager>(message, KxICON_ERROR);
		}
	}
	void NexusUtility::ReportRequestQuoteReached() const
	{
		INotificationCenter::NotifyUsing<INetworkManager>(KTrf("NetworkManager.RequestQuotaReched",
														  m_Nexus.GetName()),
														  KxICON_WARNING
														  );
	}
	KxHTTPStatusValue NexusUtility::TestRequestError(const KxCURLReplyBase& reply, const kxf::String& message) const
	{
		return DoTestRequestError(reply, message, false);
	}
	KxHTTPStatusValue NexusUtility::TestRequestErrorSilent(const KxCURLReplyBase& reply) const
	{
		return DoTestRequestError(reply, {}, true);
	}
}
