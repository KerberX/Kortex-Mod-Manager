#pragma once
#include <Kortex/Kortex.hpp>
#include "IConfigManager.h"
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex
{
	class IGameConfigManager: public IConfigManager, public kxf::SingletonPtr<IGameConfigManager>
	{
		public:
			IGameConfigManager();
	};
}
