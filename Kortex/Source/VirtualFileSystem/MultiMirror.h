#pragma once
#include <Kortex/Kortex.hpp>
#include "Convergence.h"

namespace Kortex::VirtualFileSystem
{
	class MultiMirror: public Convergence
	{
		protected:
			MultiMirror(IPC::FileSystemID id, const kxf::String& mountPoint, const kxf::String& source);

		public:
			MultiMirror(const kxf::String& mountPoint, const kxf::String& source);
			MultiMirror(const kxf::String& mountPoint, const KxStringVector& sources);
	};
}
