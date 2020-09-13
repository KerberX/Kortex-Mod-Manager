#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/Common.h"
#include "Network/ModRepositoryReply.h"

namespace Kortex::NetworkManager
{
	struct NexusValidationReply
	{
		kxf::String UserName;
		kxf::String APIKey;
		kxf::String EMailAddress;
		kxf::String ProfilePicture;
		int64_t UserID = -1;
		bool IsPremium = false;
		bool IsSupporter = false;
	};
	struct NexusGameReply
	{
		int64_t ID = -1;
		kxf::String Name;
		kxf::String Genre;
		kxf::String ForumURL;
		kxf::String NexusURL;
		kxf::String DomainName;

		int64_t FilesCount = -1;
		int64_t DownloadsCount = -1;
		int64_t ModsCount = -1;
		wxDateTime ApprovedDate;
	};
	struct NexusModFileUpdateReply
	{
		ModFileID OldID;
		ModFileID NewID;

		kxf::String OldName;
		kxf::String NewName;

		wxDateTime UploadedDate;
	};
	struct NexusModActivityReply
	{
		ModID ModID;
		ModFileID LatestFileUpdate;
		int64_t LatestModActivity = 0;
	};
	struct NexusNXMLinkData
	{
		kxf::String Key;
		kxf::String Expires;
		kxf::String UserID;

		bool IsEmpty() const
		{
			return Key.IsEmpty() && Expires.IsEmpty() && UserID.IsEmpty();
		}
	};
}
