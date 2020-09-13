#include "stdafx.h"
#include "ModEvent.h"
#include "IGameMod.h"

namespace Kortex
{
	kxf::String ModEvent::GetModID() const
	{
		return m_Mod ? m_Mod->GetID() : wxNotifyEvent::GetString();
	}
}
