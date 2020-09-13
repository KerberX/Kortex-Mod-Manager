#include "stdafx.h"
#include "Mirror.h"
#include "VirtualFileSystem/IVFSService.h"
#include "VirtualFileSystem/FSControllerService.h"

namespace Kortex::VirtualFileSystem
{
	Mirror::Mirror(const kxf::String& mountPoint, const kxf::String& source)
		:KxVFSWrapper(ToKxDynamicStringRef(mountPoint), ToKxDynamicStringRef(source))
	{
	}
}
