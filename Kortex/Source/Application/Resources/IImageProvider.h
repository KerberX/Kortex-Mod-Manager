#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf/General/Singleton.h>
#include <kxf/Drawing/ImageList.h>
#include <kxf/Drawing/ImageSet.h>

namespace Kortex
{
	class IImageProvider
	{
		public:
			virtual const kxf::ImageList& GetImageList() const = 0;
			virtual const kxf::ImageSet& GetImageSet() const = 0;

		public:
			wxBitmap GetBitmap(const kxf::ResourceID& resID) const;
			wxImage GetImage(const kxf::ResourceID& resID) const;
			wxIcon GetIcon(const kxf::ResourceID& resID) const;
	};
}

namespace Kortex::ImageProvider
{
	const kxf::ImageList& GetImageList();
	const kxf::ImageSet& GetImageSet();

	wxBitmap GetBitmap(const kxf::ResourceID& resID);
	wxImage GetImage(const kxf::ResourceID& resID);
	wxIcon GetIcon(const kxf::ResourceID& resID);
}
