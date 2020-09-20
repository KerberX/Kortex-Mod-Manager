#pragma once
#include "Utility/UniqueID.h"
#include <kxf::UI::Framework/KxURI.h>

namespace Kortex::NetworkManager::Internal
{
	namespace Tags
	{
		struct ModID
		{
		};
		struct ModFileID
		{
		};
	}
}

namespace Kortex
{
	using ModID = Utility::UniqueID::IntegerID<int64_t, -1, false, NetworkManager::Internal::Tags::ModID>;
	using ModFileID = Utility::UniqueID::IntegerID<int64_t, -1, false, NetworkManager::Internal::Tags::ModFileID>;
}
