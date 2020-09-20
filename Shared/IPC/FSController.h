#pragma once
#include <WinUser.h>
#include "MessageExchanger.h"
#include <kxf::UI::Framework/KxProcess.h>
#include <condition_variable>

namespace Kortex::IPC
{
	class ProcessingWindow;

	class FSController: public MessageExchanger
	{
		private:
			KxProcess m_Process;
			bool m_IsProcessIdle = false;
			kxf::String m_Library;

			HANDLE m_ProcessHandle = nullptr;
			std::mutex m_ThreadMutex;
			std::condition_variable m_ThreadCondition;
			bool m_IsDestroyed = false;

		private:
			void OnProcessReady(wxProcessEvent& event);
			void EndWaitForTermination();
			bool Reset();

		protected:
			void OnSendMessage(const Message& message, const void* userData, size_t dataSize) override;

		public:
			FSController(const kxf::String& executablePath);
			virtual ~FSController();

		public:
			bool IsRunning() const;
			bool IsProcessReady() const
			{
				return m_IsProcessIdle;
			}

			const KxProcess& GetProcess() const
			{
				return m_Process;
			}
			KxProcess& GetProcess()
			{
				return m_Process;
			}

			void SetLibrary(const kxf::String& library)
			{
				m_Library = library;
			}
			void SetProcessingWindow(const ProcessingWindow& processingWindow);
			bool WaitForTermination(std::function<void()> func = {});
			void Run();
	};
}
