#include "stdafx.h"
#include "SitesModel.h"
#include "PackageCreator/PageBase.h"
#include "UI/TextEditDialog.h"
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxString.h>
#include <kxf::UI::Framework/KxURI.h>
#include <kxf::UI::Framework/KxFileBrowseDialog.h>

namespace
{
	enum ColumnID
	{
		Name,
		Value,
	};
	enum MenuID
	{
		AddSite,
	};
}

namespace Kortex::PackageDesigner::PageInfoNS
{
	void SitesModel::OnInitControl()
	{
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_ACTIVATED, &SitesModel::OnActivateItem, this);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &SitesModel::OnContextMenu, this);
	
		GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(KTr("Generic.Name"), ColumnID::Name, KxDATAVIEW_CELL_EDITABLE, 200);
		GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(KTr("Generic.Address"), ColumnID::Value, m_UseInlineEditor ? KxDATAVIEW_CELL_EDITABLE : KxDATAVIEW_CELL_INERT, 300);
	}
	
	void SitesModel::GetEditorValueByRow(wxAny& data, size_t row, const KxDataViewColumn* column) const
	{
		const Utility::LabeledValue* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					data = entry->GetRawLabel();
					return;
				}
				case ColumnID::Value:
				{
					data = entry->GetValue();
					return;
				}
			};
		}
	}
	void SitesModel::GetValueByRow(wxAny& data, size_t row, const KxDataViewColumn* column) const
	{
		const Utility::LabeledValue* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					data = entry->HasLabel() ? entry->GetRawLabel() : KxURI(entry->GetValue()).GetServer();
					break;
				}
				case ColumnID::Value:
				{
					data = entry->GetValue();
					break;
				}
			};
		}
	}
	bool SitesModel::SetValueByRow(const wxAny& data, size_t row, const KxDataViewColumn* column)
	{
		Utility::LabeledValue* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					entry->SetLabel(data.As<kxf::String>());
					ChangeNotify();
					return true;
				}
				case ColumnID::Value:
				{
					entry->SetValue(data.As<kxf::String>());
					ChangeNotify();
					return true;
				}
			};
		}
		return false;
	}
	
	void SitesModel::OnActivateItem(KxDataViewEvent& event)
	{
		KxDataViewItem item = event.GetItem();
		KxDataViewColumn* column = event.GetColumn();
		if (column)
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					GetView()->EditItem(item, column);
					break;
				}
				case ColumnID::Value:
				{
					if (m_UseInlineEditor)
					{
						GetView()->EditItem(item, column);
					}
					else
					{
						if (Utility::LabeledValue* entry = GetDataEntry(GetRow(item)))
						{
							UI::TextEditDialog dialog(GetView());
							dialog.SetText(entry->GetValue());
							if (dialog.ShowModal() == wxID_OK && dialog.IsModified())
							{
								entry->SetValue(dialog.GetText());
								NotifyChangedItem(item);
							}
						}
					}
					break;
				}
			};
		}
	}
	void SitesModel::OnContextMenu(KxDataViewEvent& event)
	{
		KxDataViewItem item = event.GetItem();
		const Utility::LabeledValue* entry = GetDataEntry(GetRow(item));
	
		kxf::UI::Menu menu;
		{
			kxf::UI::MenuItem* item = menu.Add(new kxf::UI::MenuItem(MenuID::AddSite, KTr(wxID_ADD)));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::PlusSmall));
		}
		menu.AddSeparator();
		{
			kxf::UI::MenuItem* item = menu.Add(new kxf::UI::MenuItem(wxID_REMOVE, KTr(wxID_REMOVE)));
			item->Enable(entry != nullptr);
		}
		{
			kxf::UI::MenuItem* item = menu.Add(new kxf::UI::MenuItem(wxID_CLEAR, KTr(wxID_CLEAR)));
			item->Enable(!IsEmpty());
		}
	
		switch (menu.Show(GetView()))
		{
			case MenuID::AddSite:
			{
				OnAddSite();
				break;
			}
			case wxID_REMOVE:
			{
				OnRemoveSite(item);
				break;
			}
			case wxID_CLEAR:
			{
				OnClearList();
				break;
			}
		};
	};
	
	void SitesModel::OnAddSite()
	{
		GetDataVector()->emplace_back(Utility::LabeledValue(wxEmptyString));
	
		KxDataViewItem item = GetItem(GetItemCount() - 1);
		NotifyAddedItem(item);
		SelectItem(item);
		GetView()->EditItem(item, GetView()->GetColumn(ColumnID::Name));
	}
	void SitesModel::OnRemoveSite(const KxDataViewItem& item)
	{
		RemoveItemAndNotify(*GetDataVector(), item);
	}
	void SitesModel::OnClearList()
	{
		ClearItemsAndNotify(*GetDataVector());
	}
	
	void SitesModel::SetDataVector()
	{
		m_InfoData = nullptr;
		VectorModel::SetDataVector();
	}
	void SitesModel::SetDataVector(VectorType& data, PackageProject::InfoSection* info)
	{
		m_InfoData = info;
		VectorModel::SetDataVector(&data);
	}
}

namespace Kortex::PackageDesigner::PageInfoNS
{
	SitesDialog::SitesDialog(wxWindow* parent, const kxf::String& caption, WorkspaceDocument* controller, bool bUseInlineEditor)
		//:m_WindowOptions("SitesDialog", "Window"), m_ViewOptions("SitesDialog", "View")
	{
		UseInlineEditor(bUseInlineEditor);
	
		if (KxStdDialog::Create(parent, wxID_NONE, caption, wxDefaultPosition, wxDefaultSize, KxBTN_OK))
		{
			SetMainIcon(kxf::StdIcon::None);
			SetWindowResizeSide(wxBOTH);
	
			wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
			m_ViewPane = new KxPanel(GetContentWindow(), wxID_NONE);
			m_ViewPane->SetSizer(sizer);
			PostCreate();
	
			// List2
			IncRef();
			SitesModel::Create(controller, m_ViewPane, sizer);
	
			AdjustWindow(wxDefaultPosition, FromDIP(kxf::Size(700, 400)));
			//KProgramOptionSerializer::LoadDataViewLayout(GetView(), m_ViewOptions);
			//KProgramOptionSerializer::LoadWindowSize(this, m_WindowOptions);
		}
	}
	SitesDialog::~SitesDialog()
	{
		IncRef();
	
		//KProgramOptionSerializer::SaveDataViewLayout(GetView(), m_ViewOptions);
		//KProgramOptionSerializer::SaveWindowSize(this, m_WindowOptions);
	}
}
