#pragma once
#include "stdafx.h"
#include "Common.h"
#include "IPC/Message.h"
#include "VirtualFileSystem/IVirtualFileSystem.h"
#include "VirtualFileSystem/IVFSService.h"
#include <KxVirtualFileSystem/FileSystemService.h>
#include <KxVirtualFileSystem/IFileSystem.h>
#include <KxVirtualFileSystem/Logger/FileLogger.h>

namespace Kortex::FSController
{
	class RecievingWindow;
}

namespace Kortex::VirtualFileSystem
{
	class FSControllerService: public IVFSService, public KxVFS::FileSystemService
	{
		friend class FSController::RecievingWindow;

		private:
			FSController::RecievingWindow* m_RecievingWindow = nullptr;
			std::vector<std::unique_ptr<KxVFS::IFileSystem>> m_FileSystems;
			KxVFS::FileLogger m_Logger;

		private:
			IPC::FSHandle CreateFS(IPC::FileSystemID fileSystemID);
			void DestroyFS(KxVFS::IFileSystem& fileSystem);
			void OnMessage(const IPC::Message& message);

		protected:
			void RegisterFS(IVirtualFileSystem& fileSystem) override
			{
			}
			void UnregisterFS(IVirtualFileSystem& fileSystem) override
			{
			}

		public:
			FSControllerService();
			virtual ~FSControllerService();

		public:
			bool IsOK() const override
			{
				return KxVFS::FileSystemService::IsOK();
			}
			void* GetNativeService() override
			{
				return static_cast<KxVFS::FileSystemService*>(this);
			}

			kxf::String GetServiceName() const override
			{
				return Tokxf::String(KxVFS::FileSystemService::GetServiceName());
			}
			bool IsInstalled() const override
			{
				return KxVFS::FileSystemService::IsInstalled();
			}
			bool IsStarted() const override
			{
				return KxVFS::FileSystemService::IsStarted();
			}
	
			bool Start()  override
			{
				return KxVFS::FileSystemService::Start();
			}
			bool Stop() override
			{
				return KxVFS::FileSystemService::Stop();
			}
			bool Install() override;
			bool Uninstall() override;

			bool IsLogEnabled() const override
			{
				return KxVFS::ILogger::IsLogEnabled();
			}
			void EnableLog(bool value = true) override
			{
				KxVFS::ILogger::EnableLog(value);
			}

			void SetRecievingWindow(FSController::RecievingWindow* recievingWindow)
			{
				m_RecievingWindow = recievingWindow;
			}
			
			IPC::FSHandle GetFileSystemHandle(const KxVFS::IFileSystem& fileSystem) const;
			KxVFS::IFileSystem& GetFileSystemByHandle(IPC::FSHandle handle) const;
			template<class T> T& GetFileSystemByHandle(IPC::FSHandle handle) const
			{
				return static_cast<T&>(GetFileSystemByHandle(handle));
			}

		public:
			kxf::String GetLibraryName() const override
			{
				return wxS("KxVirtualFileSystem");
			}
			kxf::String GetLibraryURL() const override
			{
				return wxS("https://github.com/KerberX/KxVirtualFileSystem");
			}
			kxf::Version GetLibraryVersion() const override
			{
				return Tokxf::String(KxVFS::FileSystemService::GetLibraryVersion());
			}

			bool HasNativeLibrary() const override
			{
				return true;
			}
			kxf::String GetNativeLibraryName() const override
			{
				return wxS("Dokany");
			}
			kxf::String GetNativeLibraryURL() const override
			{
				return wxS("https://github.com/dokan-dev/dokany");
			}
			kxf::Version GetNativeLibraryVersion() const override
			{
				return Tokxf::String(KxVFS::FileSystemService::GetDokanyVersion());
			}
			
		public:
			KxVFS::ILogger& GetLogger() override
			{
				return m_Logger;
			}
	};
}
