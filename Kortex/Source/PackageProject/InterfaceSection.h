#pragma once
#include <Kortex/Kortex.hpp>
#include "ProjectSection.h"
#include "Utility/LabeledValue.h"
#include "Utility/WithBitmap.h"
#include <kxf::UI::Framework/KxColor.h>

namespace Kortex::PackageProject
{
	class ImageItem: public Utility::WithBitmap
	{
		public:
			using Vector = std::vector<ImageItem>;

		private:
			kxf::String m_Path;
			kxf::String m_Description;
			bool m_IsVisiable = true;
			bool m_FadeEnabled = false;
			kxf::Size m_Size = wxDefaultSize;
	
		public:
			ImageItem(const kxf::String& path = wxEmptyString, const kxf::String& description = wxEmptyString, bool isVisible = true);
			~ImageItem();
	
		public:
			bool HasPath() const
			{
				return !m_Path.IsEmpty();
			}
			const kxf::String& GetPath() const
			{
				return m_Path;
			}
			void SetPath(const kxf::String& value)
			{
				m_Path = value;
			}
	
			bool HasDescription() const
			{
				return !m_Description.IsEmpty();
			}
			const kxf::String& GetDescriptionRaw() const
			{
				return m_Description;
			}
			kxf::String GetDescription() const
			{
				return HasDescription() ? GetDescriptionRaw() : GetPath().AfterLast('\\');
			}
			void SetDescription(const kxf::String& label)
			{
				m_Description = label;
			}
	
			bool IsVisible() const
			{
				return m_IsVisiable;
			}
			void SetVisible(bool value)
			{
				m_IsVisiable = value;
			}
	
			bool IsFadeEnabled() const
			{
				return m_FadeEnabled;
			}
			void SetFadeEnabled(bool value)
			{
				m_FadeEnabled = value;
			}
	
			kxf::Size GetSize() const
			{
				return m_Size;
			}
			void SetSize(const kxf::Size& tSize)
			{
				m_Size = tSize;
			}
	};
}

namespace Kortex::PackageProject
{
	class TitleConfig
	{
		public:
			static const wxAlignment ms_InvalidAlignment = wxALIGN_INVALID;
			
		private:
			wxAlignment m_Alignment = ms_InvalidAlignment;
			KxColor m_Color = wxNullColour;
	
		public:
			TitleConfig(wxAlignment alignment = ms_InvalidAlignment, const KxColor& color = wxNullColour)
				:m_Alignment(alignment), m_Color(color)
			{
			}
			
		public:
			bool IsOK() const
			{
				return HasAlignment() && HasColor();
			}
			
			bool HasAlignment() const
			{
				return m_Alignment != ms_InvalidAlignment;
			}
			wxAlignment GetAlignment() const
			{
				return HasAlignment() ? m_Alignment : wxALIGN_NOT;
			}
			void SetAlignment(wxAlignment value)
			{
				m_Alignment = value;
			}
			
			bool HasColor() const
			{
				return m_Color.IsOk();
			}
			KxColor GetColor() const
			{
				return m_Color;
			}
			void SetColor(const KxColor& value)
			{
				m_Color = value;
			}
	};
}

namespace Kortex::PackageProject
{
	class InterfaceSection: public ProjectSection
	{
		private:
			kxf::String m_MainImage;
			kxf::String m_HeaderImage;
			ImageItem::Vector m_Images;
			TitleConfig m_TitleConfig;
			
		public:
			InterfaceSection(ModPackageProject& project);
			~InterfaceSection();
	
		public:
			const ImageItem::Vector& GetImages() const
			{
				return m_Images;
			}
			ImageItem::Vector& GetImages()
			{
				return m_Images;
			}
			
			const kxf::String& GetMainImage() const
			{
				return m_MainImage;
			}
			void SetMainImage(const kxf::String& path)
			{
				m_MainImage = path;
			}
	
			const kxf::String& GetHeaderImage() const
			{
				return m_HeaderImage;
			}
			void SetHeaderImage(const kxf::String& path)
			{
				m_HeaderImage = path;
			}
	
			const ImageItem* FindImageByPath(const kxf::String& path) const;
			ImageItem* FindImageByPath(const kxf::String& path);
			
			const ImageItem* GetMainItem() const
			{
				return FindImageByPath(m_MainImage);
			}
			ImageItem* GetMainItem()
			{
				return FindImageByPath(m_MainImage);
			}
	
			const ImageItem* GetHeaderItem() const
			{
				return FindImageByPath(m_HeaderImage);
			}
			ImageItem* GetHeaderItem()
			{
				return FindImageByPath(m_HeaderImage);
			}
	
			const TitleConfig& GetTitleConfig() const
			{
				return m_TitleConfig;
			}
			TitleConfig& GetTitleConfig()
			{
				return m_TitleConfig;
			}
	};
}
