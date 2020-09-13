#pragma once
#include <Kortex/Kortex.hpp>
#include "BaseNotification.h"

namespace Kortex
{
	class SimpleNotification: public Notifications::BaseNotification
	{
		private:
			kxf::String m_Caption;
			kxf::String m_Message;
			wxBitmap m_Bitmap;

		public:
			SimpleNotification(const kxf::String& caption, const kxf::String& message, KxIconType iconID = KxICON_INFORMATION);
			SimpleNotification(const kxf::String& caption, const kxf::String& message, const wxBitmap& bitmap = wxNullBitmap);

		public:
			kxf::String GetCaption() const override
			{
				return m_Caption;
			}
			kxf::String GetMessage() const override
			{
				return m_Message;
			}
			wxBitmap GetBitmap() const override
			{
				return m_Bitmap;
			}
	};
}
