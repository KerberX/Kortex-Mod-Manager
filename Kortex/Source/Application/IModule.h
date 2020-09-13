#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/Module/ModuleInfo.h"
#include "Application/Options/Option.h"

namespace Kortex
{
	class IManager;
	class IApplication;
	class IGameInstance;
}

namespace Kortex
{
	namespace GameInstance
	{
		class InstanceModuleLoader;
	}

	class IModule: public Application::WithOptions<IModule>
	{
		friend class IManager;
		friend class IApplication;
		friend class GameInstance::InstanceModuleLoader;

		public:
			using RefList = std::list<IModule*>;
			using RefVector = std::vector<IModule*>;
			using ManagerRefVector = std::vector<IManager*>;
			
			enum class Disposition
			{
				Local,
				Global,
				ActiveInstance,
			};

		private:
			template<class T> static IManager* GetManagerFrom(T&& arg)
			{
				if constexpr(KxFunctional::is_unique_ptr_v<T>)
				{
					return arg.get();
				}
				else if constexpr(std::is_pointer_v<T>)
				{
					return arg;
				}
				else
				{
					return &arg;
				}
			}

		public:
			static const RefList& GetInstances();

			static void InitModulesWithDisposition(Disposition disposition);
			static void UninitModulesWithDisposition(Disposition disposition);
		
			template<class TFunc>
			static void ForEachModule(TFunc&& func)
			{
				for (IModule* module: IModule::GetInstances())
				{
					func(*module);
				}
			}
			
			template<class TFunc>
			static void ForEachManager(IModule* module, TFunc&& func)
			{
				for (IManager* manager: module->GetManagers())
				{
					func(*manager);
				}
			}
			
			template<class TFunc>
			static void ForEachManager(TFunc&& func)
			{
				for (IModule* module: IModule::GetInstances())
				{
					ForEachManager(module, func);
				}
			}
			
			template<class TFunc>
			static void ForEachModuleAndManager(TFunc&& func)
			{
				for (IModule* module: IModule::GetInstances())
				{
					func(*module, nullptr);
					for (IManager* manager: module->GetManagers())
					{
						func(*module, manager);
					}
				}
			}

			template<class... Args>
			static ManagerRefVector ToManagersList(Args&&... arg)
			{
				ManagerRefVector refList;
				refList.reserve(sizeof...(Args));

				for (IManager* manager: std::initializer_list<IManager*> {GetManagerFrom(arg) ...})
				{
					if (manager)
					{
						refList.push_back(manager);
					}
				}
				return refList;
			}

		private:
			const Disposition m_Disposition;

		protected:
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node) = 0;
			virtual void OnInit() = 0;
			virtual void OnExit() = 0;

			bool IsEnabledInTemplate(const kxf::XMLNode& node) const
			{
				return node.GetAttributeBool("Enabled", true);
			}
			
			template<class T>
			kxf::XMLNode GetManagerNode(const kxf::XMLNode& rootNode) const
			{
				return rootNode.GetFirstChildElement(T::GetManagerTypeInfo().GetID());
			}
			
			template<class T>
			bool IsManagerEnabled(const kxf::XMLNode& rootNode) const
			{
				return IsEnabledInTemplate(GetManagerNode<T>(rootNode));
			}
			
			template<class T, class... Args>
			std::unique_ptr<T> CreateManagerIfEnabled(const kxf::XMLNode& rootNode, Args&&... arg) const
			{
				if (IsManagerEnabled<T>(rootNode))
				{
					return std::make_unique<T>(std::forward<Args>(arg)...);
				}
				return nullptr;
			}
		
		public:
			IModule(Disposition disposition);
			virtual ~IModule();

		public:
			virtual const IModuleInfo& GetModuleInfo() const = 0;
			virtual ManagerRefVector GetManagers() = 0;

			Disposition GetModuleDisposition() const
			{
				return m_Disposition;
			}
	};
}

namespace Kortex
{
	template<class t_Base, const auto& t_TypeInfo>
	class ModuleWithTypeInfo: public t_Base
	{
		public:
			static const IModuleInfo& GetModuleTypeInfo()
			{
				return t_TypeInfo;
			}

		public:
			ModuleWithTypeInfo(IModule::Disposition disposition)
				:IModule(disposition)
			{
			}

		public:
			const IModuleInfo& GetModuleInfo() const override
			{
				return t_TypeInfo;
			}
	};
}
