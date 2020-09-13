#include "stdafx.h"
#include "Convergence.h"
#include "VirtualFileSystem/IVFSService.h"
#include "VirtualFileSystem/FSControllerService.h"

namespace Kortex::VirtualFileSystem
{
	Convergence::Convergence(const kxf::String& mountPoint, const kxf::String& writeTarget)
		:KxVFSWrapper(ToKxDynamicStringRef(mountPoint), ToKxDynamicStringRef(writeTarget))
	{
	}
}
