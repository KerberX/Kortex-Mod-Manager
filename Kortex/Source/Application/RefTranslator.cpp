#include "stdafx.h"
#include "RefTranslator.h"

namespace Kortex
{
	std::optional<kxf::String> RefTranslator::DoGetString(const kxf::String& id) const
	{
		if (auto& item = m_TranslationRef->GetItem(id))
		{
			return item.GetString();
		}
		return {};
	}
	std::optional<kxf::String> RefTranslator::DoGetString(kxf::StdID id) const
	{
		if (auto& item = m_TranslationRef->GetItem(id))
		{
			return item.GetString();
		}
		return {};
	}
}
