#pragma once
#include "stdafx.h"
#include <KxVirtualFileSystem/Utility.h>

namespace Kortex::VirtualFileSystem
{
	inline kxf::String Tokxf::String(const KxVFS::KxDynamicStringW& value)
	{
		return kxf::String(value.data(), value.size());
	}
	inline kxf::String Tokxf::String(const KxVFS::KxDynamicStringRefW& value)
	{
		return kxf::String(value.data(), value.size());
	}
	
	inline KxVFS::KxDynamicStringW ToKxDynamicString(const kxf::String& value)
	{
		return KxVFS::KxDynamicStringW(value.wc_str(), value.length());
	}
	inline KxVFS::KxDynamicStringRefW ToKxDynamicStringRef(const kxf::String& value)
	{
		return KxVFS::KxDynamicStringRefW(value.wc_str(), value.length());
	}
}
