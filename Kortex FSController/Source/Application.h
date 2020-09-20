#pragma once
#include "stdafx.h"
#include <kxf::UI::Framework/KxApp.h>
#include <kxf::UI::Framework/KxProcess.h>

namespace Kortex::VirtualFileSystem
{
	class FSControllerService;
}

namespace Kortex::FSController
{
	class RecievingWindow;
	class MainApplicationLink;

	class Application: public KxApp<wxApp, Application>
	{
		private:
			kxf::String m_RootFolder;
			kxf::String m_DataFolder;
			kxf::String m_LogFolder;

			RecievingWindow* m_RecievingWindow = NULL;
			std::unique_ptr<KxProcess> m_MainProcess;
			std::unique_ptr<MainApplicationLink> m_MainApp;
			std::unique_ptr<VirtualFileSystem::FSControllerService> m_Service;

		private:
			void OnMainAppTerminates(wxProcessEvent& event);

		public:
			Application();
			~Application();

		public:
			bool OnInit() override;
			int OnExit() override;

			bool OnExceptionInMainLoop() override;
			void OnUnhandledException() override;

		public:
			kxf::String GetRootFolder() const
			{
				return m_RootFolder;
			}
			kxf::String GetDataFolder() const
			{
				return m_DataFolder;
			}
			kxf::String GetLogFolder() const
			{
				return m_LogFolder;
			}
			kxf::String GetLibraryPath() const;
	};
}
