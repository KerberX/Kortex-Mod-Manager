#pragma once
#include <Kortex/Kortex.hpp>
#include "BethesdaBasicSave.h"

namespace Kortex::SaveManager::BethesdaSave
{
	class Fallout3: public BethesdaBasicSave
	{
		protected:
			virtual bool OnRead(const KxFileItem& fileItem) override;
	};
}
