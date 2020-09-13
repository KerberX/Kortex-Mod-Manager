#pragma once
#include <Kortex/Kortex.hpp>
#include "XMLRefSource.h"

namespace Kortex::GameConfig
{
	class XMLSource: public XMLRefSource
	{
		private:
			kxf::XMLDocument m_XML;

		public:
			XMLSource(const kxf::String& xmlText = {})
				:XMLRefSource(m_XML), m_XML(xmlText)
			{
			}
			XMLSource(wxInputStream& stream)
				:XMLRefSource(m_XML), m_XML(stream)
			{
			}
	};
}
