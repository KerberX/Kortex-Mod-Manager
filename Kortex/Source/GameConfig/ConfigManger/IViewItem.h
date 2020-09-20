#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "kxf::UI::Framework/DataView2/DataView2Fwd.h"
#include <Kx/RTTI.hpp>

namespace Kortex::GameConfig
{
	class IViewItem: public KxRTTI::Interface<IViewItem>
	{
		KxRTTI_DeclareIID(IViewItem, {0x29f43e3e, 0x3375, 0x403a, {0x89, 0x5c, 0x2b, 0xf6, 0x21, 0xd3, 0x98, 0x3b}});

		public:
			virtual void OnAttachToView() {}
			virtual void OnSelect(KxDataView2::Column& column) {}
			virtual void OnActivate(KxDataView2::Column& column) {}

			virtual kxf::String GetViewString(ColumnID id) const = 0;
	};
}
