#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/INotification.h"

namespace Kortex::Notifications
{
	class TrayPopup: public INotificationPopup
	{
		friend class TrayPopupHandler;

		private:
			INotification* m_Notification = nullptr;
			TrayPopupHandler& m_PopupHandler;

		private:
			void DoDestroy();

		public:
			TrayPopup(INotification& notification, TrayPopupHandler& popupHandler);
			~TrayPopup();

		public:
			void Popup() override;
			void Dismiss() override;
			void Destroy() override;
	};
}
