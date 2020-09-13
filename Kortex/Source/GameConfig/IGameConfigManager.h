#pragma once
#include <Kortex/Kortex.hpp>
#include "IConfigManager.h"
#include <KxFramework/KxSingleton.h>

namespace Kortex
{
	class IGameConfigManager: public IConfigManager, public KxSingletonPtr<IGameConfigManager>
	{
		public:
			IGameConfigManager();
	};
}
