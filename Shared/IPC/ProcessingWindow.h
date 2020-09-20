#pragma once
#include <WinUser.h>
#include "Message.h"
#include <kxf::UI::Framework/kxf::UI::Frame.h>

namespace Kortex::IPC
{
	class ProcessingWindow: public kxf::UI::Frame
	{
		public:
			static kxf::String GetWindowName();

		private:
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		protected:
			virtual void OnMessage(const Message& message) = 0;

		public:
			ProcessingWindow(wxWindow* parent = nullptr);
	};
}
