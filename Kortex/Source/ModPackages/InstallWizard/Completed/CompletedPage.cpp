#include "stdafx.h"
#include "CompletedPage.h"
#include "PackageCreator/PageBase.h"
#include <Kortex/InstallWizard.hpp>

namespace Kortex::InstallWizard
{
	void CompletedPage::OnLoadUIOptions(const Application::ActiveInstanceOption& option)
	{
	}
	void CompletedPage::OnSaveUIOptions(Application::ActiveInstanceOption& option) const
	{
	}
	void CompletedPage::OnPackageLoaded()
	{
	}

	bool CompletedPage::OnOpenPage()
	{
		m_Label->SetLabel(KTrf("InstallWizard.InstallationComplete", GetPackage().GetName()));
		return true;
	}

	wxWindow* CompletedPage::Create()
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		m_Panel = new KxPanel(GetPageContainer(), wxID_NONE);
		m_Panel->SetSizer(sizer);

		m_Label = PackageDesigner::PageBase::CreateCaptionLabel(m_Panel, wxEmptyString);
		m_Label->ToggleWindowStyle(KxLABEL_LINE);
		m_Label->SetForegroundColour(m_Panel->GetForegroundColour());

		sizer->AddSpacer(25);
		sizer->Add(m_Label, 0, wxEXPAND|wxLEFT, 50);
		return m_Panel;
	}
}
