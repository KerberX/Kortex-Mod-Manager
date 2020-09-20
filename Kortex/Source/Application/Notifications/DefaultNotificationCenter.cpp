#include "stdafx.h"
#include "DefaultNotificationCenter.h"
#include "DisplayModel.h"
#include "Application/Resources/ImageResourceID.h"
#include "Utility/Log.h"
#include <Kortex/Application.hpp>
#include <Kortex/Notification.hpp>
#include <kxf/System/SystemInformation.h>
#include <kxf/UI/Controls/AUI/AuiToolBar.h>
#include <wx/popupwin.h>

namespace Kortex::Notifications
{
	void DefaultNotificationCenter::UpdateLabel()
	{
		if (m_Notifications.empty())
		{
			m_PpoupToolbar_Label->SetLabel(KTr("NotificationCenter.NameShort"));
		}
		else
		{
			m_PpoupToolbar_Label->SetLabel(kxf::String::Format(wxS("%1 (%2)"), KTr("NotificationCenter.NameShort"), m_Notifications.size()));
		}
		m_PpoupToolbar->UpdateUI();
	}
	void DefaultNotificationCenter::OnClearNotifications(kxf::UI::AuiToolBarEvent& event)
	{
		ClearNotifications();
	}
	void DefaultNotificationCenter::OnNotificationsCountChanged()
	{
		if (IsNotificationsDisplayed())
		{
			m_PopupDisplayModel->RefreshItems();
			UpdateLabel();
		}
	}

	void DefaultNotificationCenter::OnSetToolbarButton(kxf::UI::AuiToolBarItem& button)
	{
		m_Button = &button;
		m_Button->SetShortHelp(KTr("NotificationCenter.Name"));
		UpdateToolbarButton();

		// Popup window
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		m_PopupWindow = new wxPopupTransientWindow(&m_Button->GetToolBar(), wxBORDER_THEME);
		IThemeManager::GetActive().Apply(m_PopupWindow);
		m_PopupWindow->SetInitialSize(m_PopupWindow->FromDIP(kxf::Size(325, 425)));
		m_PopupWindow->SetSizer(sizer);

		// Toolbar
		m_PpoupToolbar = new kxf::UI::AuiToolBar(m_PopupWindow, wxID_NONE);
		m_PpoupToolbar->SetMargins(m_PopupWindow->FromDIP(LayoutConstants::HorizontalSpacing), m_PopupWindow->FromDIP(LayoutConstants::VerticalSpacing));
		m_PpoupToolbar_Label = m_PpoupToolbar->AddLabel(wxEmptyString);
		m_PpoupToolbar->AddStretchSpacer();
		
		{
			m_PpoupToolbar_ClearNotifications = m_PpoupToolbar->AddTool(wxEmptyString, ImageProvider::GetBitmap(ImageResourceID::Broom));
			m_PpoupToolbar_ClearNotifications->SetEnabled(false);
			m_PpoupToolbar_ClearNotifications->SetShortHelp(KTr("NotificationCenter.ClearNotifications"));
			m_PpoupToolbar_ClearNotifications->Bind(kxf::UI::AuiToolBarEvent::EvtItemClick, &DefaultNotificationCenter::OnClearNotifications, this);
		}

		m_PpoupToolbar->Realize();
		m_PpoupToolbar->SetBackgroundColour(m_PopupWindow->GetBackgroundColour());
		sizer->Add(m_PpoupToolbar, 0, wxEXPAND);

		// Notifications list
		m_PopupDisplayModel = new DisplayModel();
		m_PopupDisplayModel->CreateView(m_PopupWindow);
		sizer->Add(m_PopupDisplayModel->GetView(), 1, wxEXPAND|wxTOP, LayoutConstants::VerticalSpacingSmall);
	}
	void DefaultNotificationCenter::OnToolbarButton(kxf::UI::AuiToolBarEvent& event)
	{
		if (IsNotificationsDisplayed())
		{
			HideNotificationsWindow();
		}
		else
		{
			ShowNotificationsWindow();
		}
	}
	void DefaultNotificationCenter::UpdateToolbarButton()
	{
		const bool isEmpty = m_Notifications.empty();

		if (m_Button)
		{
			if (!isEmpty)
			{
				m_Button->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::BellRedCircle));
			}
			else
			{
				m_Button->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Bell));
			}
		}
		if (m_PpoupToolbar_ClearNotifications)
		{
			m_PpoupToolbar_ClearNotifications->SetEnabled(!isEmpty);
		}
	}

	void DefaultNotificationCenter::QueueNotification(std::unique_ptr<INotification> notification)
	{
		Utility::Log::LogInfo("DefaultNotificationCenter::DoNotify");
		Utility::Log::LogInfo("Caption: %1", notification->GetCaption());
		Utility::Log::LogInfo("Message: %1", notification->GetMessage());

		INotification& ref = *notification;
		if (wxCriticalSectionLocker lock(m_NotificationsCS); true)
		{
			m_Notifications.emplace(m_Notifications.begin(), std::move(notification));
		}

		BroadcastProcessor::Get().CallAfter([this, &ref]()
		{
			UpdateToolbarButton();
			OnNotificationAdded(ref);

			QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
			if (SUCCEEDED(::SHQueryUserNotificationState(&state)) && state == QUNS_ACCEPTS_NOTIFICATIONS)
			{
				ref.Popup();
			}
		});
	}

	bool DefaultNotificationCenter::IsNotificationsDisplayed() const
	{
		return m_PopupWindow && m_PopupWindow->IsShown();
	}
	void DefaultNotificationCenter::ShowNotificationsWindow()
	{
		if (m_PopupWindow)
		{
			kxf::Point pos = m_Button->GetToolBar().ClientToScreen(m_Button->GetDropdownMenuPosition());
			const int offset = kxf::System::GetMetric(kxf::SystemSizeMetric::IconSmall).GetWidth();
			const int screenWidth = kxf::System::GetMetric(kxf::SystemSizeMetric::Screen).GetWidth();
			const int rightSide = pos.GetX() + m_PopupWindow->GetSize().GetWidth();
			if (rightSide > screenWidth)
			{
				pos.X() -= (rightSide - screenWidth) + offset;
			}

			m_PopupWindow->Layout();
			m_PopupWindow->SetPosition(pos);
			m_PopupWindow->Popup(wxWindow::FindFocus());

			m_PopupDisplayModel->OnShowWindow();
			OnNotificationsCountChanged();
		}
	}
	void DefaultNotificationCenter::HideNotificationsWindow()
	{
		if (m_PopupWindow)
		{
			m_PopupWindow->Dismiss();
		}
	}
}
