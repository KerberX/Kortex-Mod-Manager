#include "pch.hpp"
#include "BookWorkspaceContainer.h"
#include "Log.h"
#include <kxf/UI/Controls/Simplebook.h>
#include <kxf/UI/Controls/AUI/AuiNotebook.h>

namespace Kortex::Application
{
	bool BookWorkspaceContainer::DoInsertWorkspacePage(IWorkspace& workspace, size_t index)
	{
		wxBookCtrlBase& bookCtrl = GetBookCtrl();
		if (bookCtrl.InsertPage(index, &workspace.GetWindow(), workspace.GetName(), false))
		{
			if (auto iconID = workspace.GetIcon().QueryInt())
			{
				bookCtrl.SetPageImage(index, *iconID);
			}
			return true;
		}
		return false;
	}

	bool BookWorkspaceContainer::DoSwitchWorkspace(IWorkspace* fromWorkspace, IWorkspace& toWorkspace)
	{
		Log::Info("%1: switching from %2 to %3", __FUNCTION__, fromWorkspace ? fromWorkspace->GetID() : "null", toWorkspace.GetID());

		if (fromWorkspace == &toWorkspace)
		{
			Log::Info("Can't switch workspace to itself");
			return false;
		}
		if (fromWorkspace && (!fromWorkspace->GetCurrentContainer() || !toWorkspace.GetCurrentContainer()))
		{
			Log::Info("Invalid workspace");
			return false;
		}

		// Close current workspace
		if (fromWorkspace && !CallOnClose(*fromWorkspace))
		{
			Log::Info("%1: %2 refused to close", __FUNCTION__, fromWorkspace->GetID());
			return false;
		}

		// Create next workspace if needed
		if (!toWorkspace.IsCreated() && !CallOnCreate(toWorkspace))
		{
			Log::Info("%1: unable to create %2 workspace", __FUNCTION__, toWorkspace.GetID());
			return false;
		}

		// Open next workspace
		if (CallOnOpen(toWorkspace))
		{
			Log::Info("%1: %2 opened. Process switching", __FUNCTION__, toWorkspace.GetID());
			return true;
		}
		else
		{
			Log::Info("%1: %2 refused to open", __FUNCTION__, toWorkspace.GetID());
		}

		return false;
	}
	void BookWorkspaceContainer::ShowWorkspace(IWorkspace& workspace)
	{
		Log::Info("%1: displaying workspace \"%2\"", __FUNCTION__, workspace.GetID());

		if (auto index = GetWorkspaceIndex(workspace))
		{
			GetBookCtrl().ChangeSelection(*index);
			m_HasCurrentWorkspace = true;
		}
	}
	void BookWorkspaceContainer::HideWorkspace(IWorkspace& workspace)
	{
		Log::Info("%1: hiding workspace \"%2\"", __FUNCTION__, workspace.GetID());

		// Nothing to do
	}

	IWorkspace* BookWorkspaceContainer::GetWorkspaceByID(const kxf::String& id) const
	{
		Log::Info("Attempt to convert workspace ID (%1) to workspace instance", id);

		const wxBookCtrlBase& bookCtrl = GetBookCtrl();
		for (size_t i = 0; i < bookCtrl.GetPageCount(); i++)
		{
			IWorkspace* workspace = GetWorkspaceByIndex(i);
			if (workspace && workspace->GetID() == id)
			{
				return workspace;
			}
		}
		return nullptr;
	}
	IWorkspace* BookWorkspaceContainer::GetWorkspaceByIndex(size_t index) const
	{
		if (wxWindow* window = GetBookCtrl().GetPage(index))
		{
			return IWorkspace::FromWindow(*window);
		}
		return nullptr;
	}
	IWorkspace* BookWorkspaceContainer::GetCurrentWorkspace() const
	{
		Log::Info("Attempt to get current workspace");

		wxWindow* window = GetBookCtrl().GetCurrentPage();
		if (m_HasCurrentWorkspace && window)
		{
			return IWorkspace::FromWindow(*window);
		}
		return nullptr;
	}
	
	size_t BookWorkspaceContainer::GetWorkspaceCount() const
	{
		return GetBookCtrl().GetPageCount();
	}
	size_t BookWorkspaceContainer::EnumWorkspaces(std::function<bool(IWorkspace&)> func)
	{
		size_t count = 0;
		const wxBookCtrlBase& bookCtrl = GetBookCtrl();

		for (size_t i = 0; i < bookCtrl.GetPageCount(); i++)
		{
			if (wxWindow* window = bookCtrl.GetPage(i))
			{
				if (IWorkspace* workspace = IWorkspace::FromWindow(*window))
				{
					count++;
					if (!std::invoke(func, *workspace))
					{
						break;
					}
				}
			}
		}
		return count;
	}
	std::optional<size_t> BookWorkspaceContainer::GetWorkspaceIndex(const IWorkspace& workspace) const
	{
		const wxBookCtrlBase& bookCtrl = GetBookCtrl();

		int index = wxNOT_FOUND;
		if (bookCtrl.IsKindOf(wxCLASSINFO(wxAuiNotebook)))
		{
			// wxAuiNotebook implements pages sifferentely and, for some weird reason, 'wxBookCtrlBase::FindPage' isn't virtual.
			index = static_cast<const wxAuiNotebook&>(bookCtrl).GetPageIndex(const_cast<wxWindow*>(&workspace.GetWindow()));
		}
		else
		{
			index = bookCtrl.FindPage(&workspace.GetWindow());
		}

		if (index != wxNOT_FOUND)
		{
			return index;
		}
		return {};
	}
	bool BookWorkspaceContainer::ChangeWorkspaceIndex(IWorkspace& workspace, size_t newIndex)
	{
		if (auto currentIndex = GetWorkspaceIndex(workspace))
		{
			wxBookCtrlBase& bookCtrl = GetBookCtrl();
			if (currentIndex != newIndex && newIndex < bookCtrl.GetPageCount())
			{
				return bookCtrl.RemovePage(*currentIndex) && DoInsertWorkspacePage(workspace, newIndex);
			}
		}
		return false;
	}

	bool BookWorkspaceContainer::AddWorkspace(IWorkspace& workspace)
	{
		if (IWorkspaceContainer::AddWorkspace(workspace))
		{
			return DoInsertWorkspacePage(workspace, GetBookCtrl().GetPageCount());
		}
		else
		{
			IWorkspaceContainer::RemoveWorkspace(workspace);
			return false;
		}
	}
	bool BookWorkspaceContainer::RemoveWorkspace(IWorkspace& workspace)
	{
		if (IWorkspaceContainer::RemoveWorkspace(workspace))
		{
			if (auto index = GetWorkspaceIndex(workspace))
			{
				return GetBookCtrl().RemovePage(*index);
			}
		}
		return false;
	}
	bool BookWorkspaceContainer::SwitchWorkspace(IWorkspace& nextWorkspace)
	{
		Log::Info("Attempt to switch workspace to %1", nextWorkspace.GetID());

		return DoSwitchWorkspace(GetCurrentWorkspace(), nextWorkspace);
	}
}