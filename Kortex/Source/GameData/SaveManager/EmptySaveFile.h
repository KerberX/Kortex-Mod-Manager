#pragma once
#include <Kortex/Kortex.hpp>
#include "BaseGameSave.h"

namespace Kortex::SaveManager
{
	class EmptySaveFile: public BaseGameSave
	{
		private:
			InfoPairVector m_Info;

		protected:
			bool OnRead(const KxFileItem& fileItem) override
			{
				return fileItem.IsOK();
			}

		public:
			wxBitmap GetBitmap() const override
			{
				return wxNullBitmap;
			}
			const InfoPairVector& GetBasicInfo() const override
			{
				return m_Info;
			}
			kxf::String GetDisplayName() const override
			{
				return {};
			}
	};
}
