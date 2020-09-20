#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IModule.h"
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex
{
	class IPackageManager;
}

namespace Kortex
{
	namespace Internal
	{
		extern const SimpleModuleInfo ModPackagesModuleTypeInfo;
	};

	class ModPackagesModule:
		public ModuleWithTypeInfo<IModule, Internal::ModPackagesModuleTypeInfo>,
		public kxf::SingletonPtr<ModPackagesModule>
	{
		private:
			std::unique_ptr<IPackageManager> m_PackageManager;

		protected:
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node) override;
			void OnInit() override;
			void OnExit() override;

		public:
			ModPackagesModule();

		public:
			virtual ManagerRefVector GetManagers() override
			{
				return ToManagersList(m_PackageManager);
			}
	};
}