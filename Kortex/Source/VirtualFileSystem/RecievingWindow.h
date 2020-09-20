#pragma once
#include <Kortex/Kortex.hpp>
#include "IPC/ProcessingWindow.h"
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex::VirtualFileSystem
{
	class DefaultVFSService;

	class RecievingWindow: public IPC::ProcessingWindow, public kxf::SingletonPtr<RecievingWindow>
	{
		private:
			DefaultVFSService& m_Service;

		protected:
			void OnMessage(const IPC::Message& message) override;

		public:
			RecievingWindow(DefaultVFSService& service, wxWindow* parent = nullptr);
			virtual ~RecievingWindow();

		public:
			bool Destroy() override;
	};
}
