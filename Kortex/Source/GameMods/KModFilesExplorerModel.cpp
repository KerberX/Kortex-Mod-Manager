#include "stdafx.h"
#include "KModFilesExplorerModel.h"
#include "KModCollisionViewerModel.h"
#include "FileTreeNode.h"
#include <Kortex/Application.hpp>
#include "UI/KMainWindow.h"
#include "KAux.h"
#include <KxFramework/KxShell.h>
#include <KxFramework/KxFile.h>
#include <KxFramework/KxFileFinder.h>
#include <KxFramework/KxTaskDialog.h>
#include <KxFramework/KxString.h>

enum ColumnID
{
	Name,
	Collisions,
	ModificationDate,
	Type,
	Size,
};

namespace Kortex::ModManager
{
	void KModFilesExplorerModel::OnInitControl()
	{
		/* View */
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_SELECTED, &KModFilesExplorerModel::OnSelectItem, this);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_ACTIVATED, &KModFilesExplorerModel::OnActivateItem, this);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &KModFilesExplorerModel::OnContextMenu, this);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_EXPANDING, &KModFilesExplorerModel::OnExpandingItem, this);
		GetView()->SetIndent(20);

		/* Columns */
		GetView()->AppendColumn<KxDataViewBitmapTextRenderer>(KTr("Generic.Name"), ColumnID::Name, KxDATAVIEW_CELL_INERT, 300);
		GetView()->AppendColumn<KxDataViewBitmapTextRenderer>(KTr("ModExplorer.Collisions"), ColumnID::Collisions, KxDATAVIEW_CELL_INERT, 200);
		GetView()->AppendColumn<KxDataViewBitmapTextRenderer>(KTr("Generic.ModificationDate"), ColumnID::ModificationDate, KxDATAVIEW_CELL_INERT, 125);
		GetView()->AppendColumn<KxDataViewBitmapTextRenderer>(KTr("Generic.Type"), ColumnID::Type, KxDATAVIEW_CELL_INERT, 175);
		GetView()->AppendColumn<KxDataViewBitmapTextRenderer>(KTr("Generic.Size"), ColumnID::Size, KxDATAVIEW_CELL_INERT, 125);
	}

	bool KModFilesExplorerModel::IsContainer(const KxDataViewItem& item) const
	{
		if (const FileTreeNode* node = GetNode(item))
		{
			return node->HasChildren();
		}
		return false;
	}
	KxDataViewItem KModFilesExplorerModel::GetParent(const KxDataViewItem& item) const
	{
		if (const FileTreeNode* node = GetNode(item))
		{
			if (node->HasParent())
			{
				return MakeItem(node->GetParent());
			}
		}
		return KxDataViewItem();
	}
	void KModFilesExplorerModel::GetChildren(const KxDataViewItem& item, KxDataViewItem::Vector& children) const
	{
		// Root item, read groups
		if (item.IsTreeRootItem())
		{
			children.reserve(m_ModEntry.GetFileTree().GetChildrenCount());
			for (const FileTreeNode& node: m_ModEntry.GetFileTree().GetChildren())
			{
				children.push_back(MakeItem(node));
			}
			return;
		}

		// Group item, read entries
		if (const FileTreeNode* node = GetNode(item))
		{
			children.reserve(node->GetChildrenCount());
			for (const FileTreeNode& childNode: node->GetChildren())
			{
				children.push_back(MakeItem(childNode));
			}
		}
	}

	void KModFilesExplorerModel::GetEditorValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const
	{
		if (const FileTreeNode* node = GetNode(item))
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					value = node->GetName();
					return;
				}
			};
		}
		GetValue(value, item, column);
	}
	void KModFilesExplorerModel::GetValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const
	{
		if (const FileTreeNode* node = GetNode(item))
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					wxIcon icon;
					if (node->IsFile())
					{
						icon = KxShell::GetFileIcon(node->GetFullPath(), true);
						if (!icon.IsOk())
						{
							icon = KGetIcon(KIMG_DOCUMENT);
						}
					}
					else
					{
						icon = KGetIcon(KIMG_FOLDER);
					}

					value = KxDataViewBitmapTextValue(node->GetName(), icon);
					break;
				}
				case ColumnID::Collisions:
				{
					if (node->IsFile())
					{
						const KDispatcherCollision::Vector* collisions = GetCollisions(*node);
						if (collisions)
						{
							value = KModCollisionViewerModel::FormatCollisionsCount(*collisions);
							return;
						}
						value = 0;
					}
					break;
				}
				case ColumnID::ModificationDate:
				{
					value = KAux::FormatDateTime(node->GetItem().GetModificationTime());
					break;
				}
				case ColumnID::Type:
				{
					value = node->IsDirectory() ? KTr(KxID_FOLDER) : KxShell::GetTypeName(node->GetName());
					break;
				}
				case ColumnID::Size:
				{
					value = node->IsFile() ? KxFile::FormatFileSize(node->GetFileSize()) : wxEmptyString;
					break;
				}
			};
		}
	}
	bool KModFilesExplorerModel::SetValue(const wxAny& data, const KxDataViewItem& item, const KxDataViewColumn* column)
	{
		return false;
	}
	bool KModFilesExplorerModel::IsEnabled(const KxDataViewItem& item, const KxDataViewColumn* column) const
	{
		if (column->GetID() == ColumnID::Name)
		{
			return GetNode(item) != nullptr;
		}
		return false;
	}

	void KModFilesExplorerModel::OnSelectItem(KxDataViewEvent& event)
	{
	}
	void KModFilesExplorerModel::OnActivateItem(KxDataViewEvent& event)
	{
		KxDataViewItem item = event.GetItem();
		KxDataViewColumn* column = event.GetColumn();
		FileTreeNode* node = GetNode(item);

		if (node->IsDirectory())
		{
			GetView()->ToggleItemExpanded(item);
		}
		else if (column)
		{
			switch (column->GetID())
			{
				case ColumnID::Collisions:
				{
					const KDispatcherCollision::Vector* collisions = GetCollisions(*node);
					if (collisions && !collisions->empty())
					{
						KxTaskDialog dialog(GetViewTLW(), KxID_NONE, column->GetTitle(), KModCollisionViewerModel::FormatCollisionsView(*collisions));
						dialog.ShowModal();
					}
					break;
				}
			};
		}
	}
	void KModFilesExplorerModel::OnContextMenu(KxDataViewEvent& event)
	{
		const FileTreeNode* node = GetNode(event.GetItem());
		if (node)
		{
		}
	}
	void KModFilesExplorerModel::OnExpandingItem(KxDataViewEvent& event)
	{
		const FileTreeNode* rootNode = GetNode(event.GetItem());
		if (rootNode)
		{
			for (const FileTreeNode& node: rootNode->GetChildren())
			{
				if (node.IsFile())
				{
					GetOrUpdateCollisions(node);
				}
			}
		}
	}

	const KDispatcherCollision::Vector* KModFilesExplorerModel::GetCollisions(const FileTreeNode& node) const
	{
		auto it = m_Collisions.find(&node);
		if (it != m_Collisions.end())
		{
			return &it->second;
		}
		return nullptr;
	}
	const KDispatcherCollision::Vector& KModFilesExplorerModel::GetOrUpdateCollisions(const FileTreeNode& node)
	{
		auto it = m_Collisions.find(&node);
		if (it != m_Collisions.end())
		{
			return it->second;
		}
		else
		{
			wxString path = node.GetFullPath().Remove(0, m_ModEntry.GetModFilesDir().Length());
			//auto it = m_Collisions.insert_or_assign(&node, IModDispatcher::GetInstance()->FindCollisions(m_ModEntry, path));
			
			auto it = m_Collisions.insert_or_assign(&node, KDispatcherCollision::Vector());
			return it.first->second;
		}
	}

	void KModFilesExplorerModel::RefreshItems()
	{
		m_Collisions.clear();
		ItemsCleared();

		for (const FileTreeNode& node: m_ModEntry.GetFileTree().GetChildren())
		{
			if (node.IsFile())
			{
				GetOrUpdateCollisions(node);
			}
			ItemAdded(MakeItem(node));
		}
	}
}