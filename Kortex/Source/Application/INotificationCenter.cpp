#include "stdafx.h"
#include "INotificationCenter.h"
#include <Kortex/Application.hpp>
#include <Kortex/Notification.hpp>
#include <Kortex/NetworkManager.hpp>

namespace Kortex::Notifications::Internal
{
	const SimpleManagerInfo TypeInfo("NotificationCenter", "NotificationCenter.Name");
}

namespace Kortex
{
	void INotificationCenter::OnInit()
	{
	}
	void INotificationCenter::OnExit()
	{
		GetNotifications().clear();
	}
	void INotificationCenter::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode)
	{
	}

	INotificationCenter::INotificationCenter()
		:ManagerWithTypeInfo(&IApplication::GetInstance()->GetModule())
	{
	}

	bool INotificationCenter::HasActivePopups() const
	{
		for (const auto& notification: GetNotifications())
		{
			if (notification->HasPopup())
			{
				return true;
			}
		}
		return false;
	}
	size_t INotificationCenter::GetActivePopupsCount() const
	{
		size_t count = 0;
		for (const auto& notification: GetNotifications())
		{
			if (notification->HasPopup())
			{
				count++;
			}
		}
		return count;
	}

	bool INotificationCenter::RemoveNotification(INotification& notification)
	{
		INotification::Vector& items = GetNotifications();
		auto it = std::find_if(items.begin(), items.end(), [&notification](const auto& value)
		{
			return value.get() == &notification;
		});
		if (it != items.end())
		{
			auto temp = std::move(*it);
			items.erase(it);

			if (items.empty())
			{
				UpdateToolbarButton();
			}
			OnNotificationRemoved(*temp);
			return true;
		}
		return false;
	}
	bool INotificationCenter::ClearNotifications()
	{
		INotification::Vector& items = GetNotifications();
		if (!items.empty())
		{
			items.clear();
			UpdateToolbarButton();
			OnNotificationsCleared();

			return true;
		}
		return false;
	}

	void INotificationCenter::Notify(const kxf::String& caption, const kxf::String& message, kxf::StdIcon iconID)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(caption, message, iconID));
	}
	void INotificationCenter::Notify(const kxf::String& caption, const kxf::String& message, const wxBitmap& bitmap)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(caption, message, bitmap));
	}
	
	void INotificationCenter::Notify(const IModule& module, const kxf::String& message, kxf::StdIcon iconID)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(module.GetModuleInfo().GetName(), message, iconID));
	}
	void INotificationCenter::Notify(const IModule& module, const kxf::String& message, const wxBitmap& bitmap)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(module.GetModuleInfo().GetName(), message, bitmap));
	}

	void INotificationCenter::Notify(const IManager& manager, const kxf::String& message, kxf::StdIcon iconID)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(manager.GetManagerInfo().GetName(), message, iconID));
	}
	void INotificationCenter::Notify(const IManager& manager, const kxf::String& message, const wxBitmap& bitmap)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(manager.GetManagerInfo().GetName(), message, bitmap));
	}

	void INotificationCenter::Notify(const IModNetwork& modNetwork, const kxf::String& message, kxf::StdIcon iconID)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(modNetwork.GetName(), message, iconID));
	}
	void INotificationCenter::Notify(const IModNetwork& modNetwork, const kxf::String& message, const wxBitmap& bitmap)
	{
		GetInstance()->QueueNotification(std::make_unique<SimpleNotification>(modNetwork.GetName(), message, bitmap));
	}
}
