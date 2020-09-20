#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxArchive.h>
#include <kxf::UI::Framework/KxArchiveEvent.h>
#include <kxf::UI::Framework/KxFileItem.h>

namespace Kortex::Archive
{
	enum class Method
	{
		Unknown = -1,

		LZMA,
		LZMA2,
		PPMd,
		BZip2,
	};
	enum class Format
	{
		Unknown = -1,
		SevenZip,
		Zip,
		RAR,
		RAR5,
		GZip,
		BZip2,
		Tar,
		ISO,
		CAB,
		LZMA,
		LZMA86,
	};
}

namespace Kortex::Archive
{
	Format DetectFormat(const kxf::String& filePath);

	kxf::String GetExtensionFromFormat(Format format);
	kxf::String GetFormatName(Format format);
}

namespace Kortex::Archive::Property
{
	//KxArchiveDeclareUserProperty(Compression, Solid);
}
