#include "stdafx.h"
#include <Kortex/Application.hpp>
#include <Kortex/InstallWizard.hpp>
#include "InfoDisplayModel.h"
#include "PackageCreator/PageComponents.h"
#include <Kortex/ModManager.hpp>
#include <Kortex/ModTagManager.hpp>
#include "UI/TextEditDialog.h"
#include "Utility/Common.h"
#include "Utility/UI.h"
#include "Utility/BitmapSize.h"
#include <KxFramework/KxTextBoxDialog.h>
#include <KxFramework/KxTaskDialog.h>

namespace
{
	using namespace KxDataView2;
	enum class ColumnRef
	{
		Name,
		Value,
	};
}

namespace Kortex::InstallWizard::InfoPageNS
{
	wxAny InfoDisplayModel::GetValue(const Node& node, const Column& column) const
	{
		const Item& item = m_Items[node.GetRow()];
		const Utility::LabeledValue& itemValue = item.Value;

		switch (column.GetID<ColumnRef>())
		{
			case ColumnRef::Name:
			{
				BitmapTextValue value;

				// Label
				if (!itemValue.HasLabel())
				{
					value.SetText(KxURI(itemValue.GetValue()).GetServer());
				}
				else
				{
					value.SetText(itemValue.GetLabel());
				}

				// Icon
				if (item.IconID)
				{
					value.SetBitmap(ImageProvider::GetBitmap(item.IconID));
				}
				else
				{
					value.SetDefaultBitmapWidth(Utility::BitmapSize().FromSystemSmallIcon().GetWidth());
				}

				return value;
			}
			case ColumnRef::Value:
			{
				ModPackageProject& packageConfig = m_Page.GetPackageConfig();
				switch (item.Type)
				{
					case InfoKind::Tags:
					{
						const ModTagStore& tags = packageConfig.GetInfo().GetTagStore();
						return PackageDesigner::PageComponents::FormatArrayToText(tags.GetNames());
					}
					case InfoKind::ID:
					{
						return kxf::String::Format(wxS("%1 (%2)"), packageConfig.GetModID(), packageConfig.GetSignature());
					}
					case InfoKind::Name:
					{
						return packageConfig.GetModName();
					}
				};
				return itemValue.GetValue();
			}
		};
		return {};
	}
	wxAny InfoDisplayModel::GetEditorValue(const Node& node, const Column& column) const
	{
		if (column.GetID<ColumnRef>() == ColumnRef::Value)
		{
			const Item& item = m_Items[node.GetRow()];
			ModPackageProject& packageConfig = m_Page.GetPackageConfig();

			switch (item.Type)
			{
				case InfoKind::ID:
				{
					return packageConfig.GetModID();
				}
				case InfoKind::Name:
				{
					return packageConfig.GetModName();
				}
			};
		}
		return {};
	}
	bool InfoDisplayModel::SetValue(Node& node, Column& column, const wxAny& value)
	{
		if (column.GetID<ColumnRef>() == ColumnRef::Value)
		{
			const Item& item = m_Items[node.GetRow()];
			ModPackageProject& packageConfig = m_Page.GetPackageConfig();

			switch (item.Type)
			{
				case InfoKind::ID:
				{
					kxf::String id = value.As<kxf::String>();
					if (id != packageConfig.GetModID() && CheckModID(id))
					{
						packageConfig.SetModID(id);
						m_Page.GetWizard().FindExistingMod();
						return true;
					}
					return false;
				}
				case InfoKind::Name:
				{
					packageConfig.GetInfo().SetName(value.As<kxf::String>());
					return true;
				}
			};
		}
		return false;
	}
	bool InfoDisplayModel::IsEnabled(const Node& node, const Column& column) const
	{
		if (column.GetID<ColumnRef>() == ColumnRef::Value)
		{
			const Item& item = m_Items[node.GetRow()];
			switch (item.Type)
			{
				case InfoKind::ID:
				case InfoKind::Name:
				{
					return true;
				}
			};
		}
		return false;
	}
	bool InfoDisplayModel::GetAttributes(const Node& node, const Column& column, const CellState& cellState, CellAttributes& attributes) const
	{
		if (column.GetID<ColumnRef>() == ColumnRef::Value)
		{
			const Item& item = m_Items[node.GetRow()];
			switch (item.Type)
			{
				case InfoKind::ModSource:
				{
					attributes.FontOptions().Enable(CellFontOption::Underlined, cellState.IsHotTracked());
					return true;
				}
				case InfoKind::Tags:
				{
					attributes.BGOptions().Enable(CellBGOption::ComboBox);
					attributes.Options().Enable(CellOption::Editable, false);
					return true;
				}
			};
		}
		return false;
	}

	bool InfoDisplayModel::CheckModID(const kxf::String& id)
	{
		if (id.IsEmpty())
		{
			KxTaskDialog msg(GetView(), KxID_NONE, KTr("InstallWizard.ChangeID.Invalid"), wxEmptyString, KxBTN_OK, KxICON_WARNING);
			msg.ShowModal();
			return false;
		}
		return true;
	}
	void InfoDisplayModel::OnActivateItem(Event& event)
	{
		if (event.GetColumn() && event.GetColumn()->GetID<ColumnRef>() == ColumnRef::Value)
		{
			Node* node = event.GetNode();
			const Item& item = m_Items[node->GetRow()];
			const Utility::LabeledValue& itemValue = item.Value;

			switch (item.Type)
			{
				case InfoKind::ID:
				case InfoKind::Name:
				{
					if (Column* column = event.GetColumn())
					{
						node->Edit(*column);
					}
					break;
				}
				case InfoKind::ModSource:
				{
					if (itemValue.HasValue())
					{
						Utility::UI::AskOpenURL(itemValue.GetValue(), GetView());
					}
					break;
				}
				case InfoKind::Tags:
				{
					ModPackageProject& packageConfig = m_Page.GetPackageConfig();
					ModTagStore& tags = packageConfig.GetInfo().GetTagStore();

					ModTagManager::SelectorDialog dialog(GetView(), itemValue.GetLabel());
					dialog.SetDataVector(tags);
					dialog.ShowModal();
					if (dialog.IsModified())
					{
						dialog.ApplyChanges();
						node->Refresh();
					}
					break;
				}
				default:
				{
					if (itemValue.HasValue())
					{
						UI::TextEditDialog dialog(GetView());
						dialog.SetText(itemValue.GetValue());
						dialog.SetEditable(false);
						dialog.ShowPreview(true);
						dialog.ShowModal();
					}
					break;
				}
			};
		}
	}

	void InfoDisplayModel::CreateView(wxWindow* parent)
	{
		using namespace KxDataView2;

		// View
		View* view = new View(parent, KxID_NONE, CtrlStyle::VerticalRules|CtrlStyle::CellFocus|CtrlStyle::FitLastColumn);
		view->AssignModel(this);

		// Columns
		view->AppendColumn<BitmapTextRenderer>(KTr("Generic.Name"), ColumnRef::Name, 250);
		view->AppendColumn<TextRenderer, TextEditor>(KTr("Generic.Value"), ColumnRef::Value);

		// Events
		view->Bind(EvtITEM_ACTIVATED, &InfoDisplayModel::OnActivateItem, this);
	}
	void InfoDisplayModel::AddItem(const Utility::LabeledValue& value, const kxf::ResourceID& image, InfoKind type)
	{
		Item& item = m_Items.emplace_back(value);
		item.IconID = image;
		item.Type = type;

		SetItemCount(m_Items.size());
	}
}
