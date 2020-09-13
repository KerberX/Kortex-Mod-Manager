#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IModule.h"
#include <KxFramework/KxSingleton.h>

namespace Kortex
{
	class IProgramManager;
}

namespace Kortex
{
	namespace Internal
	{
		extern const SimpleModuleInfo ProgramModuleTypeInfo;
	};

	class KProgramModule: public ModuleWithTypeInfo<IModule, Internal::ProgramModuleTypeInfo>, public KxSingletonPtr<KProgramModule>
	{
		private:
			std::unique_ptr<IProgramManager> m_ProgramManager;

		protected:
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node) override;
			virtual void OnInit() override;
			virtual void OnExit() override;

		public:
			KProgramModule();

		public:
			virtual ManagerRefVector GetManagers() override;
	};
}
