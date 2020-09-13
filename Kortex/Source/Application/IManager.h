#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/BroadcastProcessor.h"
#include "Application/Module/ManagerInfo.h"
#include "Application/Options/Option.h"

namespace kxf::UI
{
	class AuiToolBar;
	class AuiToolBarItem;
	class AuiToolBarEvent;
}
namespace Kortex
{
	class IModule;
	class IWorkspace;
	class IMainWindow;
	class IApplication;
	class IGameInstance;
}

namespace Kortex
{
	namespace GameInstance
	{
		class InstanceModuleLoader;
	}

	class IManager: public kxf::RTTI::Interface<IManager>, public Application::WithOptions<IManager>
	{
		KxRTTI_DeclareIID(IManager, {0x80d27f02, 0x98d6, 0x47cb, {0x84, 0xc5, 0x62, 0xd3, 0x7f, 0xa7, 0x91, 0xbf}});

		friend class IModule;
		friend class IMainWindow;
		friend class IApplication;
		friend class GameInstance::InstanceModuleLoader;

		public:
			using RefList = std::list<IManager*>;
			using RefVector = std::vector<IManager*>;

		public:
			template<class... Args>
			static std::vector<IWorkspace*> ToWorkspacesList(Args&&... arg)
			{
				std::vector<IWorkspace*> items;
				auto Filter = [&items](IWorkspace* workspace)
				{
					if (workspace)
					{
						items.push_back(workspace);
					}
					return 0;
				};

				std::initializer_list<int>{Filter(arg) ...};
				return items;
			}

		private:
			IModule* m_Module = nullptr;

		protected:
			virtual void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) = 0;
			virtual void OnInit() = 0;
			virtual void OnExit() = 0;
			virtual void CreateWorkspaces()
			{
			}

			void AssignModule(IModule& module)
			{
				m_Module = &module;
			}

		public:
			IManager(IModule* module)
				:m_Module(module)
			{
			}
			virtual ~IManager() = default;

		public:
			IModule& GetModule()
			{
				return *m_Module;
			}
			const IModule& GetModule() const
			{
				return *m_Module;
			}
			
			virtual const IManagerInfo& GetManagerInfo() const = 0;
			virtual std::vector<IWorkspace*> EnumWorkspaces() const
			{
				return {};
			}
			
			void ScheduleWorkspacesReload();
	};
}

namespace Kortex
{
	template<class t_Base, const auto& t_TypeInfo>
	class ManagerWithTypeInfo: public t_Base
	{
		public:
			static const IManagerInfo& GetManagerTypeInfo()
			{
				return t_TypeInfo;
			}

		protected:
			ManagerWithTypeInfo(IModule* module = nullptr)
				:t_Base(module)
			{
			}

		public:
			const IManagerInfo& GetManagerInfo() const override
			{
				return t_TypeInfo;
			}
	};
}

namespace Kortex::Application
{
	class ManagerWithToolbarButton
	{
		protected:
			virtual void OnSetToolbarButton(kxf::UI::AuiToolBarItem& button) = 0;
			virtual void OnToolbarButton(kxf::UI::AuiToolBarEvent& event) = 0;

		public:
			virtual ~ManagerWithToolbarButton() = default;

		public:
			virtual void UpdateToolbarButton()
			{
			}
			
			kxf::UI::AuiToolBarItem& AddToolbarButton(kxf::UI::AuiToolBar& toolbar, const kxf::ResourceID& image);
	};
}
