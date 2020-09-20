#include "stdafx.h"
#include "StoreDialog.h"
#include "Application/Resources/IImageProvider.h"
#include "Utility/Common.h"
#include <Kortex/NetworkManager.hpp>
#include <kxf::UI::Framework/KxButton.h>

namespace Kortex::ModSource
{
	void StoreDialog::CreateAddMenu()
	{
		// Add known mod networks
		for (IModNetwork* modNetwork: INetworkManager::GetInstance()->GetModNetworks())
		{
			kxf::UI::MenuItem* item = m_AddButtonMenu.Add(new kxf::UI::MenuItem(modNetwork->GetName()));
			item->SetBitmap(ImageProvider::GetBitmap(modNetwork->GetIcon()));
			item->Enable(!m_ModSourceStore.HasItem(*modNetwork));

			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, modNetwork](kxf::UI::MenuEvent& event)
			{
				OnAddItem(m_ModSourceStore.AssignWith(*modNetwork, ModID()));
			});
		}

		// Add generic
		m_AddButtonMenu.AddSeparator();
		kxf::UI::MenuItem* item = m_AddButtonMenu.Add(new kxf::UI::MenuItem(Utility::MakeBracketedLabel(KTr(wxID_NEW))));
		item->SetBitmap(ImageProvider::GetBitmap(IModNetwork::GetGenericIcon()));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
		{
			OnAddItem(m_ModSourceStore.AssignWith(wxEmptyString, wxEmptyString));
		});
	}
	void StoreDialog::OnAddItem(ModSourceItem& node)
	{
		m_IsModified = true;

		RefreshItems();
		SelectItem(MakeItem(node));
		GetView()->EditItem(MakeItem(node), GetView()->GetColumn(node.HasModNetwork() ? ColumnID::Value : ColumnID::Name));
	}

	void StoreDialog::OnSelectItem(KxDataViewEvent& event)
	{
		const ModSourceItem* node = GetNode(event.GetItem());
		m_RemoveButton->Enable(node != nullptr);
	}
	void StoreDialog::OnAddProvider(wxCommandEvent& event)
	{
		m_AddButtonMenu.ShowAsPopup(m_AddButton);
	}
	void StoreDialog::OnRemoveProvider(wxCommandEvent& event)
	{
		ModSourceItem* node = GetNode(GetView()->GetSelection());
		if (node)
		{
			m_ModSourceStore.RemoveItem(node->GetName());
			m_IsModified = true;
			
			RefreshItems();
			GetView()->SetFocus();
		}
	}

	StoreDialog::StoreDialog(wxWindow* parent, ModSourceStore& store)
		:StoreDisplayModel(store)
	{
		if (KxStdDialog::Create(parent, wxID_NONE, KTr("ModManager.SitesEditor"), wxDefaultPosition, wxDefaultSize, KxBTN_OK))
		{
			SetMainIcon(kxf::StdIcon::None);
			SetWindowResizeSide(wxBOTH);
			Bind(wxEVT_CLOSE_WINDOW, &StoreDialog::OnCloseDialog, this);

			m_RemoveButton = AddButton(wxID_REMOVE, wxEmptyString, true).As<KxButton>();
			m_RemoveButton->Bind(wxEVT_BUTTON, &StoreDialog::OnRemoveProvider, this);
			m_RemoveButton->Disable();

			m_AddButton = AddButton(wxID_ADD, wxEmptyString, true).As<KxButton>();
			m_AddButton->Bind(wxEVT_BUTTON, &StoreDialog::OnAddProvider, this);
			CreateAddMenu();

			wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
			m_ViewPane = new KxPanel(GetContentWindow(), wxID_NONE);
			m_ViewPane->SetSizer(sizer);
			PostCreate();

			// List
			StoreDisplayModel::Create(m_ViewPane, sizer);
			GetView()->Bind(KxEVT_DATAVIEW_ITEM_SELECTED, &StoreDialog::OnSelectItem, this);
			RefreshItems();

			AdjustWindow(wxDefaultPosition, FromDIP(kxf::Size(500, 350)));
			GetView()->SetFocus();
		}
	}
	StoreDialog::~StoreDialog()
	{
		IncRef();
	}

	void StoreDialog::OnCloseDialog(wxCloseEvent& event)
	{
		ApplyChanges();
		event.Skip();
	}
}
