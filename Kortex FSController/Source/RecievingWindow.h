#pragma once
#include "stdafx.h"
#include "IPC/ProcessingWindow.h"
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex::VirtualFileSystem
{
	class FSControllerService;
}

namespace Kortex::FSController
{
	class RecievingWindow: public IPC::ProcessingWindow, public kxf::SingletonPtr<RecievingWindow>
	{
		private:
			VirtualFileSystem::FSControllerService& m_Service;

		protected:
			void OnMessage(const IPC::Message& message) override;

		public:
			RecievingWindow(VirtualFileSystem::FSControllerService& service, wxWindow* parent = nullptr);
			virtual ~RecievingWindow();
	};
}
