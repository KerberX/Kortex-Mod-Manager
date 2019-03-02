#include "stdafx.h"
#include "BrowseFile.h"
#include <Kortex/Application.hpp>
#include <KxFramework/KxFileBrowseDialog.h>

namespace Kortex::GameConfig::Actions
{
	void BrowseFile::Invoke(ItemValue& value)
	{
		KxFileBrowseDialog dialog(IApplication::GetInstance()->GetTopWindow(), KxID_NONE, KxFBD_OPEN);
		if (dialog.ShowModal() == KxID_OK)
		{
			wxString folder = value.As<wxString>().BeforeLast(wxS('\\'));
			if (folder.IsEmpty())
			{
				folder = value.As<wxString>();
			}

			dialog.SetFolder(folder);
			value.Assign(dialog.GetResult());
		}
	}
}
