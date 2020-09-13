#include "stdafx.h"
#include "IManager.h"
#include "IWorkspace.h"
#include "Resources/IImageProvider.h"
#include <kxf/UI/Controls/AUI/AuiToolBar.h>

namespace Kortex
{
	void IManager::ScheduleWorkspacesReload()
	{
		for (IWorkspace* workspace: EnumWorkspaces())
		{
			workspace->ScheduleReload();
		}
	}
}

namespace Kortex::Application
{
	kxf::UI::AuiToolBarItem& ManagerWithToolbarButton::AddToolbarButton(kxf::UI::AuiToolBar& toolbar, const kxf::ResourceID& image)
	{
		using namespace kxf::UI;

		AuiToolBarItem* button = toolbar.AddTool(wxEmptyString, ImageProvider::GetBitmap(image), wxITEM_NORMAL);
		button->Bind(AuiToolBarEvent::EvtItemClick, &ManagerWithToolbarButton::OnToolbarButton, this);
		OnSetToolbarButton(*button);

		return *button;
	}
}
