#pragma once
#include "stdafx.h"
#include "Common.h"
#include "KxVFSWrapper.h"
#include <KxVirtualFileSystem/IFileSystem.h>
#include <KxVirtualFileSystem/ConvergenceFS.h>

namespace Kortex::VirtualFileSystem
{
	class Convergence: public KxVFSWrapper<KxVFS::ConvergenceFS>
	{
		public:
			Convergence(const kxf::String& mountPoint = {}, const kxf::String& writeTarget = {});

		public:
			kxf::String GetWriteTarget() const
			{
				return Tokxf::String(ConvergenceFS::GetWriteTarget());
			}
			void SetWriteTarget(const kxf::String& writeTarget)
			{
				return ConvergenceFS::SetWriteTarget(ToKxDynamicStringRef(writeTarget));
			}

			void AddVirtualFolder(const kxf::String& path)
			{
				ConvergenceFS::AddVirtualFolder(ToKxDynamicStringRef(path));
			}
	};
}
