#pragma once
#include <Kortex/Kortex.hpp>
#include "IModTag.h"
#include "Application/IManager.h"
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex
{
	namespace ModTagManager::Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	}

	class IGameMod;

	class IModTagManager:
		public ManagerWithTypeInfo<IManager, ModTagManager::Internal::TypeInfo>,
		public kxf::SingletonPtr<IModTagManager>
	{
		public:
			IModTagManager();

		public:
			virtual const IModTag::Vector& GetDefaultTags() const = 0;
			virtual const IModTag::Vector& GetTags() const = 0;
			virtual IModTag::Vector& GetTags() = 0;
			size_t GetTagsCount() const
			{
				return GetTags().size();
			}
			bool HasTags() const
			{
				return !GetTags().empty();
			}

			virtual void OnInitialModsLoading() = 0;
			virtual void LoadTagsFromMod(IModTag::Vector& items, const IGameMod& mod) = 0;
			void LoadTagsFromMod(const IGameMod& mod)
			{
				LoadTagsFromMod(GetTags(), mod);
			}
			
			void LoadDefaultTags(IModTag::Vector& items);
			void LoadDefaultTags();

			virtual std::unique_ptr<IModTag> NewTag() = 0;

			IModTag& EmplaceTag();
			IModTag& EmplaceTag(IModTag::Vector& items);

			IModTag& EmplaceTag(std::unique_ptr<IModTag> tag);
			IModTag& EmplaceTag(IModTag::Vector& items, std::unique_ptr<IModTag> tag);

			IModTag& EmplaceTagWith(const kxf::String& id, const kxf::String& name = wxEmptyString);
			IModTag& EmplaceTagWith(IModTag::Vector& items, const kxf::String& id, const kxf::String& name = wxEmptyString);

			void RemoveAllTags();
			void RemoveAllTags(IModTag::Vector& items);

			bool RemoveTag(IModTag& tag);
			bool RemoveTag(IModTag::Vector& items, IModTag& tag);

			bool RemoveTagByID(const kxf::String& id);
			bool RemoveTagByID(IModTag::Vector& items, const kxf::String& id);

			IModTag* FindTagByID(const kxf::String& id) const;
			IModTag* FindTagByID(const IModTag::Vector& items, const kxf::String& id) const;

			IModTag* FindTagByName(const kxf::String& name) const;
			IModTag* FindTagByName(const IModTag::Vector& items, const kxf::String& name) const;

			kxf::String GetTagNameByID(const kxf::String& id) const;
			kxf::String GetTagNameByID(const IModTag::Vector& items, const kxf::String& id) const;
	};
}
