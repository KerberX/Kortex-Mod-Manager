#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxStdDialog.h>
#include <kxf::UI::Framework/KxAuiNotebook.h>
#include <kxf::UI::Framework/KxImageView.h>
#include <kxf::UI::Framework/KxPanel.h>
class KxHTMLWindow;
class wxHtmlLinkEvent;

namespace Kortex::Application
{
	namespace About
	{
		class DisplayModel;
		class AppNode;
	}

	class AboutDialog: public KxStdDialog
	{
		friend class About::DisplayModel;

		private:
			KxImageView* m_Logo = nullptr;
			KxAuiNotebook* m_TabView = nullptr;

			std::unique_ptr<About::AppNode> m_AppInfo;
			About::DisplayModel* m_DisplayModel = nullptr;
			wxWindow* m_TemporaryTab = nullptr;

		private:
			int GetViewSizerProportion() const override
			{
				return 1;
			}
			wxOrientation GetViewSizerOrientation() const override
			{
				return wxHORIZONTAL;
			}
			wxOrientation GetViewLabelSizerOrientation() const override
			{
				return wxVERTICAL;
			}
			bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* id = nullptr) const override
			{
				return true;
			}
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_TabView;
			}

		private:
			kxf::Size GetLogoSize() const;
			kxf::String GetCaption() const override;

			wxWindow* CreateTab_Info();
			wxWindow* CreateTab_Components();
			wxWindow* CreateTab_License();

			KxHTMLWindow* CreateHTMLWindow(wxWindow* parent = nullptr);
			void CreateTemporaryTab(wxWindow* window, const kxf::String& label, const wxBitmap& bitmap = wxNullBitmap);
			void OnTabChanged(wxAuiNotebookEvent& event);
			void OnLinkClicked(wxHtmlLinkEvent& event);

		public:
			AboutDialog(wxWindow* parent);
			virtual ~AboutDialog();
	};
}
