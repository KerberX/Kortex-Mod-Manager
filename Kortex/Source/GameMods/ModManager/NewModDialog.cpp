#include "stdafx.h"
#include "NewModDialog.h"
#include <Kortex/Application.hpp>
#include <Kortex/ModManager.hpp>
#include <kxf::UI::Framework/KxFileBrowseDialog.h>
#include <kxf::UI::Framework/KxFile.h>

namespace Kortex::ModManager
{
	void NewModDialog::OnOK(wxNotifyEvent& event)
	{
		if (event.GetId() == wxID_OK)
		{
			kxf::String name = GetValue();
			if (!name.IsEmpty())
			{
				const IGameMod* existingMod = IModManager::GetInstance()->FindModByID(name);
				if (existingMod)
				{
					BroadcastProcessor::Get().ProcessEvent(LogEvent::EvtWarning, KTr("ModManager.NewMod.NameCollision"), this);
				}
				else
				{
					m_Name = name;
					event.Skip();
					return;
				}
			}
			else
			{
				BroadcastProcessor::Get().ProcessEvent(LogEvent::EvtWarning, KTr("ModManager.NewMod.NameInvalid"), this);
			}
			event.Veto();
		}
		else
		{
			event.Skip();
		}
	}

	NewModDialog::NewModDialog(wxWindow* parent)
	{
		if (Create(parent, wxID_NONE, KTr("ModManager.NewMod.DialogCaption"), wxDefaultPosition, wxDefaultSize, KxBTN_OK|KxBTN_CANCEL))
		{
			SetLabel(KTr("ModManager.NewMod.DialogMessage"));
			GetDialogMainCtrl()->SetFocus();

			Bind(KxEVT_STDDIALOG_BUTTON, &NewModDialog::OnOK, this);
			Center();
		}
	}
	NewModDialog::~NewModDialog()
	{
	}
}

namespace Kortex::ModManager
{
	void NewModFromFolderDialog::OnSelectFolder(wxNotifyEvent& event)
	{
		if (event.GetId() == wxID_SELECT_FOLDER)
		{
			KxFileBrowseDialog dialog(this, wxID_NONE, KxFBD_OPEN_FOLDER);
			dialog.SetOptionEnabled(KxFBD_FORCE_FILE_SYSTEM, true);
			if (dialog.ShowModal() == wxID_OK)
			{
				m_FolderPath = dialog.GetResult();
				if (GetFolderName().IsEmpty())
				{
					SetValue(KxFile(m_FolderPath).GetFullName());
				}
			}

			UpdateLabelText();
			event.Veto();
		}
		else if (event.GetId() == wxID_OK)
		{
			if (m_FolderPath.IsEmpty())
			{
				BroadcastProcessor::Get().ProcessEvent(LogEvent::EvtWarning, KTr("ModManager.NewMod.SelectedFolderNone"), this);
				event.Veto();
			}
			else
			{
				event.Skip();
			}
		}
		else
		{
			event.Skip();
		}
	}
	void NewModFromFolderDialog::OnChangeMethod(wxCommandEvent& event)
	{
		m_IsLinkedMod = m_AsLinkedModCB->GetValue();
		UpdateLabelText();
	}

	kxf::String NewModFromFolderDialog::GetMethodString(bool bLink) const
	{
		if (bLink)
		{
			return KTr("ModManager.NewMod.FromFolderMessageLink");
		}
		else
		{
			return KTr("ModManager.NewMod.FromFolderMessageCopy");
		}
	}
	void NewModFromFolderDialog::UpdateLabelText()
	{
		if (m_FolderPath.IsEmpty())
		{
			SetLabel(GetMethodString());
		}
		else
		{
			SetLabel(kxf::String::Format("%s\r\n%s: %s", GetMethodString(), KTr("ModManager.NewMod.SelectedFolder"), m_FolderPath));
		}
	}

	NewModFromFolderDialog::NewModFromFolderDialog(wxWindow* parent)
		:NewModDialog(parent)
	{
		SetCaption(KTr("ModManager.NewMod.DialogCaption"));
		SetLabel(GetMethodString(false));
		AddButton(wxID_SELECT_FOLDER, wxEmptyString, true);

		// Method checkbox
		m_AsLinkedModCB = new wxCheckBox(GetContentWindow(), wxID_NONE, KTr("ModManager.NewMod.CreateAsLinkedMod"));
		GetContentWindowSizer()->Add(m_AsLinkedModCB, 1, wxEXPAND|wxTOP, LayoutConstants::VerticalSpacing);

		Bind(KxEVT_STDDIALOG_BUTTON, &NewModFromFolderDialog::OnSelectFolder, this);
		m_AsLinkedModCB->Bind(wxEVT_CHECKBOX, &NewModFromFolderDialog::OnChangeMethod, this);

		AdjustWindow();
		Center();
	}
	NewModFromFolderDialog::~NewModFromFolderDialog()
	{
	}
}
