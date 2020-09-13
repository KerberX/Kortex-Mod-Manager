#pragma once
#include <Kortex/Kortex.hpp>
#include "IWebView.h"
#include <wx/webview.h>
#include <KxFramework/KxCOM.h>
struct IWebBrowser2;
struct IHTMLDocument2;

namespace Kortex::UI::WebViewBackend
{
	class InternetExplorer: public IWebView
	{
		private:
			wxEvtHandler& m_EvtHandler;
			wxWebView* m_WebView = nullptr;
			bool m_IsEmpty = true;

		private:
			void DoLoadPage(const kxf::String& html);
			void DoLoadURL(const kxf::String& url);

			void OnNavigating(wxWebViewEvent& event);
			void OnNavigated(wxWebViewEvent& event);
			void OnLoaded(wxWebViewEvent& event);
			void OnError(wxWebViewEvent& event);

			IWebBrowser2& GetWebBrowser() const;
			KxCOMPtr<IHTMLDocument2> GetDocument2() const;
			bool ExecCommand(const kxf::String& command, const wxAny& arg = {});

		public:
			InternetExplorer(wxWindow* parent, wxEvtHandler& evthandler, long style = 0);

		public:
			wxWebView* GetWindow() override
			{
				return m_WebView;
			}

			KxColor GetBackgroundColor() const override
			{
				return m_WebView->GetBackgroundColour();
			}
			bool SetBackgroundColor(const KxColor& color) override
			{
				return m_WebView->SetBackgroundColour(color);
			}

			KxColor GetForegroundColor() const override
			{
				return m_WebView->GetForegroundColour();
			}
			bool SetForegroundColor(const KxColor& color) override
			{
				return m_WebView->SetForegroundColour(color);
			}

		public:
			void Unload() override
			{
				m_WebView->LoadURL(wxWebViewDefaultURLStr);
			}
			bool LoadText(const kxf::String& text) override;
			bool LoadHTML(const kxf::String& html) override
			{
				DoLoadPage(html);
				return true;
			}
			bool LoadURL(const kxf::String& url) override
			{
				DoLoadURL(url);
				return true;
			}
	};
}
