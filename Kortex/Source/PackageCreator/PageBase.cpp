#include "stdafx.h"
#include "PageBase.h"
#include "PackageProject/ModPackageProject.h"
#include "Workspace.h"
#include "WorkspaceDocument.h"
#include "ModPackages/IPackageManager.h"
#include "GameInstance/IGameInstance.h"
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxStdDialogSimple.h>
#include <kxf::UI::Framework/KxLabel.h>
#include <kxf::UI::Framework/KxTextBox.h>
#include <kxf::UI::Framework/KxRichToolTip.h>

namespace Kortex::PackageDesigner
{
	bool PageBase::OnCreateWorkspace()
	{
		return true;
	}
	bool PageBase::OnOpenWorkspace()
	{
		return true;
	}
	bool PageBase::OnCloseWorkspace()
	{
		return true;
	}
	void PageBase::OnReloadWorkspace()
	{
	}

	PageBase::PageBase(Workspace& mainWorkspace, WorkspaceDocument& controller)
		:m_MainWorkspace(&mainWorkspace), m_Controller(&controller)
	{
	}

	ModPackageProject* PageBase::GetProject() const
	{
		return m_Controller->GetProject();
	}
	kxf::String PageBase::GetName() const
	{
		return GetPageName();
	}

	KxTextBox* PageBase::CreateInputField(wxWindow* window)
	{
		KxTextBox* textBox = new KxTextBox(window, wxID_NONE);
		textBox->Bind(wxEVT_TEXT, [this](wxCommandEvent& event)
		{
			m_Controller->ChangeNotify();
			event.Skip();
		});
		return textBox;
	}

	KxLabel* PageBase::CreateCaptionLabel(wxWindow* window, const kxf::String& label)
	{
		return new KxLabel(window, wxID_NONE, label, KxLabel::DefaultStyle|KxLABEL_CAPTION|KxLABEL_LINE|KxLABEL_COLORED);
	}
	KxLabel* PageBase::CreateNormalLabel(wxWindow* window, const kxf::String& label, bool addColon, bool addLine)
	{
		int style = (addLine ? KxLABEL_LINE : 0);
		return new KxLabel(window, wxID_NONE, !addColon || label.IsEmpty() ? label : label + ':', style);
	}
	kxf::UI::AuiToolBar* PageBase::CreateListToolBar(wxWindow* window, bool isVertical, bool showText)
	{
		int flags = kxf::UI::AuiToolBar::DefaultStyle|wxAUI_TB_PLAIN_BACKGROUND|(isVertical ? wxAUI_TB_VERTICAL : wxAUI_TB_HORIZONTAL)|(showText ? wxAUI_TB_TEXT : 0);

		kxf::UI::AuiToolBar* toolBar = new kxf::UI::AuiToolBar(window, wxID_NONE, flags);
		toolBar->SetBackgroundColour(window->GetBackgroundColour());
		toolBar->SetToolPacking(0);
		toolBar->SetMargins(0, 0, 0, 0);
		toolBar->SetToolSeparation(0);

		IThemeManager::GetActive().Apply(static_cast<wxWindow*>(toolBar));
		return toolBar;
	}
	void PageBase::ShowTooltipWarning(wxWindow* window, const kxf::String& message, const wxRect& rect)
	{
		KxRichToolTip tooltip(KTr(wxID_ERROR), message);
		tooltip.SetIcon(KxICON_WARNING);

		if (!rect.IsEmpty() || wxRect(kxf::Point(0, 0), wxGetDisplaySize()).Contains(rect))
		{
			tooltip.SetKind(wxTipKind_BottomLeft);
		}
		tooltip.Show(window, rect);
	}
	void PageBase::WarnIDCollision(wxWindow* window, const wxRect& rect)
	{
		ShowTooltipWarning(window, KTr("PackageCreator.IDCollision"), rect);
	}
}
