#include "stdafx.h"
#include "IWorkspaceDocument.h"
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/KxString.h>

namespace Kortex
{
	kxf::String IWorkspaceDocument::GetSaveConfirmationCaption() const
	{
		const IWorkspace* workspace = QueryInterface<IWorkspace>();
		return kxf::String::Format(wxS("%s - %s"), workspace->GetName(), KxString::ToLower(KTr("Controller.SaveChanges.Caption")));
	}
	kxf::String IWorkspaceDocument::GetSaveConfirmationMessage() const
	{
		return KTr("Controller.SaveChanges.Message");
	}

	KxStandardID IWorkspaceDocument::AskForSave(bool canCancel)
	{
		IWorkspace* workspace = QueryInterface<IWorkspace>();
		if (workspace && HasUnsavedChanges())
		{
			KxTaskDialog dialog(&workspace->GetWindow(), wxID_NONE, GetSaveConfirmationCaption(), GetSaveConfirmationMessage(), canCancel ? KxBTN_CANCEL : KxBTN_NONE);
			dialog.SetMainIcon(KxICON_WARNING);
			dialog.AddButton(wxID_YES, KTr("Controller.SaveChanges.Save"));
			dialog.AddButton(wxID_NO, KTr("Controller.SaveChanges.Discard"));

			wxWindowID ret = dialog.ShowModal();
			if (ret == wxID_YES)
			{
				SaveChanges();
			}
			else if (ret == wxID_NO)
			{
				DiscardChanges();
			}
			else
			{
				return wxID_CANCEL;
			}
		}
		return wxID_OK;
	}
}
