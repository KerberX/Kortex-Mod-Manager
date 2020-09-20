#pragma once
#include "stdafx.h"
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/kxf::Version.h>

namespace Kortex
{
	class IVirtualFileSystem;

	class IVFSService: public kxf::SingletonPtr<IVFSService>
	{
		friend class IVirtualFileSystem;

		protected:
			virtual void RegisterFS(IVirtualFileSystem& vfs) = 0;
			virtual void UnregisterFS(IVirtualFileSystem& vfs) = 0;

		public:
			virtual bool IsOK() const = 0;
			virtual void* GetNativeService() = 0;
			template<class T> T* GetNativeService()
			{
				return static_cast<T*>(GetNativeService());
			}

			virtual kxf::String GetServiceName() const = 0;
			virtual bool IsInstalled() const = 0;
			virtual bool IsStarted() const = 0;

			virtual bool Start() = 0;
			virtual bool Stop() = 0;
			virtual bool Install() = 0;
			virtual bool Uninstall() = 0;

			virtual bool IsLogEnabled() const = 0;
			virtual void EnableLog(bool value = true) = 0;

		public:
			virtual kxf::String GetLibraryName() const = 0;
			virtual kxf::String GetLibraryURL() const = 0;
			virtual kxf::Version GetLibraryVersion() const = 0;

			virtual bool HasNativeLibrary() const = 0;
			virtual kxf::String GetNativeLibraryName() const = 0;
			virtual kxf::String GetNativeLibraryURL() const = 0;
			virtual kxf::Version GetNativeLibraryVersion() const = 0;
	};
}
