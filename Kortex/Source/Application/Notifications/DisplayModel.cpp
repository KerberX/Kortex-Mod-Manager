#include "stdafx.h"
#include "DisplayModel.h"
#include <Kortex/Application.hpp>
#include <Kortex/Notification.hpp>
#include <kxf/UI/Dialogs/TaskDialog.h>
#include <kxf/Drawing/UxTheme.h>

using namespace kxf::UI::DataView;

namespace
{
	enum class ColumnRef
	{
		Icon,
		Value,
		ActionRemove,
	};
}

namespace Kortex::Notifications
{
	wxAny DisplayModel::GetValue(const Node& node, const Column& column) const
	{
		const INotification& notification = GetItem(node);
		switch (column.GetID<ColumnRef>())
		{
			case ColumnRef::ActionRemove:
			{
				return ImageProvider::GetBitmap(ImageResourceID::BellMinus);
			}
			case ColumnRef::Icon:
			{
				return notification.GetBitmap();
			}
			case ColumnRef::Value:
			{
				return FormatText(notification);
			}
		};
		return {};
	}
	ToolTip DisplayModel::GetToolTip(const Node& node, const Column& column) const
	{
		const INotification& notification = GetItem(node);
		switch (column.GetID<ColumnRef>())
		{
			case ColumnRef::ActionRemove:
			{
				return KTr("NotificationCenter.RemoveNotification");
			}
			case ColumnRef::Icon:
			case ColumnRef::Value:
			{
				ToolTip toolTip(notification.GetCaption(), notification.GetMessage());
				toolTip.SetAnchorColumn(*GetView()->GetColumnByID(ColumnRef::Icon));
				toolTip.DisplayOnlyIfClipped(*GetView()->GetColumnByID(ColumnRef::Value));
				return toolTip;
			}
		};
		return VirtualListModel::GetToolTip(node, column);
	}
	bool DisplayModel::GetAttributes(const Node& node, const Column& column, const CellState& cellState, CellAttribute& attributes) const
	{
		const INotification& notification = GetItem(node);
		switch (column.GetID<ColumnRef>())
		{
			case ColumnRef::ActionRemove:
			{
				if (cellState.IsHotTracked() && column.IsHotTracked())
				{
					attributes.Options().AddOption(CellOption::HighlightItem);
					return true;
				}
				break;
			}
		};
		return false;
	}

	void DisplayModel::OnSelectItem(ItemEvent& event)
	{
		if (event.GetNode() && event.GetColumn())
		{
			INotification& notification = GetItem(*event.GetNode());
			switch (event.GetColumn()->GetID<ColumnRef>())
			{
				case ColumnRef::ActionRemove:
				{
					if (INotificationCenter::GetInstance()->RemoveNotification(notification))
					{
						RefreshItems();
						GetView()->UnselectAll();
					}
					break;
				}
			};
		}
	}
	void DisplayModel::OnActivateItem(ItemEvent& event)
	{
		if (event.GetNode() && event.GetColumn())
		{
			const INotification& notification = GetItem(*event.GetNode());
			switch (event.GetColumn()->GetID<ColumnRef>())
			{
				case ColumnRef::Icon:
				case ColumnRef::Value:
				{
					INotificationCenter::GetInstance()->HideNotificationsWindow();
					BroadcastProcessor::Get().CallAfter([&notification]()
					{
						kxf::UI::TaskDialog dialog(&IMainWindow::GetInstance()->GetFrame(), wxID_NONE, notification.GetCaption(), notification.GetMessage());
						dialog.SetMainIcon(notification.GetBitmap());
						dialog.ShowModal();
					});
					break;
				}
			};
		}
	}

	kxf::String DisplayModel::FormatText(const INotification& notification) const
	{
		return kxf::String::Format(wxS("<font color='%1'>%2</font>\r\n%3"),
								   kxf::UxTheme::GetDialogMainInstructionColor(*GetView()).ToString(kxf::C2SFormat::HTML),
								   notification.GetCaption(),
								   notification.GetMessage()
		);
	}

	DisplayModel::DisplayModel()
		:m_Notifications(INotificationCenter::GetInstance()->GetNotifications())
	{
		m_BitmapSize.FromSystemIcon();
	}

	void DisplayModel::CreateView(wxWindow* parent)
	{
		View* view = new View(parent, wxID_NONE, CtrlStyle::NoHeader);
		view->ModWindowStyle(static_cast<CtrlStyle>(kxf::UI::WindowBorder::None), true);
		view->SetEmptyControlLabel(KTr("NotificationCenter.NoNotifications"));
		view->AssignModel(this);

		view->Bind(ItemEvent::EvtItemSelected, &DisplayModel::OnSelectItem, this);
		view->Bind(ItemEvent::EvtItemActivated, &DisplayModel::OnActivateItem, this);
		view->SetUniformRowHeight(m_BitmapSize.GetHeight() * 1.5);

		view->AppendColumn<BitmapRenderer>(wxEmptyString, ColumnRef::Icon, m_BitmapSize.GetWidth() + view->FromDIP(8));
		view->AppendColumn<HTMLRenderer>(wxEmptyString, ColumnRef::Value);
		view->AppendColumn<BitmapRenderer>(wxEmptyString, ColumnRef::ActionRemove);
	}
	void DisplayModel::OnShowWindow()
	{
		GetView()->SetFocus();
	}
	void DisplayModel::RefreshItems()
	{
		SetItemCount(m_Notifications.size());
		GetView()->UnselectAll();
		GetView()->GetColumnByID(ColumnRef::Value)->FitInside();
	}
}
