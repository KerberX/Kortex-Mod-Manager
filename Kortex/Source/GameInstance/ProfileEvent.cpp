#include "stdafx.h"
#include "ProfileEvent.h"
#include "IGameProfile.h"

namespace Kortex
{
	kxf::String ProfileEvent::GetProfileID() const
	{
		return m_Profile ? m_Profile->GetID() : GetString();
	}
}
