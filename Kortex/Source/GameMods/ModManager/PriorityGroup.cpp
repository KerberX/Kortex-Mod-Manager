#include "stdafx.h"
#include "PriorityGroup.h"
#include <Kortex/ModTagManager.hpp>

namespace Kortex::ModManager
{
	kxf::String PriorityGroup::GetName() const
	{
		return m_Tag ? m_Tag->GetName() : wxEmptyString;
	}
	kxf::String PriorityGroup::GetID() const
	{
		return m_Tag ? m_Tag->GetID() : wxEmptyString;
	}

	bool PriorityGroup::HasColor() const
	{
		return m_Tag && m_Tag->HasColor();
	}
	KxColor PriorityGroup::GetColor() const
	{
		return m_Tag ? m_Tag->GetColor() : KxColor();
	}
	void PriorityGroup::SetColor(const KxColor& color)
	{
		if (m_Tag)
		{
			m_Tag->SetColor(color);
		}
	}
}
