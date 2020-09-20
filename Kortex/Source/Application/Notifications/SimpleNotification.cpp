#include "stdafx.h"
#include "SimpleNotification.h"
#include "Utility/BitmapSize.h"
#include <wx/artprov.h>

namespace Kortex
{
	SimpleNotification::SimpleNotification(const kxf::String& caption, const kxf::String& message, kxf::StdIcon iconID)
		:m_Caption(caption), m_Message(message), m_Bitmap(iconID != kxf::StdIcon::None ? wxArtProvider::GetMessageBoxIcon(kxf::UI::ToWxStdIcon(iconID)) : wxNullBitmap)
	{
	}
	SimpleNotification::SimpleNotification(const kxf::String& caption, const kxf::String& message, const wxBitmap& bitmap)
		: m_Caption(caption), m_Message(message)
	{
		Utility::BitmapSize bitmapSize;
		bitmapSize.FromSystemIcon();

		if (m_Bitmap.GetWidth() != bitmapSize.GetWidth() || m_Bitmap.GetHeight() != bitmapSize.GetHeight())
		{
			m_Bitmap = bitmapSize.ScaleMaintainRatio(bitmap);
		}
		else
		{
			m_Bitmap = bitmap;
		}
	}
}
