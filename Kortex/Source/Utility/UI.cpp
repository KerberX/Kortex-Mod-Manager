#include "stdafx.h"
#include "UI.h"
#include "Application/Resources/IImageProvider.h"
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/KxShell.h>
#include <kxf::UI::Framework/KxURI.h>

namespace Kortex::Utility::UI
{
	kxf::UI::AuiToolBarItem* CreateToolBarButton(kxf::UI::AuiToolBar* toolBar,
										  const kxf::String& label,
										  const kxf::ResourceID& imageID,
										  wxItemKind kind,
										  int index
	)
	{
		wxBitmap bitmap = ImageProvider::GetBitmap(imageID);
		kxf::UI::AuiToolBarItem* button = toolBar->AddTool(label, bitmap, kind);
		if (!toolBar->HasFlag(wxAUI_TB_TEXT))
		{
			button->SetShortHelp(label);
		}

		return button;
	}

	bool AskOpenURL(const KxURI& uri, wxWindow* parent)
	{
		KxTaskDialog dialog(parent, wxID_NONE, KTr("Generic.OpenWebSiteDialog.Caption"), uri.BuildUnescapedURI(), KxBTN_YES|KxBTN_NO);
		dialog.SetOptionEnabled(KxTD_SIZE_TO_CONTENT);
		if (dialog.ShowModal() == wxID_YES)
		{
			return KxShell::Execute(parent, uri.BuildUnescapedURI());
		}
		return false;
	}
	bool AskOpenURL(const LabeledValue::Vector& urlList, wxWindow* parent)
	{
		KxTaskDialog dialog(parent, wxID_NONE, KTr("Generic.OpenWebSiteListDialog.Caption"), wxEmptyString, KxBTN_CANCEL);
		dialog.SetOptionEnabled(KxTD_SIZE_TO_CONTENT);
		dialog.SetOptionEnabled(KxTD_CMDLINKS_ENABLED);
		dialog.Bind(KxEVT_STDDIALOG_BUTTON, [parent, urlList](wxNotifyEvent& event)
		{
			if (event.GetId() != wxID_CANCEL)
			{
				KxShell::Execute(parent, urlList[event.GetId() - wxID_HIGHEST].GetValue());
			}
			event.Skip();
		});

		for (size_t i = 0; i < urlList.size(); i++)
		{
			const Utility::LabeledValue& url = urlList[i];
			if (url.HasValue() && url.HasLabel())
			{
				dialog.AddButton(wxID_HIGHEST + i, kxf::String::Format("%s\n%s", url.GetLabel(), url.GetValue()));
			}
			else if (url.HasValue() && !url.HasLabel())
			{
				dialog.AddButton(wxID_HIGHEST + i, url.GetValue());
			}
		}
		return dialog.ShowModal() != wxID_CANCEL;
	}

	kxf::String MakeHTMLWindowPlaceholder(const kxf::String& text, const wxWindow* window)
	{
		kxf::String color = window ? window->GetForegroundColour().MakeDisabled().GetAsString() : wxS("gray");
		return kxf::String::Format(wxS("<br><br><font color=\"%s\"><div align=\"center\">%s</div></font>"), color, text);
	}

	bool SetSearchMask(kxf::String& storage, const kxf::String& newMask)
	{
		if (storage != newMask)
		{
			if (!newMask.IsEmpty())
			{
				storage = wxS('*') + KxString::ToLower(newMask) + wxS('*');
			}
			else
			{
				storage.clear();
			}
			return true;
		}
		return false;
	}
	bool CheckSearchMask(const kxf::String& mask, const kxf::String& string)
	{
		return mask.IsEmpty() || (mask.length() == 1 && *mask.begin() == wxS('*')) || KxString::ToLower(string).Matches(mask);
	}
}
