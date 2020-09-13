#pragma once
#include <Kortex/Kortex.hpp>
#include "VirtualFileSystem/IVFSService.h"
#include "VirtualFileSystem/IVirtualFileSystem.h"
#include "IPC/FSController.h"

namespace Kortex::VirtualFileSystem
{
	class BaseFileSystem: public IVirtualFileSystem
	{
		protected:
			IPC::FSController& m_Controller;
			IPC::FSHandle m_Handle = 0;

		protected:
			void OnEnabled() override;
			void OnDisabled() override;

		public:
			BaseFileSystem(IPC::FileSystemID id);

		public:
			IPC::FSHandle GetHandle() const override;

			bool IsEnabled() const override;
			bool Enable() override;
			void Disable() override;

		public:
			kxf::String GetMountPoint() const;
			void SetMountPoint(const kxf::String& path);

			bool EnableAsyncIO(bool value);
			bool EnableExtendedSecurity(bool value);
			bool EnableImpersonateCallerUser(bool value);

			bool IsProcessCreatedInVFS(uint32_t pid) const;
	};
}
