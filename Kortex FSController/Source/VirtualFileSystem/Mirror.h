#pragma once
#include "stdafx.h"
#include "Common.h"
#include "KxVFSWrapper.h"
#include <KxVirtualFileSystem/MirrorFS.h>

namespace Kortex::VirtualFileSystem
{
	class Mirror: public KxVFSWrapper<KxVFS::MirrorFS>
	{
		public:
			Mirror(const kxf::String& mountPoint = {}, const kxf::String& source = {});

		public:
			kxf::String GetSource() const
			{
				return Tokxf::String(MirrorFS::GetSource());
			}
			void SetSource(const kxf::String& source)
			{
				MirrorFS::SetSource(ToKxDynamicStringRef(source));
			}
	};
}