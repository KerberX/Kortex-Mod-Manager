#include "stdafx.h"
#include "IModTagManager.h"
#include "GameMods/GameModsModule.h"
#include <kxf::UI::Framework/KxUtility.h>

namespace
{
	enum class FindBy
	{
		Object,
		ID,
		Name
	};

	template<class TagT, FindBy findBy, class TVector, class TValue, class TIterator = void>
	TagT* FindModTag(TVector&& items, TValue&& value, TIterator* iterator = nullptr)
	{
		auto it = std::find_if(items.begin(), items.end(), [&value](const auto& tag)
		{
			if constexpr(findBy == FindBy::Object)
			{
				return tag.get() == &value;
			}
			else if constexpr(findBy == FindBy::ID)
			{
				return tag->GetID() == value;
			}
			else if constexpr(findBy == FindBy::Name)
			{
				return tag->GetName() == value;
			}
			return false;
		});

		if (it != items.end())
		{
			if constexpr(!std::is_void_v<TIterator>)
			{
				KxUtility::SetIfNotNull(iterator, it);
			}
			return (*it).get();
		}
		else
		{
			if constexpr (!std::is_void_v<TIterator>)
			{
				KxUtility::SetIfNotNull(iterator, items.end());
			}
			return nullptr;
		}
	}
}

namespace Kortex
{
	namespace ModTagManager::Internal
	{
		const SimpleManagerInfo TypeInfo("TagManager", "TagManager.Name");
	}

	IModTagManager::IModTagManager()
		:ManagerWithTypeInfo(GameModsModule::GetInstance())
	{
	}

	void IModTagManager::LoadDefaultTags(IModTag::Vector& items)
	{
		for (const auto& tag: GetDefaultTags())
		{
			IModTag::Vector::iterator it;
			if (FindModTag<IModTag, FindBy::ID>(items, tag->GetID(), &it))
			{
				(*it) = tag->Clone();
			}
			else
			{
				items.emplace_back(tag->Clone());
			}
		}
	}
	void IModTagManager::LoadDefaultTags()
	{
		LoadDefaultTags(GetTags());
	}

	IModTag& IModTagManager::EmplaceTag()
	{
		return EmplaceTag(NewTag());
	}
	IModTag& IModTagManager::EmplaceTag(IModTag::Vector& items)
	{
		return EmplaceTag(items, NewTag());
	}
	
	IModTag& IModTagManager::EmplaceTag(std::unique_ptr<IModTag> tag)
	{
		return EmplaceTag(GetTags(), std::move(tag));
	}
	IModTag& IModTagManager::EmplaceTag(IModTag::Vector& items, std::unique_ptr<IModTag> tag)
	{
		if (IModTag* existingTag = FindModTag<IModTag, FindBy::ID>(items, tag->GetID()))
		{
			return *existingTag;
		}
		return *items.emplace_back(std::move(tag));
	}

	IModTag& IModTagManager::EmplaceTagWith(const kxf::String& id, const kxf::String& name)
	{
		return EmplaceTagWith(GetTags(), id, name);
	}
	IModTag& IModTagManager::EmplaceTagWith(IModTag::Vector& items, const kxf::String& id, const kxf::String& name)
	{
		if (IModTag* existingTag = FindTagByID(items, id))
		{
			return *existingTag;
		}
		else
		{
			IModTag& tag = EmplaceTag(items);
			tag.SetID(id);
			tag.SetName(name.IsEmpty() ? id : name);

			return tag;
		}
	}

	void IModTagManager::RemoveAllTags()
	{
		GetTags().clear();
	}
	void IModTagManager::RemoveAllTags(IModTag::Vector& items)
	{
		items.clear();
	}

	bool IModTagManager::RemoveTag(IModTag& tag)
	{
		return RemoveTag(GetTags(), tag);
	}
	bool IModTagManager::RemoveTag(IModTag::Vector& items, IModTag& tag)
	{
		IModTag::Vector::const_iterator it;
		if (FindModTag<IModTag, FindBy::Object>(items, tag, &it))
		{
			items.erase(it);
			return true;
		}
		return false;
	}

	bool IModTagManager::RemoveTagByID(const kxf::String& id)
	{
		RemoveTagByID(GetTags(), id);
	}
	bool IModTagManager::RemoveTagByID(IModTag::Vector& items, const kxf::String& id)
	{
		IModTag::Vector::const_iterator it;
		if (FindModTag<IModTag, FindBy::ID>(items, id, &it))
		{
			items.erase(it);
			return true;
		}
		return false;
	}

	IModTag* IModTagManager::FindTagByID(const kxf::String& id) const
	{
		return FindModTag<IModTag, FindBy::ID>(GetTags(), id);
	}
	IModTag* IModTagManager::FindTagByID(const IModTag::Vector& items, const kxf::String& id) const
	{
		return FindModTag<IModTag, FindBy::ID>(items, id);
	}

	IModTag* IModTagManager::FindTagByName(const kxf::String& name) const
	{
		return FindModTag<IModTag, FindBy::Name>(GetTags(), name);
	}
	IModTag* IModTagManager::FindTagByName(const IModTag::Vector& items, const kxf::String& name) const
	{
		return FindModTag<IModTag, FindBy::Name>(items, name);
	}

	kxf::String IModTagManager::GetTagNameByID(const kxf::String& id) const
	{
		return GetTagNameByID(GetTags(), id);
	}
	kxf::String IModTagManager::GetTagNameByID(const IModTag::Vector& items, const kxf::String& id) const
	{
		if (const IModTag* tag = FindTagByID(items, id))
		{
			return tag->GetName();
		}
		return wxEmptyString;
	}
}
