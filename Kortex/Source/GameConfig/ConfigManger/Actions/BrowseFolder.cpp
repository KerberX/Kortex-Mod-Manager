#include "stdafx.h"
#include "BrowseFolder.h"
#include "GameConfig/ConfigManger/Item.h"
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxFileBrowseDialog.h>

namespace Kortex::GameConfig::Actions
{
	void BrowseFolder::Invoke(Item& item, ItemValue& value)
	{
		KxFileBrowseDialog dialog(item.GetInvokingTopLevelWindow(), wxID_NONE, KxFBD_OPEN_FOLDER);
		dialog.SetFolder(value.As<kxf::String>());

		if (dialog.ShowModal() == wxID_OK)
		{
			value.Assign(dialog.GetResult());
		}
	}
}
