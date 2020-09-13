#pragma once
#include <Kortex/Kortex.hpp>
#include "Module/ModuleInfo.h"

namespace Kortex
{
	class SystemApplication;
}

namespace Kortex::Application::Internal
{
	extern const SimpleModuleInfo ApplicationModuleTypeInfo;

	class ApplicationModuleInfo: public IModuleInfo
	{
		public:
			kxf::String GetID() const override;
			kxf::String GetName() const override;
			kxf::Version GetVersion() const override;
			kxf::ResourceID GetImageID() const override;
	};
}

namespace Kortex::Application
{
	class ApplicationModule: public IModule
	{
		friend class SystemApplication;

		private:
			Internal::ApplicationModuleInfo m_ModuleInfo;

		protected:
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node) override;
			virtual void OnInit() override;
			virtual void OnExit() override;

		private:
			ApplicationModule();

		public:
			const IModuleInfo& GetModuleInfo() const override
			{
				return m_ModuleInfo;
			}
			ManagerRefVector GetManagers() override;
	};
}
