#include "stdafx.h"
#include "ApplicationModule.h"
#include "SystemApplication.h"
#include "INotificationCenter.h"
#include "Resources/Imagekxf::ResourceID.h"

namespace Kortex::Application::Internal
{
	kxf::String ApplicationModuleInfo::GetID() const
	{
		return SystemApplication::GetInstance()->GetAppName();
	}
	kxf::String ApplicationModuleInfo::GetName() const
	{
		return SystemApplication::GetInstance()->GetAppDisplayName();
	}
	kxf::Version ApplicationModuleInfo::GetVersion() const
	{
		return SystemApplication::GetInstance()->GetAppVersion();
	}
	kxf::ResourceID ApplicationModuleInfo::GetImageID() const
	{
		return Imagekxf::ResourceID::KortexLogoSmall;
	}
}

namespace Kortex::Application
{
	void ApplicationModule::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node)
	{
	}
	void ApplicationModule::OnInit()
	{
	}
	void ApplicationModule::OnExit()
	{
	}

	ApplicationModule::ApplicationModule()
		:IModule(Disposition::Global)
	{
	}

	IModule::ManagerRefVector ApplicationModule::GetManagers()
	{
		return {INotificationCenter::GetInstance()};
	}
}
