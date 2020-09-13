#include "stdafx.h"
#include "DefaultTag.h"
#include "DefaultTagManager.h"

namespace Kortex::ModTagManager
{
	kxf::String DefaultTag::GetName() const
	{
		if (m_Name.IsEmpty())
		{
			auto name = GetTranslatedNameByID(m_ID);
			return name ? *name : m_ID;
		}
		return m_Name;
	}
	void DefaultTag::SetName(const kxf::String& name)
	{
		m_Name = name;
	}
}
