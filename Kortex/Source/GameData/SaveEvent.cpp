#include "stdafx.h"
#include "SaveEvent.h"
#include "IGameSave.h"

namespace Kortex
{
	kxf::String SaveEvent::GetSaveName() const
	{
		return m_Save ? m_Save->GetFileItem().GetName() : GetString();
	}
}
