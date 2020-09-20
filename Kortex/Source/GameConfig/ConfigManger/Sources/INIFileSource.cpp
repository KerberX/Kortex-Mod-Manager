#include "stdafx.h"
#include "INIFileSource.h"
#include <kxf::UI::Framework/KxFileStream.h>

namespace Kortex::GameConfig
{
	bool INIFileSource::Open()
	{
		if (!INISource::IsOpened())
		{
			KxFileStream stream(GetResolvedFilePath(), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
			if (stream.IsOk() && GetINI().Load(stream))
			{
				INISource::Open();
				return true;
			}
		}
		return false;
	}
	bool INIFileSource::Save()
	{
		KxFileStream stream(GetResolvedFilePath(), KxFileStream::Access::Write, KxFileStream::Disposition::CreateAlways, KxFileStream::Share::Read);
		if (stream.IsOk())
		{
			return GetINI().Save(stream);
		}
		return false;
	}
	void INIFileSource::Close()
	{
		INISource::Close();
	}
}
