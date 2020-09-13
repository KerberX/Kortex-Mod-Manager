#include "stdafx.h"
#include "CategoryItem.h"
#include "GameConfig/IGameConfigManager.h"
#include "GameConfig/ConfigManger/Common.h"
#include <KxFramework/KxComparator.h>

namespace Kortex::GameConfig
{
	kxf::String CategoryItem::TranslateCategoryName() const
	{
		const IGameConfigManager* manager = IGameConfigManager::GetInstance();
		if (!m_CategoryPath.IsEmpty())
		{
			kxf::String name = m_CategoryPath.AfterLast(wxS('/'));

			kxf::String label = manager->TranslateItemLabel(name, wxS("Category"));
			if (label.IsEmpty())
			{
				label = manager->TranslateItemLabel(name, wxS("Category.ENB"));
			}
			return label;
		}
		return manager->GetTranslator().GetString(wxS("Category.None"));
	}

	CategoryItem::CategoryItem(const kxf::String& categoryPath)
		:m_CategoryPath(categoryPath), m_CategoryName(TranslateCategoryName())
	{
	}
	CategoryItem::CategoryItem(const kxf::String& categoryPath, const kxf::String& categoryName)
		:m_CategoryPath(categoryPath), m_CategoryName(categoryName)
	{
	}
	CategoryItem::~CategoryItem()
	{
		DetachAllChildren();
	}

	kxf::String CategoryItem::GetViewString(ColumnID id) const
	{
		if (id == ColumnID::Path)
		{
			return m_CategoryName;
		}
		if (id == ColumnID::Name)
		{
			KxDataView2::Column* column = GetView()->GetColumnByID(ColumnID::Path);
			if (column && !column->IsVisible())
			{
				return m_CategoryName;
			}
		}
		return kxf::String();
	}
	void CategoryItem::OnActivate(KxDataView2::Column& column)
	{
		if (column.GetID<ColumnID>() == ColumnID::Path)
		{
			ToggleExpanded();
		}
	}

	wxAny CategoryItem::GetValue(const KxDataView2::Column& column) const
	{
		return GetViewString(column.GetID<ColumnID>());
	}
	KxDataView2::Renderer& CategoryItem::GetRenderer(const KxDataView2::Column& column) const
	{
		return column.GetRenderer();
	}
	KxDataView2::Editor* CategoryItem::GetEditor(const KxDataView2::Column& column) const
	{
		return nullptr;
	}
	bool CategoryItem::GetAttributes(const KxDataView2::Column& column, const KxDataView2::CellState& cellState, KxDataView2::CellAttributes& attributes) const
	{
		return false;
	}
}
