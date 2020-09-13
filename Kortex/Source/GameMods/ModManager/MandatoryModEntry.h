#pragma once
#include <Kortex/Kortex.hpp>
#include "FixedGameMod.h"

namespace Kortex::ModManager
{
	class KMandatoryModEntry: public FixedGameMod
	{
		public:
			KMandatoryModEntry(intptr_t priority = -1)
				:FixedGameMod(priority)
			{
			}

		public:
			virtual bool IsLinkedMod() const override
			{
				return true;
			}
			virtual kxf::String GetModFilesDir() const override
			{
				return KVarExp(FixedGameMod::GetModFilesDir());
			}
	};
}
