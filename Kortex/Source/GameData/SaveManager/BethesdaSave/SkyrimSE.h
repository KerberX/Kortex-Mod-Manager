#pragma once
#include <Kortex/Kortex.hpp>
#include "BethesdaBasicSave.h"

namespace Kortex::SaveManager::BethesdaSave
{
	class SkyrimSE: public BethesdaBasicSave
	{
		protected:
			virtual bool OnRead(const KxFileItem& fileItem) override;
	};
}
