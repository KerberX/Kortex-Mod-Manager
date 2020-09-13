#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "ISource.h"
#include "Item.h"
#include "ItemOptions.h"
class kxf::XMLNode;

namespace Kortex
{
	class IConfigManager;
}

namespace Kortex::GameConfig
{
	class Definition;

	class ItemGroup
	{
		friend class Item;

		private:
			Definition& m_Definition;

			kxf::String m_ID;
			ItemOptions m_Options;
			SourceTypeValue m_SourceType = SourceType::None;
			std::unique_ptr<ISource> m_Source;

			std::vector<std::unique_ptr<Item>> m_Items;
			std::unordered_map<size_t, Item*> m_ItemsHash;
			bool m_Destructing = false;

		private:
			void LoadItems(const kxf::XMLNode& groupNode);
			void RemoveAllUnknownItems();

			void AddKnownItem(size_t hash, Item& item);
			void RemoveKnownItem(size_t hash);

			template<class TItems, class TFunctor> static void DoForEachItem(TItems&& items, TFunctor&& func)
			{
				for (auto&& item: items)
				{
					func(*item);
				}
			}
			
		public:
			ItemGroup(Definition& definition, const kxf::String& id, const kxf::XMLNode& groupNode, const ItemOptions& parentOptions);
			~ItemGroup();

		public:
			bool IsOK() const
			{
				return !m_ID.IsEmpty() && m_Source != nullptr;
			}
			kxf::String GetID() const
			{
				return m_ID;
			}
			const ItemOptions& GetOptions() const
			{
				return m_Options;
			}
			Definition& GetDefinition() const
			{
				return m_Definition;
			}
			IConfigManager& GetManager() const;

			const ISource& GetSource() const
			{
				return *m_Source;
			}
			ISource& GetSource()
			{
				return *m_Source;
			}
			void AssignSource(std::unique_ptr<ISource> source)
			{
				m_Source = std::move(source);
			}

			void LoadItemsData();
			void OnLoadInstance(const kxf::XMLNode& groupNode);

			template<class TFunctor> void ForEachItem(TFunctor&& func) const
			{
				DoForEachItem(m_Items, func);
			}
			template<class TFunctor> void ForEachItem(TFunctor&& func)
			{
				DoForEachItem(m_Items, func);
			}

			bool HasItem(const Item& item) const
			{
				return HasItem(item.GetHash());
			}
			bool HasItem(size_t hash) const
			{
				return m_ItemsHash.find(hash) != m_ItemsHash.end();
			}
			
			template<class T, class... Args> auto NewItem(Args&&... arg)
			{
				return std::make_unique<T>(std::forward<Args>(arg)...);
			}
			template<class T> T& AddItem(std::unique_ptr<T> item)
			{
				return static_cast<T&>(*m_Items.emplace_back(std::move(item)));
			}
			template<class T, class... Args> T& EmplaceItem(Args&&... arg)
			{
				return AddItem(NewItem<T>(std::forward<Args>(arg)...));
			}
	};
}
