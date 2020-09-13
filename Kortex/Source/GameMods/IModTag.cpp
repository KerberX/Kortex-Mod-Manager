#include "stdafx.h"
#include "IModTag.h"
#include <Kortex/ModTagManager.hpp>
#include <Kortex/Application.hpp>

namespace Kortex
{
	std::optional<kxf::String> IModTag::GetTranslatedNameByID(const kxf::String& id)
	{
		return ITranslator::GetAppTranslator().TryGetString(wxS("TagManager.Tag.") + id);
	}

	bool IModTag::IsDefaultTag() const
	{
		IModTagManager* manager = IModTagManager::GetInstance();
		return manager && manager->FindTagByID(manager->GetDefaultTags(), GetID()) != nullptr;
	}
}
