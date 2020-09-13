#pragma once
#include <Kortex/Kortex.hpp>
#include "DefaultGameInstance.h"

namespace Kortex
{
	class IModule;
	class IManager;
	class IGameProfile;
	class IGameInstance;

	class GameDataModule;
	class GameConfigModule;
}

namespace Kortex::GameInstance
{
	class InstanceModuleLoader
	{
		private:
			IGameInstance* m_Instance = nullptr;

		protected:
			void LoadGlobalModule(IModule& module, const kxf::XMLDocument& instanceConfig);
			void LoadModule(IModule& module, const kxf::XMLNode& node);
			
			template<class T, bool isAlwaysEnabled = false>
			std::unique_ptr<T> InitModule(const kxf::XMLDocument& instanceConfig)
			{
				const kxf::XMLNode node = instanceConfig.GetFirstChildElement("Definition").GetFirstChildElement(T::GetModuleTypeInfo().GetID());
				if (isAlwaysEnabled || node.GetAttributeBool("Enabled", true))
				{
					auto module = std::make_unique<T>();
					LoadModule(*module, node);
					return module;
				}
				return nullptr;
			}

		protected:
			InstanceModuleLoader(IGameInstance* instance)
				:m_Instance(instance)
			{
			}
	};

	class ActiveGameInstance: public ConfigurableGameInstance, public InstanceModuleLoader
	{
		private:
			KxFileStream m_DirectoryLock;
			kxf::String m_CurrentProfileID;
		
			std::unique_ptr<GameDataModule> m_GameDataModule;
			std::unique_ptr<GameConfigModule> m_GameConfigModule;

		protected:
			void InitModulesConfig(const kxf::XMLDocument& instanceConfig);
			void InitVariables(const IGameProfile& profile);
		
			bool OnLoadInstance(const kxf::XMLDocument& instanceConfig) override;
			bool ShouldInitProfiles() const override;

		public:
			ActiveGameInstance(const IGameInstance& instanceTemplate, const kxf::String& instanceID);

		public:
			const kxf::String& GetActiveProfileID() const;
			void SetCurrentProfileID(const kxf::String& id);

			const IGameProfile* GetActiveProfile() const
			{
				return GetProfile(m_CurrentProfileID);
			}
			IGameProfile* GetActiveProfile()
			{
				return GetProfile(m_CurrentProfileID);
			}

			void DoChangeProfileTo(IGameProfile& profile);
			void LoadSavedProfileOrDefault() override;
	};
}
