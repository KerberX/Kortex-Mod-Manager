#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxCallAtScopeExit.h>
#include <Kx/Sciter.hpp>
#include "IWebView.h"
class wxHtmlLinkEvent;

namespace Kortex::UI::WebViewBackend
{
	class Sciter: public KxSciter::WindowWrapper<wxControl>, public IWebView
	{
		private:
			wxEvtHandler& m_EvtHandler;
			wxEvtHandler m_SpoilerButtonHandler;

		private:
			void OnLoaded(KxSciter::BehaviorEvent& event);
			void OnHyperlink(KxSciter::BehaviorEvent& event);
			void OnSpoilerButton(KxSciter::BehaviorEvent& event);

			bool SendEvent(wxEventTypeTag<wxWebViewEvent> eventID, const kxf::String& url = {}, const kxf::String& target = {});

		public:
			Sciter(wxWindow* parent, wxEvtHandler& evthandler, long style = 0);

		public:
			wxWindow* GetWindow() override
			{
				return this;
			}

			KxColor GetBackgroundColor() const override
			{
				return WindowWrapper::GetBackgroundColour();
			}
			bool SetBackgroundColor(const KxColor& color) override
			{
				return WindowWrapper::SetBackgroundColour(color);
			}

			KxColor GetForegroundColor() const override
			{
				return WindowWrapper::GetForegroundColour();
			}
			bool SetForegroundColor(const KxColor& color) override
			{
				return WindowWrapper::SetForegroundColour(color);
			}

		public:
			void Unload() override
			{
				WindowWrapper::ClearDocument();
			}
			bool LoadText(const kxf::String& text) override;
			bool LoadHTML(const kxf::String& html) override;
			bool LoadURL(const kxf::String& url) override;
	};
}
