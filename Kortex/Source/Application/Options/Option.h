#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/AppOption.h"

namespace Kortex
{
	class IApplication;
	class IWorkspace;
	class IMainWindow;
	class IModule;
	class IManager;
	class IGameInstance;
	class IGameProfile;
}
namespace Kortex::InstallWizard
{
	class WizardDialog;
}

namespace Kortex::Application
{
	class BasicOption: public AppOption
	{
		protected:
			void Create(Disposition disposition, kxf::XMLDocument& xml, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const IApplication& app, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const IGameInstance& instance, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const IGameProfile& profile, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const IModule& module, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const IManager& manager, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const IWorkspace& workspace, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const IMainWindow& mainWindow, const kxf::String& branch = wxEmptyString);
			void Create(Disposition disposition, kxf::XMLDocument& xml, const InstallWizard::WizardDialog& installWizard, const kxf::String& branch = wxEmptyString);
	};
}

namespace Kortex::Application
{
	class GlobalOption: public BasicOption
	{
		private:
			kxf::XMLDocument& GetXML() const;

		public:
			template<class... Args>
			GlobalOption(Args&&... arg)
			{
				Create(Disposition::Global, GetXML(), std::forward<Args>(arg)...);
			}
	};

	class InstanceOption: public BasicOption
	{
		private:
			IConfigurableGameInstance* GetConfigurableInstance(IGameInstance* instance) const;
			kxf::XMLDocument& GetXML(IConfigurableGameInstance* instance) const;

		public:
			template<class... Args>
			InstanceOption(IGameInstance* instance, Args&&... arg)
			{
				if (IConfigurableGameInstance* configurableInstance = GetConfigurableInstance(instance))
				{
					Create(Disposition::Instance, GetXML(configurableInstance), std::forward<Args>(arg)...);
					AssignInstance(configurableInstance);
				}
			}
	};

	class ActiveInstanceOption: public InstanceOption
	{
		private:
			IGameInstance* GetActiveInstance() const;

		public:
			template<class... Args>
			ActiveInstanceOption(Args&&... arg)
				:InstanceOption(GetActiveInstance(), std::forward<Args>(arg)...)
			{
			}
	};

	class ProfileOption: public BasicOption
	{
		private:
			kxf::XMLDocument& GetXML(IGameProfile& profile) const;

		public:
			template<class... Args>
			ProfileOption(IGameProfile* profile, Args&&... arg)
			{
				Create(Disposition::Profile, GetXML(*profile), std::forward<Args>(arg)...);
				AssignProfile(profile);
			}
	};

	class ActiveProfileOption: public ProfileOption
	{
		private:
			IGameProfile* GetActiveProfile() const;

		public:
			template<class... Args> ActiveProfileOption(Args&&... arg)
				:ProfileOption(GetActiveProfile(), std::forward<Args>(arg)...)
			{
			}
	};
}

namespace Kortex::Application
{
	template<class T>
	class WithOptions
	{
		protected:
			const T& CSelf() const
			{
				return *static_cast<const T*>(this);
			}
			T& NCSelf() const
			{
				return const_cast<T&>(CSelf());
			}

		public:
			template<class... Args>
			GlobalOption GetGlobalOption(Args&&... arg) const
			{
				return GlobalOption(NCSelf(), AppOption::MakeXPath(std::forward<Args>(arg)...));
			}
			
			template<class... Args>
			ActiveInstanceOption GetAInstanceOption(Args&&... arg) const
			{
				return ActiveInstanceOption(NCSelf(), AppOption::MakeXPath(std::forward<Args>(arg)...));
			}
			
			template<class... Args>
			ActiveProfileOption GetAProfileOption(Args&&... arg) const
			{
				return ActiveProfileOption(NCSelf(), AppOption::MakeXPath(std::forward<Args>(arg)...));
			}
	};

	template<class T>
	class WithInstanceOptions: public WithOptions<T>
	{
		public:
			template<class... Args>
			InstanceOption GetInstanceOption(Args&&... arg) const
			{
				return InstanceOption(&NCSelf(), NCSelf(), AppOption::MakeXPath(std::forward<Args>(arg)...));
			}
	};

	template<class T>
	class WithProfileOptions: public WithOptions<T>
	{
		public:
			template<class... Args>
			ProfileOption GetProfileOption(Args&&... arg) const
			{
				return ProfileOption(&NCSelf(), NCSelf(), AppOption::MakeXPath(std::forward<Args>(arg)...));
			}
	};
}

namespace Kortex::Application
{
	template<class T, class... Args>
	GlobalOption GetGlobalOptionOf(Args&&... arg)
	{
		return T::GetInstance()->GetGlobalOption(std::forward<Args>(arg)...);
	}
	
	template<class T, class... Args>
	ActiveInstanceOption GetAInstanceOptionOf(Args&&... arg)
	{
		return T::GetInstance()->GetAInstanceOption(std::forward<Args>(arg)...);
	}
	
	template<class T, class... Args>
	ActiveProfileOption GetAProfileOptionOf(Args&&... arg)
	{
		return T::GetInstance()->GetAProfileOption(std::forward<Args>(arg)...);
	}

	template<class T, class... Args>
	InstanceOption GetInstanceOptionOf(Args&&... arg)
	{
		return T::GetInstance()->GetInstanceOption(std::forward<Args>(arg)...);
	}
	
	template<class T, class... Args>
	ProfileOption GetProfileOptionOf(Args&&... arg)
	{
		return T::GetInstance()->GetProfileOption(std::forward<Args>(arg)...);
	}
}
