#pragma once
#include <Kortex/Kortex.hpp>
#include "KxFramework/KxDataViewListModelEx.h"
#include "Store.h"

namespace Kortex::ModSource
{
	class StoreDisplayModel: public KxDataViewListModelExBase<KxDataViewModel>
	{
		protected:
			enum ColumnID
			{
				Name,
				Value
			};

		protected:
			ModSourceStore& m_ModSourceStore;
			bool m_IsModified = false;

		protected:
			void OnInitControl() override;

			void GetChildren(const KxDataViewItem& item, KxDataViewItem::Vector& children) const override;
			bool IsContainer(const KxDataViewItem& item) const override;
			KxDataViewItem GetParent(const KxDataViewItem& item) const override;

			bool IsEnabled(const KxDataViewItem& item, const KxDataViewColumn* column) const override;
			void GetEditorValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const override;
			void GetValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const override;
			bool SetValue(const wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) override;

			void OnActivate(KxDataViewEvent& event);

		public:
			StoreDisplayModel(ModSourceStore& store)
				:m_ModSourceStore(store)
			{
			}

		public:
			const ModSourceItem* GetNode(const KxDataViewItem& item) const
			{
				return item.GetValuePtr<ModSourceItem>();
			}
			ModSourceItem* GetNode(const KxDataViewItem& item)
			{
				return item.GetValuePtr<ModSourceItem>();
			}
			KxDataViewItem MakeItem(const ModSourceItem& node)
			{
				return KxDataViewItem(&node);
			}

			bool IsModified() const
			{
				return m_IsModified;
			}
			void ApplyChanges();
	};
}
