#include "stdafx.h"
#include "MultiMirror.h"
#include "IPC/Common.h"

using namespace Kortex::IPC;

namespace Kortex::VirtualFileSystem
{
	MultiMirror::MultiMirror(FileSystemID id, const kxf::String& mountPoint, const kxf::String& source)
		:Convergence(id, mountPoint, source)
	{
	}
	MultiMirror::MultiMirror(const kxf::String& mountPoint, const kxf::String& source)
		:Convergence(IPC::FileSystemID::MultiMirror, mountPoint, source)
	{
	}
	MultiMirror::MultiMirror(const kxf::String& mountPoint, const KxStringVector& sources)
		:MultiMirror(mountPoint, sources.front())
	{
		for (size_t i = 1; i < sources.size(); i++)
		{
			AddVirtualFolder(sources[i]);
		}
	}
}
