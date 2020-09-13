#include "stdafx.h"
#include "IImageProvider.h"
#include "Application/IApplication.h"

namespace Kortex
{
	wxBitmap IImageProvider::GetBitmap(const kxf::ResourceID& resID) const
	{
		if (resID.IsOK())
		{
			if (auto value = resID.TryAsInt())
			{
				return GetImageList().GetBitmap(*value);
			}
			else if (auto value = resID.TryAsString())
			{
				return GetImageSet().GetBitmap(*value);
			}
		}
		return {};
	}
	wxImage IImageProvider::GetImage(const kxf::ResourceID& resID) const
	{
		if (resID.IsOK())
		{
			if (auto value = resID.TryAsInt())
			{
				return GetImageList().GetImage(*value);
			}
			else if (auto value = resID.TryAsString())
			{
				return GetImageSet().GetImage(*value);
			}
		}
		return {};
	}
	wxIcon IImageProvider::GetIcon(const kxf::ResourceID& resID) const
	{
		if (resID.IsOK())
		{
			if (auto value = resID.TryAsInt())
			{
				return GetImageList().GetIcon(*value);
			}
			else if (auto value = resID.TryAsString())
			{
				return GetImageSet().GetIcon(*value);
			}
		}
		return {};
	}
}

namespace Kortex::ImageProvider
{
	const kxf::ImageList& GetImageList()
	{
		return IApplication::GetInstance()->GetImageProvider().GetImageList();
	}
	const kxf::ImageSet& GetImageSet()
	{
		return IApplication::GetInstance()->GetImageProvider().GetImageSet();
	}

	wxBitmap GetBitmap(const kxf::ResourceID& resID)
	{
		return IApplication::GetInstance()->GetImageProvider().GetBitmap(resID);
	}
	wxImage GetImage(const kxf::ResourceID& resID)
	{
		return IApplication::GetInstance()->GetImageProvider().GetImage(resID);
	}
	wxIcon GetIcon(const kxf::ResourceID& resID)
	{
		return IApplication::GetInstance()->GetImageProvider().GetIcon(resID);
	}
}
