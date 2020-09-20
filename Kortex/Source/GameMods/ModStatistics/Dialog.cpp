#include "stdafx.h"
#include "Dialog.h"
#include <kxf::UI::Framework/KxProgressDialog.h>

namespace Kortex::ModStatistics
{
	Dialog::Dialog(wxWindow* parent)
	{
		if (KxStdDialog::Create(parent, wxID_NONE, KTr("ModManager.Statistics"), wxDefaultPosition, wxDefaultSize, KxBTN_OK))
		{
			KxProgressDialog dialog(parent, wxID_NONE, KTr("ModManager.Statistics.Status"), wxDefaultPosition, wxDefaultSize, KxBTN_NONE);
			dialog.Pulse();
			dialog.Show();

			SetMainIcon(kxf::StdIcon::None);
			SetWindowResizeSide(wxBOTH);

			wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
			m_ViewPane = new KxPanel(GetContentWindow(), wxID_NONE);
			m_ViewPane->SetSizer(sizer);
			PostCreate();

			// List
			DisplayModel::Create(m_ViewPane, sizer);
			RefreshItems();
			dialog.Hide();

			AdjustWindow(wxDefaultPosition, FromDIP(kxf::Size(500, 350)));
			GetView()->SetFocus();
		}
	}
	Dialog::~Dialog()
	{
		IncRef();
	}
}
