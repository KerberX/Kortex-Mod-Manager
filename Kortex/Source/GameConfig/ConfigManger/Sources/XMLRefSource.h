#pragma once
#include <Kortex/Kortex.hpp>
#include "GameConfig/ConfigManger/ISource.h"
#include <kxf::UI::Framework/KxXML.h>

namespace Kortex::GameConfig
{
	class XMLRefSource: public ISource
	{
		private:
			kxf::XMLDocument& m_XML;
			bool m_IsOpened = false;

		protected:
			const kxf::XMLDocument& GetXML() const
			{
				return m_XML;
			}
			kxf::XMLDocument& GetXML()
			{
				return m_XML;
			}

		public:
			XMLRefSource(kxf::XMLDocument& xml)
				:m_XML(xml)
			{
			}

		public:
			// ISource
			SourceFormatValue GetFormat() const override
			{
				return SourceFormat::XML;
			}
			kxf::String GetPathDescription() const override
			{
				return {};
			}

			bool IsOpened() const override
			{
				return m_IsOpened;
			}
			bool Open() override;
			bool Save() override;
			void Close() override;

			bool WriteValue(const Item& item, const ItemValue& value) override;
			bool ReadValue(Item& item, ItemValue& value) const override;
			void LoadUnknownItems(ItemGroup& group) override;
	};
}
