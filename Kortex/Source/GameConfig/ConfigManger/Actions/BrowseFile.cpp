#include "stdafx.h"
#include "BrowseFile.h"
#include "GameConfig/ConfigManger/Item.h"
#include <Kortex/Application.hpp>
#include <KxFramework/KxFileBrowseDialog.h>

namespace Kortex::GameConfig::Actions
{
	void BrowseFile::Invoke(Item& item, ItemValue& value)
	{
		KxFileBrowseDialog dialog(item.GetInvokingTopLevelWindow(), KxID_NONE, KxFBD_OPEN);

		kxf::String folder = value.As<kxf::String>().BeforeLast(wxS('\\'));
		if (folder.IsEmpty())
		{
			folder = value.As<kxf::String>();
		}
		dialog.SetFolder(folder);

		if (dialog.ShowModal() == KxID_OK)
		{
			value.Assign(dialog.GetResult());
		}
	}
}
