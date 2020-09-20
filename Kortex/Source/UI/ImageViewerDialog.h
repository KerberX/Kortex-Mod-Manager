#pragma once;
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxPanel.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/KxStdDialog.h>
#include <kxf::UI::Framework/KxSlider.h>
#include <kxf::UI::Framework/KxImageView.h>
#include <kxf::UI::Framework/KxHTMLWindow.h>
#include <kxf::UI::Framework/kxf::UI::SplitterWindow.h>
#include <Kx/EventSystem/Event.h>
#include "Archive/GenericArchive.h"

namespace Kortex::UI
{
	class ImageViewerEvent: public wxNotifyEvent
	{
		public:
			enum Type
			{
				FilePath,
				BitmapPtr,
				InputStream,
			};
			
		public:
			KxEVENT_MEMBER(ImageViewerEvent, Next);
			KxEVENT_MEMBER(ImageViewerEvent, Previous);

		private:
			std::variant<kxf::String, wxBitmap, wxInputStream*> m_Data;
			bool m_HasPrev = false;
			bool m_HasNext = false;
			
		private:
			kxf::String GetString() const = delete;
			void SetString(const kxf::String& s) = delete;
			
		public:
			ImageViewerEvent() = default;
			ImageViewerEvent(wxEventType type, const wxBitmap& bitmap)
				:wxNotifyEvent(type)
			{
				SetBitmap(bitmap);
			}
			ImageViewerEvent(wxEventType type, const kxf::String& filePath)
				:wxNotifyEvent(type)
			{
				SetFilePath(filePath);
			}
			ImageViewerEvent(wxEventType type, wxInputStream& buffer)
				:wxNotifyEvent(type)
			{
				SetInputStream(buffer);
			}

		public:
			ImageViewerEvent* Clone() const override
			{
				return new ImageViewerEvent(*this);
			}
			Type GetType() const
			{
				return (Type)m_Data.index();
			}
			
			bool HasPrev() const
			{
				return m_HasPrev;
			}
			bool HasNext() const
			{
				return m_HasNext;
			}
			void SetHasPrevNext(bool bPrev, bool bNext)
			{
				m_HasPrev = bPrev;
				m_HasNext = bNext;
			}
			
			kxf::String GetDescription() const
			{
				return wxNotifyEvent::GetString();
			}
			void SetDescription(const kxf::String& s)
			{
				wxNotifyEvent::SetString(s);
			}
			
			bool HasBitmap() const;
			wxBitmap GetBitmap() const;
			void SetBitmap(const wxBitmap& bitmap);
	
			bool IsAnimationFile() const;
			bool HasFilePath() const;
			kxf::String GetFilePath() const;
			void SetFilePath(const kxf::String& filePath);
			
			bool IsInputStream() const;
			wxInputStream* GetInputSteram();
			void SetInputStream(wxInputStream& stream);
	};
}

namespace Kortex::UI
{
	class ImageViewerDialog: public KxStdDialog
	{
		private:
			kxf::UI::SplitterWindow* m_Splitter = nullptr;
			KxImageView* m_ImageView = nullptr;
			KxHTMLWindow* m_Description = nullptr;
			kxf::UI::AuiToolBar* m_ToolBar = nullptr;
			kxf::UI::AuiToolBarItem* m_Backward = nullptr;
			kxf::UI::AuiToolBarItem* m_Forward = nullptr;
			KxSlider* m_ScaleSlider = nullptr;
			kxf::String m_FilePath;
	
			wxColourPickerCtrl* m_ColorBGCtrl = nullptr;
			wxColourPickerCtrl* m_ColorFGCtrl = nullptr;
	
		private:
			int GetViewSizerProportion() const override
			{
				return 1;
			}
			wxOrientation GetViewSizerOrientation() const override
			{
				return wxVERTICAL;
			}
			wxOrientation GetViewLabelSizerOrientation() const override
			{
				return wxHORIZONTAL;
			}
			bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* id = nullptr) const override
			{
				return true;
			}
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_Splitter;
			}
			void ResetState()
			{
				m_ImageView->SetBitmap(wxNullBitmap);
			}
			bool OnDynamicBind(wxDynamicEventTableEntry& entry) override;
	
			void OnLoadFromDisk(const kxf::String& filePath);
			void OnNavigation(wxAuiToolBarEvent& event);
			void OnAcceptNavigation(ImageViewerEvent& event);
			void OnScaleChanged(wxCommandEvent& event);
			void OnSaveImage(wxCommandEvent& event);
			void OnChangeColor(wxColourPickerEvent& event);
	
		public:
			ImageViewerDialog(wxWindow* parent, const kxf::String& caption = wxEmptyString);
			~ImageViewerDialog();
	
		public:
			bool Create(wxWindow* parent, const kxf::String& caption = wxEmptyString);
			void Navigate(ImageViewerEvent& event)
			{
				OnAcceptNavigation(event);
			}
	};
}
