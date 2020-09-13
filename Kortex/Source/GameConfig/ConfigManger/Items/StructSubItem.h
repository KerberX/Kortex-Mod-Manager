#pragma once
#include <Kortex/Kortex.hpp>
#include "SimpleItem.h"

namespace Kortex::GameConfig
{
	class StructItem;

	class StructSubItem: public KxRTTI::ExtendInterface<StructSubItem, SimpleItem>
	{
		KxRTTI_DeclareIID(StructSubItem, {0xc1a2e07e, 0x1463, 0x4ec4, {0x94, 0x87, 0x99, 0xe4, 0x32, 0x5d, 0xca, 0x3b}});

		friend class StructItem;

		private:
			StructItem& m_Struct;

		public:
			StructSubItem(StructItem& structItem, const kxf::XMLNode& itemNode = {});

		public:
			bool IsOK() const override;
			kxf::String GetViewString(ColumnID id) const override;
			kxf::String GetPath() const override;

			const StructItem& GetStruct() const
			{
				return m_Struct;
			}
			StructItem& GetStruct()
			{
				return m_Struct;
			}

		public:
			bool SetValue(KxDataView2::Column& column, const wxAny& value) override;
	};
}
