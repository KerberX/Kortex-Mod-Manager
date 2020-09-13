#include "stdafx.h"
#include "PluginEvent.h"
#include "IGamePlugin.h"

namespace Kortex
{
	kxf::String PluginEvent::GetPluginName() const
	{
		return m_Plugin ? m_Plugin->GetName() : GetString();
	}
}
