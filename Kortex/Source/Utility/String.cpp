#include "stdafx.h"
#include "String.h"
#include <kxf::UI::Framework/KxUtility.h>

namespace Kortex::Utility::String
{
	std::optional<bool> ToBool(const kxf::String& value)
	{
		bool isUnknown = false;
		const bool result = KxUtility::StringToBool(value, &isUnknown);
		
		if (!isUnknown)
		{
			return result;
		}
		return std::nullopt;
	}
}
