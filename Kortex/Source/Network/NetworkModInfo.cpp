#include "stdafx.h"
#include "NetworkModInfo.h"
#include "ModRepositoryReply.h"

namespace Kortex
{
	wxTextValidator NetworkModInfo::CreateValidator()
	{
		wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
		validator.SetCharIncludes(wxS(":0123456789"));
		return validator;
	}

	NetworkModInfo::NetworkModInfo(const ModFileReply& fileInfo)
		:m_ModID(fileInfo.ModID), m_FileID(fileInfo.ID)
	{
	}

	bool NetworkModInfo::FromString(const kxf::String& stringValue)
	{
		kxf::String fileID;
		kxf::String modID = stringValue.BeforeFirst(wxS(':'), &fileID);

		m_FileID.FromString(fileID);
		return m_ModID.FromString(modID);
	}
}
