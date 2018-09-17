#pragma once
#include "stdafx.h"
#include "KModManagerDispatcher.h"
#include "KDataViewListModel.h"
#include "KImageProvider.h"
#include "KFileTreeNode.h"
class KModEntry;
class KFileTreeNode;

class KModManagerVirtualGameFolderModel: public KxDataViewModelExBase<KxDataViewModel>
{
	private:
		wxString m_SearchMask;

		const KFileTreeNode::Vector* m_TreeItems = NULL;
		KFileTreeNode::Vector m_FoundItems;

	private:
		virtual void OnInitControl() override;
		virtual bool IsContainer(const KxDataViewItem& item) const override;
		virtual bool HasContainerColumns(const KxDataViewItem& item) const override
		{
			return true;
		}
		virtual KxDataViewItem GetParent(const KxDataViewItem& item) const override;
		virtual void GetChildren(const KxDataViewItem& item, KxDataViewItem::Vector& children) const override;
		
		virtual void GetEditorValue(wxAny& data, const KxDataViewItem& item, const KxDataViewColumn* column) const override;
		virtual void GetValue(wxAny& data, const KxDataViewItem& item, const KxDataViewColumn* column) const override;
		virtual bool SetValue(const wxAny& data, const KxDataViewItem& item, const KxDataViewColumn* column) override;
		virtual bool IsEnabled(const KxDataViewItem& item, const KxDataViewColumn* column) const override;

		virtual bool GetItemAttributes(const KxDataViewItem& item, const KxDataViewColumn* column, KxDataViewItemAttributes& attributes, KxDataViewCellState cellState) const override;
		virtual bool HasDefaultCompare() const override
		{
			return true;
		}
		virtual bool Compare(const KxDataViewItem& item1, const KxDataViewItem& item2, const KxDataViewColumn* column) const override;

		void OnSelectItem(KxDataViewEvent& event);
		void OnActivateItem(KxDataViewEvent& event);
		void OnContextMenu(KxDataViewEvent& event);

	public:
		KModManagerVirtualGameFolderModel();

	public:
		virtual void RefreshItems() override;

		bool IsInSearchMode() const
		{
			return !m_SearchMask.IsEmpty();
		}
		bool SetSearchMask(const wxString& mask);

		KxDataViewItem MakeItem(const KFileTreeNode* node) const
		{
			return KxDataViewItem(node);
		}
		KxDataViewItem MakeItem(const KFileTreeNode& node) const
		{
			return MakeItem(&node);
		}
		KFileTreeNode* GetNode(const KxDataViewItem& item) const
		{
			return item.GetValuePtr<KFileTreeNode>();
		}
};
