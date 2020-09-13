#include "stdafx.h"
#include "Convergence.h"
#include "IPC/Common.h"

using namespace Kortex::IPC;

namespace Kortex::VirtualFileSystem
{
	Convergence::Convergence(FileSystemID id, const kxf::String& mountPoint, const kxf::String& writeTarget)
		:Mirror(id, mountPoint, writeTarget)
	{
	}
	Convergence::Convergence(const kxf::String& mountPoint, const kxf::String& writeTarget)
		:Convergence(FileSystemID::Convergence, mountPoint, writeTarget)
	{
	}

	void Convergence::AddVirtualFolder(const kxf::String& path)
	{
		m_Controller.Send(RequestID::FSAddVirtualFolder, m_Handle, path);
	}
	size_t Convergence::BuildFileTree()
	{
		return m_Controller.Send(RequestID::FSBuildFileTree, m_Handle).GetAs<size_t>();
	}
}
