#pragma once
#include "stdafx.h"
#include "Common.h"
#include "Convergence.h"
#include <KxVirtualFileSystem/ConvergenceFS.h>

namespace Kortex::VirtualFileSystem
{
	class MultiMirror: public Convergence
	{
		public:
			MultiMirror(const kxf::String& mountPoint = {}, const kxf::String& source = {});
			MultiMirror(const kxf::String& mountPoint, const KxStringVector& sources);
	};
}
