#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "DataType.h"

namespace Kortex::GameConfig
{
	class IFileSource: public KxRTTI::Interface<IFileSource>
	{
		KxRTTI_DeclareIID(IFileSource, {0xf07744eb, 0x2d9f, 0x475f, {0x8b, 0xfb, 0xa0, 0xb7, 0xd5, 0xe0, 0xad, 0x9e}});

		protected:
			kxf::String ResolveFSLocation(const kxf::String& path) const;

		public:
			virtual kxf::String GetFileName() const = 0;
			virtual kxf::String GetFilePath() const = 0;

			kxf::String GetExpandedFileName() const;
			kxf::String GetResolvedFilePath() const;
	};
}
