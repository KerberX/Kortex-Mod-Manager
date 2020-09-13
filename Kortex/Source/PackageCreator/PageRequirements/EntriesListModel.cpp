#include "stdafx.h"
#include <Kortex/PluginManager.hpp>
#include "EntriesListModel.h"
#include "PackageCreator/PageBase.h"
#include "PackageProject/ModPackageProject.h"
#include "ModPackages/IPackageManager.h"
#include "UI/TextEditDialog.h"
#include "UI/ImageViewerDialog.h"
#include "Utility/Common.h"
#include <Kortex/Application.hpp>
#include <KxFramework/KxString.h>
#include <KxFramework/KxAuiToolBar.h>
#include <KxFramework/KxFileBrowseDialog.h>
#include <KxFramework/KxTextBoxDialog.h>

namespace
{
	enum ColumnID
	{
		Type,
		ID,
		Name,
		RequiredVersion,
		CurrentVersion,
		RequiredState,
		Object,
		Description,
	};
	enum MenuID
	{
		AddEntry,
	};
}

namespace Kortex::PackageDesigner::PageRequirementsNS
{
	void EntriesListModel::OnInitControl()
	{
		/* View */
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_ACTIVATED, &EntriesListModel::OnActivateItem, this);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_EDIT_STARTING, &EntriesListModel::OnStartEditItem, this);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &EntriesListModel::OnContextMenuItem, this);
	
		/* Columns */
		// Type
		{
			auto info = GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewComboBoxEditor>(KTr("PackageCreator.PageRequirements.Type"), ColumnID::Type, KxDATAVIEW_CELL_EDITABLE, 115);
			m_TypeEditor = info.GetEditor();
			m_TypeEditor->SetItems
			({
				KTr("PackageCreator.PageRequirements.Type.User"),
				KTr("PackageCreator.PageRequirements.Type.System"),
				KTr("PackageCreator.PageRequirements.Type.Auto")
			 });
			
		}
	
		GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(KTr("Generic.ID"), ColumnID::ID, KxDATAVIEW_CELL_EDITABLE, 175);
		GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(KTr("Generic.Name"), ColumnID::Name, KxDATAVIEW_CELL_EDITABLE, 235);
		GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(KTr("PackageCreator.PageRequirements.RequiredVersion") + " (RV)", ColumnID::RequiredVersion, KxDATAVIEW_CELL_EDITABLE, 150);
		GetView()->AppendColumn<KxDataViewTextRenderer>(KTr("PackageCreator.PageRequirements.CurrentVersion") + " (CV)", ColumnID::CurrentVersion, KxDATAVIEW_CELL_INERT, 200);
	
		// Object function
		{
			auto info = GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewComboBoxEditor>(KTr("PackageCreator.PageRequirements.RequiredState"), ColumnID::RequiredState, KxDATAVIEW_CELL_EDITABLE, 150);
			m_ObjectFunctionEditor = info.GetEditor();
	
			KxStringVector choices;
			choices.push_back(Utility::MakeNoneLabel());
			choices.push_back(KTr("PackageCreator.PageRequirements.RequiredState.ModActive"));
			choices.push_back(KTr("PackageCreator.PageRequirements.RequiredState.ModInactive"));
			choices.push_back(KTr("PackageCreator.PageRequirements.RequiredState.FileExist"));
			choices.push_back(KTr("PackageCreator.PageRequirements.RequiredState.FileNotExist"));
	
			if (IPluginManager::GetInstance())
			{
				choices.push_back(KTr("PackageCreator.PageRequirements.RequiredState.PluginActive"));
				choices.push_back(KTr("PackageCreator.PageRequirements.RequiredState.PluginInactive"));
			}
			m_ObjectFunctionEditor->SetItems(choices);
		}
	
		GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(KTr("PackageCreator.PageRequirements.Object"), ColumnID::Object, KxDATAVIEW_CELL_EDITABLE, 400);
		GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(KTr("Generic.Description"), ColumnID::Description, KxDATAVIEW_CELL_INERT, KxCOL_WIDTH_AUTOSIZE);
	}
	
	void EntriesListModel::GetEditorValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const
	{
		const PackageProject::RequirementItem* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::Type:
				{
					value = entry->GetType();
					return;
				}
				case ColumnID::ID:
				{
					value = entry->RawGetID();
					return;
				}
				case ColumnID::Name:
				{
					value = entry->RawGetName();
					return;
				}
				case ColumnID::RequiredVersion:
				{
					value = entry->GetRequiredVersion().ToString();
					return;
				}
				case ColumnID::RequiredState:
				{
					value = entry->GetObjectFunction();
					return;
				}
				case ColumnID::Object:
				{
					value = entry->GetObject();
					return;
				}
				case ColumnID::Description:
				{
					value = entry->GetDescription();
					return;
				}
			};
		}
	}
	void EntriesListModel::GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const
	{
		const PackageProject::RequirementItem* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::Type:
				{
					value = m_TypeEditor->GetItems()[ToInt(entry->GetType())];
					break;
				}
				case ColumnID::ID:
				{
					value = entry->IsEmptyID() ? Utility::MakeNoneLabel() : entry->GetID();
					break;
				}
				case ColumnID::Name:
				{
					value = entry->IsEmptyName() ? Utility::MakeNoneLabel() : entry->GetName();
					break;
				}
				case ColumnID::RequiredVersion:
				{
					value = entry->GetRequiredVersion().IsOK() ? entry->GetRequiredVersion().ToString() : Utility::MakeNoneLabel();
					break;
				}
				case ColumnID::CurrentVersion:
				{
					kxf::String operatorName = ModPackageProject::OperatorToSymbolicName(entry->GetRequiredVersionOperator());
					kxf::String cv = entry->GetCurrentVersion();
					kxf::String rv = entry->GetRequiredVersion();
	
					value = kxf::String::Format("CV(%1) %2 RV(%3) %4 %5", cv, operatorName, rv, Utility::String::GetUnicodeChar(Utility::CharCode::ArrowRight), entry->CheckVersion());
					break;
				}
				case ColumnID::RequiredState:
				{
					KxFormat format("%1 %2 %3");
					format(m_ObjectFunctionEditor->GetItems()[ToInt(entry->GetObjectFunction())]);
					format(Utility::String::GetUnicodeChar(Utility::CharCode::ArrowRight));
					format(entry->GetObjectFunctionResult() == PackageProject::ReqState::True);
	
					value = format.ToString();
					break;
				}
				case ColumnID::Object:
				{
					value = entry->GetObject().IsEmpty() ? Utility::MakeNoneLabel() : entry->GetObject();
					break;
				}
				case ColumnID::Description:
				{
					value = entry->GetDescription().IsEmpty() ? Utility::MakeNoneLabel() : entry->GetDescription();
					break;
				}
			};
		}
	}
	bool EntriesListModel::SetValueByRow(const wxAny& data, size_t row, const KxDataViewColumn* column)
	{
		PackageProject::RequirementItem* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::Type:
				{
					entry->ResetCurrentVersion();
					entry->ResetObjectFunctionResult();
	
					entry->SetType(data.As<PackageProject::ReqType>());
					ChangeNotify();
					break;
				}
				case ColumnID::ID:
				{
					kxf::String newID = data.As<kxf::String>();
					if (newID != entry->GetID())
					{
						if (newID.IsEmpty() || !m_Group->HasItemWithID(newID))
						{
							entry->ResetCurrentVersion();
							entry->ResetObjectFunctionResult();
	
							entry->SetID(newID);
							entry->ConformToType();
							ChangeNotify();
						}
						else
						{
							PageBase::WarnIDCollision(GetView(), GetView()->GetAdjustedItemRect(GetItem(row), column));
							return false;
						}
					}
					break;
				}
				case ColumnID::Name:
				{
					entry->ResetCurrentVersion();
					entry->ResetObjectFunctionResult();
	
					entry->SetName(data.As<kxf::String>());
					entry->ConformToType();
					ChangeNotify();
					break;
				}
				case ColumnID::RequiredVersion:
				{
					entry->ResetObjectFunctionResult();
	
					entry->SetRequiredVersion(data.As<kxf::String>());
					ChangeNotify();
					break;
				}
				case ColumnID::RequiredState:
				{
					entry->ResetObjectFunctionResult();
	
					entry->SetObjectFunction(data.As<PackageProject::ObjectFunction>());
					ChangeNotify();
					break;
				}
				case ColumnID::Object:
				{
					entry->ResetCurrentVersion();
					entry->ResetObjectFunctionResult();
	
					entry->SetObject(data.As<kxf::String>());
					ChangeNotify();
					break;
				}
			};
		}
		return true;
	}
	bool EntriesListModel::IsEnabledByRow(size_t row, const KxDataViewColumn* column) const
	{
		switch (column->GetID())
		{
			case ColumnID::ID:
			case ColumnID::Name:
			case ColumnID::RequiredState:
			case ColumnID::Object:
			{
				if (PackageProject::RequirementItem* entry = GetDataEntry(row))
				{
					return entry->IsTypeUserEditable();
				}
				break;
			}
		};
		return true;
	}
	bool EntriesListModel::GetItemAttributesByRow(size_t row, const KxDataViewColumn* column, KxDataViewItemAttributes& attributes, KxDataViewCellState cellState) const
	{
		const PackageProject::RequirementItem* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::CurrentVersion:
				{
					attributes.SetFontFace("Consolas");
					break;
				}
				case ColumnID::RequiredVersion:
				{
					if (entry->GetRequiredVersion().IsOK())
					{
						attributes.SetFontFace("Consolas");
					}
					break;
				}
			}
		};
	
		if (!IsEnabledByRow(row, column))
		{
			attributes.SetEnabled(false);
		}
		return !attributes.IsDefault();
	}
	
	void EntriesListModel::OnActivateItem(KxDataViewEvent& event)
	{
		KxDataViewItem item = event.GetItem();
		KxDataViewColumn* column = event.GetColumn();
		PackageProject::RequirementItem* entry = GetDataEntry(GetRow(event.GetItem()));
	
		if (column)
		{
			switch (column->GetID())
			{
				case ColumnID::Type:
				case ColumnID::RequiredVersion:
				{
					GetView()->EditItem(item, column);
					break;
				}
				case ColumnID::CurrentVersion:
				{
					if (entry)
					{
						KxMenu menu;
						for (int i = ToInt(PackageProject::Operator::MIN); i < ToInt(PackageProject::Operator::MAX_COMPARISON); i++)
						{
							KxMenuItem* item = menu.Add(new KxMenuItem(i, ModPackageProject::OperatorToSymbolicName((PackageProject::Operator)i), wxEmptyString, wxITEM_CHECK));
							item->Check(i == ToInt(entry->GetRequiredVersionOperator()));
						}
	
						wxWindowID id = menu.Show(GetView(), GetView()->GetDropdownMenuPosition(item, column) + wxPoint(0, 1));
						if (id != KxID_NONE)
						{
							entry->SetRequiredVersionOperator((PackageProject::Operator)id);
							NotifyChangedItem(item);
						}
					}
					break;
				}
				case ColumnID::ID:
				case ColumnID::Name:
				case ColumnID::RequiredState:
				case ColumnID::Object:
				{
					if (entry && entry->IsTypeUserEditable())
					{
						GetView()->EditItem(item, column);
					}
					else
					{
						wxBell();
					}
					break;
				}
				case ColumnID::Description:
				{
					if (entry)
					{
						UI::TextEditDialog dialog(GetView());
						dialog.SetText(entry->GetDescription());
						if (dialog.ShowModal() == KxID_OK && dialog.IsModified())
						{
							entry->SetDescription(dialog.GetText());
							NotifyChangedItem(item);
						}
					}
					break;
				}
			};
		}
	}
	void EntriesListModel::OnStartEditItem(KxDataViewEvent& event)
	{
		switch (event.GetColumn()->GetID())
		{
			case ColumnID::ID:
			case ColumnID::Name:
			case ColumnID::RequiredState:
			case ColumnID::Object:
			{
				PackageProject::RequirementItem* entry = GetDataEntry(GetRow(event.GetItem()));
				if (entry && !entry->IsTypeUserEditable())
				{
					event.Veto();
					wxBell();
				}
				break;
			}
		};
	}
	void EntriesListModel::OnContextMenuItem(KxDataViewEvent& event)
	{
		KxDataViewItem item = event.GetItem();
		const PackageProject::RequirementItem* entry = GetDataEntry(GetRow(item));
	
		KxMenu menu;
		{
			KxMenu* allItems = CreateAllItemsMenu(menu);
			CreateAllItemsMenuEntry(allItems, ColumnID::Name);
			CreateAllItemsMenuEntry(allItems, ColumnID::RequiredVersion);
			CreateAllItemsMenuEntry(allItems, ColumnID::Object);
			CreateAllItemsMenuEntry(allItems, ColumnID::Description);
			menu.AddSeparator();
		}
	
		{
			KxMenuItem* item = menu.Add(new KxMenuItem(MenuID::AddEntry, KTr(KxID_ADD)));
			item->SetBitmap(ImageProvider::GetBitmap(Imagekxf::ResourceID::ChequePlus));
		}
		menu.AddSeparator();
		{
			KxMenuItem* item = menu.Add(new KxMenuItem(KxID_REMOVE, KTr(KxID_REMOVE)));
			item->Enable(entry != nullptr);
		}
		{
			KxMenuItem* item = menu.Add(new KxMenuItem(KxID_CLEAR, KTr(KxID_CLEAR)));
			item->Enable(!IsEmpty());
		}
	
		switch (menu.Show(GetView()))
		{
			case MenuID::AddEntry:
			{
				OnAddEntry();
				break;
			}
			case KxID_REMOVE:
			{
				OnRemoveEntry(item);
				break;
			}
			case KxID_CLEAR:
			{
				OnClearList();
				break;
			}
		};
	}
	void EntriesListModel::OnAllItemsMenuSelect(KxDataViewColumn* column)
	{
		switch (column->GetID())
		{
			case ColumnID::Name:
			case ColumnID::RequiredVersion:
			case ColumnID::Object:
			{
				KxTextBoxDialog dialog(GetView(), KxID_NONE, column->GetTitle());
				if (dialog.ShowModal() == KxID_OK)
				{
					for (auto& entry: *GetDataVector())
					{
						if (column->GetID() == ColumnID::Name)
						{
							if (entry->IsTypeUserEditable())
							{
								entry->SetName(dialog.GetValue());
							}
						}
						else if (column->GetID() == ColumnID::RequiredVersion)
						{
							entry->SetRequiredVersion(dialog.GetValue());
						}
						else if (column->GetID() == ColumnID::Object)
						{
							if (entry->IsTypeUserEditable())
							{
								entry->SetObject(dialog.GetValue());
							}
						}
					}
					NotifyAllItemsChanged();
				}
				break;
			}
			case ColumnID::Description:
			{
				UI::TextEditDialog dialog(GetView());
				if (dialog.ShowModal() == KxID_OK)
				{
					for (auto& entry: *GetDataVector())
					{
						entry->SetDescription(dialog.GetText());
					}
					NotifyAllItemsChanged();
				}
				break;
			}
		};
	}
	
	void EntriesListModel::OnAddEntry()
	{
		GetDataVector()->emplace_back(new PackageProject::RequirementItem(PackageProject::ReqType::User));
	
		KxDataViewItem item = GetItem(GetItemCount() - 1);
		NotifyAddedItem(item);
		SelectItem(item);
		GetView()->EditItem(item, GetView()->GetColumn(ColumnID::ID));
	}
	void EntriesListModel::OnRemoveEntry(const KxDataViewItem& item)
	{
		RemoveItemAndNotify(*GetDataVector(), item);
	}
	void EntriesListModel::OnClearList()
	{
		ClearItemsAndNotify(*GetDataVector());
	}
	
	void EntriesListModel::SetProject(ModPackageProject& projectData)
	{
		m_Requirements = &projectData.GetRequirements();
	}
}
