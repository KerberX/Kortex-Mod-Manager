#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/AppOption.h"
#include <KxFramework/KxSingleton.h>
class kxf::XMLDocument;

namespace Kortex
{
	class IGameMod;
	class IGamePlugin;
	class IGameInstance;
	class AppOption;
	class IVariableTable;

	namespace GameInstance
	{
		namespace FolderName
		{
			constexpr static const auto Overwrites = wxS("Overwrites");
			constexpr static const auto Saves = wxS("Saves");
			constexpr static const auto Config = wxS("Config");
		}
	}
}

namespace Kortex::GameInstance
{
	class ProfileMod
	{
		public:
			using Vector = std::vector<ProfileMod>;

		public:
			kxf::String m_Signature;
			intptr_t m_Priority = -1;
			bool m_IsActive = false;

		public:
			ProfileMod(const IGameMod& mod, bool active);
			ProfileMod(const kxf::String& signature, bool active, intptr_t priority = -1);

		public:
			bool IsOK() const
			{
				return !m_Signature.IsEmpty();
			}

			bool IsActive() const
			{
				return m_IsActive;
			}
			intptr_t GetPriority() const
			{
				return m_Priority;
			}

			const kxf::String& GetSignature() const
			{
				return m_Signature;
			}
			IGameMod* GetMod() const;
	};
}

namespace Kortex::GameInstance
{
	class ProfilePlugin
	{
		public:
			using Vector = std::vector<ProfilePlugin>;

		public:
			kxf::String m_Name;
			intptr_t m_Priority = -1;
			bool m_IsActive = false;

		public:
			ProfilePlugin(const IGamePlugin& plugin, bool active);
			ProfilePlugin(const kxf::String& name, bool active, intptr_t priority = -1);

		public:
			bool IsOK() const
			{
				return !m_Name.IsEmpty();
			}
			
			bool IsActive() const
			{
				return m_IsActive;
			}
			intptr_t GetPriority() const
			{
				return m_Priority;
			}

			const kxf::String& GetName() const
			{
				return m_Name;
			}
			IGamePlugin* GetPlugin() const;
	};
}

namespace Kortex
{
	class IGameProfile:
		public Application::IWithConfig,
		public Application::WithProfileOptions<IGameProfile>
	{
		friend class IGameInstance;
		friend class AppOption;

		public:
			using ProfileMod = GameInstance::ProfileMod;
			using ProfilePlugin = GameInstance::ProfilePlugin;

			using Vector = std::vector<std::unique_ptr<IGameProfile>>;
			using RefVector = std::vector<IGameProfile*>;
			using CRefVector = std::vector<const IGameProfile*>;

		public:
			static IGameProfile* GetActive();
			static kxf::String ProcessID(const kxf::String& id);
			static void UpdateVariablesUsingActive(IVariableTable& variables);

		protected:
			static bool CreateLocalFolder(const kxf::String& id, const kxf::String& name);

		public:
			virtual ~IGameProfile() = default;

		public:
			bool IsActive() const
			{
				return GetActive() == this;
			}

			kxf::String GetConfigFile() const;
			kxf::String GetProfileDir() const;
			kxf::String GetProfileRelativePath(const kxf::String& name) const;
			kxf::String GetSavesDir() const;
			kxf::String GetConfigDir() const;
			kxf::String GetOverwritesDir() const;

			virtual std::unique_ptr<IGameProfile> Clone() const = 0;
			virtual void LoadConfig() = 0;
			virtual void SyncWithCurrentState() = 0;

			virtual kxf::String GetID() const = 0;
			virtual void SetID(const kxf::String& id) = 0;

			virtual bool IsLocalSavesEnabled() const = 0;
			virtual void SetLocalSavesEnabled(bool value) = 0;

			virtual bool IsLocalConfigEnabled() const = 0;
			virtual void SetLocalConfigEnabled(bool value) = 0;

			virtual const ProfileMod::Vector& GetMods() const = 0;
			virtual ProfileMod::Vector& GetMods() = 0;

			virtual const ProfilePlugin::Vector& GetPlugins() const = 0;
			virtual ProfilePlugin::Vector& GetPlugins() = 0;
	};
}
