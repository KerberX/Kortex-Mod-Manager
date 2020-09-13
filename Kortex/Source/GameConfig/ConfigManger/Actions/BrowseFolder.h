#pragma once
#include <Kortex/Kortex.hpp>
#include "GameConfig/ConfigManger/IAction.h"

namespace Kortex::GameConfig::Actions
{
	class BrowseFolder: public IAction
	{
		public:
			virtual void Invoke(Item& item, ItemValue& value) override;
	};
}
