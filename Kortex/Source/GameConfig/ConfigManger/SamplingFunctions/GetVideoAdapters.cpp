#include "stdafx.h"
#include "GetVideoAdapters.h"
#include <KxFramework/KxSystemSettings.h>

namespace Kortex::GameConfig::SamplingFunction
{
	void GetVideoAdapters::DoCall()
	{
		for (const DISPLAY_DEVICE& adapter: KxSystemSettings::EnumVideoAdapters())
		{
			// DeviceString field is wchar_t[] array so wrap it into kxf::String,
			// otherwise wxAny will store pointer to the array and not copy its content as kxf::String.
			m_Values.emplace_back(kxf::String(adapter.DeviceString));
		}
	}
	void GetVideoAdapters::OnCall(const ItemValue::Vector& arguments)
	{
		DoCall();
	}
}
