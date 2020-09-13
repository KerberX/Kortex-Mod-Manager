#pragma once
#include <Kortex/Kortex.hpp>
#include "INIRefSource.h"

namespace Kortex::GameConfig
{
	class INISource: public INIRefSource
	{
		private:
			KxINI m_INI;

		public:
			INISource(const kxf::String& iniText = {})
				:INIRefSource(m_INI), m_INI(iniText)
			{
			}
			INISource(wxInputStream& stream)
				:INIRefSource(m_INI), m_INI(stream)
			{
			}
	};
}
