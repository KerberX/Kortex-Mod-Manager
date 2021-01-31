#pragma once
#include "Framework.hpp"
#include <kxf/Localization/ILocalizationPackage.h>

namespace Kortex
{
	kxf::String Localize(const kxf::ResourceID& id);
	kxf::LocalizationItem LocalizeItem(const kxf::ResourceID& id);

	template<class... Args>
	kxf::String Localize(const kxf::ResourceID& id, Args&&... arg)
	{
		return kxf::String::Format(Localize(id), std::forward<Args>(arg)...);
	}

	template<class... Args>
	kxf::LocalizationItem LocalizeItem(const kxf::ResourceID& id, Args&&... arg)
	{
		return kxf::String::Format(LocalizeItem(id), std::forward<Args>(arg)...);
	}
}
