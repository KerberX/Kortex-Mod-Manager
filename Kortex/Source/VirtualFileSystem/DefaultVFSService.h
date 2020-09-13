#pragma once
#include <Kortex/Kortex.hpp>
#include "VirtualFileSystem/IVFSService.h"
#include "VirtualFileSystem/IVirtualFileSystem.h"
#include "IPC/FSController.h"
#include <KxFramework/KxSingleton.h>
#include <KxFramework/KxProcess.h>

namespace Kortex::VirtualFileSystem
{
	class RecievingWindow;

	class DefaultVFSService: public IVFSService
	{
		friend class RecievingWindow;

		private:
			mutable IPC::FSController m_Controller;
			RecievingWindow* m_RecievingWindow = nullptr;

			std::list<IVirtualFileSystem*> m_FileSystems;

		private:
			void OnDestroyRecievingWindow();
			void DestroyRecievingWindow();

			void OnFSControllerTerminated();
			void OnMessage(const IPC::Message& message);
			void RunController();

			IVirtualFileSystem* FindFSByhandle(IPC::FSHandle handle) const;

		protected:
			void RegisterFS(IVirtualFileSystem& vfs) override
			{
				m_FileSystems.push_back(&vfs);
			}
			void UnregisterFS(IVirtualFileSystem& vfs) override
			{
				m_FileSystems.remove(&vfs);
			}

		public:
			DefaultVFSService();
			virtual ~DefaultVFSService();

		public:
			bool IsOK() const override;
			void* GetNativeService() override
			{
				return &m_Controller;
			}

			kxf::String GetServiceName() const override
			{
				return wxS("Kortex FSController");
			}
			bool IsInstalled() const override;
			bool IsStarted() const override;
	
			bool Start()  override;
			bool Stop() override;
			bool Install() override;
			bool Uninstall() override;

			bool IsLogEnabled() const override;
			void EnableLog(bool value = true) override;

		public:
			kxf::String GetLibraryName() const override;
			kxf::String GetLibraryURL() const override;
			kxf::Version GetLibraryVersion() const override;

			bool HasNativeLibrary() const override;
			kxf::String GetNativeLibraryName() const override;
			kxf::String GetNativeLibraryURL() const override;
			kxf::Version GetNativeLibraryVersion() const override;
	};
}
