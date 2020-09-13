#include "stdafx.h"
#include "Mirror.h"
#include "IPC/Common.h"

using namespace Kortex::IPC;

namespace Kortex::VirtualFileSystem
{
	Mirror::Mirror(FileSystemID id, const kxf::String& mountPoint, const kxf::String& source)
		:BaseFileSystem(id)
	{
		m_Controller.Send(RequestID::FSSetMountPoint, m_Handle, mountPoint);
		m_Controller.Send(RequestID::FSSetSource, m_Handle, source);
	}
	Mirror::Mirror(const kxf::String& mountPoint, const kxf::String& source)
		:Mirror(FileSystemID::Mirror, mountPoint, source)
	{
	}

	void Mirror::SetSource(const kxf::String& path)
	{
		m_Controller.Send(RequestID::FSSetSource, m_Handle, path);
	}
}
