#pragma once
#include <Kortex/Kortex.hpp>
#include "Mirror.h"

namespace Kortex::VirtualFileSystem
{
	class Convergence: public Mirror
	{
		protected:
			Convergence(IPC::FileSystemID id, const kxf::String& mountPoint, const kxf::String& writeTarget);

		public:
			Convergence(const kxf::String& mountPoint, const kxf::String& writeTarget);

		public:
			void AddVirtualFolder(const kxf::String& path);
			size_t BuildFileTree();
	};
}
