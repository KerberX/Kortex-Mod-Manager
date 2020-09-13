#pragma once
#include <Kortex/Kortex.hpp>
#include "BaseFileSystem.h"

namespace Kortex::VirtualFileSystem
{
	class Mirror: public BaseFileSystem
	{
		protected:
			Mirror(IPC::FileSystemID id, const kxf::String& mountPoint, const kxf::String& source);

		public:
			Mirror(const kxf::String& mountPoint, const kxf::String& source);

		public:
			void SetSource(const kxf::String& path);
	};
}
