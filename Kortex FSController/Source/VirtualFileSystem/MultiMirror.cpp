#include "stdafx.h"
#include "MultiMirror.h"
#include "VirtualFileSystem/IVFSService.h"
#include "VirtualFileSystem/FSControllerService.h"

namespace Kortex::VirtualFileSystem
{
	MultiMirror::MultiMirror(const kxf::String& mountPoint, const kxf::String& source)
		:Convergence(mountPoint, source)
	{
	}
	MultiMirror::MultiMirror(const kxf::String& mountPoint, const KxStringVector& sources)
		:Convergence(mountPoint, sources.front())
	{
		for (size_t i = 1; i < sources.size(); i++)
		{
			AddVirtualFolder(sources[i]);
		}
	}
}
