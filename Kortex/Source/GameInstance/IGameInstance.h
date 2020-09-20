#pragma once
#include <Kortex/Kortex.hpp>
#include "GameID.h"
#include "Application/AppOption.h"
#include "Application/Options/Option.h"
#include <kxf::UI::Framework/KxINI.h>
#include <kxf::UI::Framework/KxFileStream.h>
#include <Kx/RTTI.hpp>

namespace Kortex
{
	class IGameProfile;
	class IVariableTable;
}

namespace Kortex
{
	namespace GameInstance
	{
		class TemplateLoader;

		enum CopyOptionsInstance: uint32_t
		{
			Config = 1 << 0,
		};
		enum CopyOptionsProfile: uint32_t
		{
			GameConfig = 1 << 0,
			GameSaves = 1 << 1,
		};
	}

	class IGameInstance:
		public KxRTTI::Interface<IGameInstance>,
		public Application::WithInstanceOptions<IGameInstance>
	{
		KxRTTI_DeclareIID(IGameInstance, {0x6fe63d61, 0x8666, 0x44fc, {0xbb, 0x4e, 0x4f, 0xcd, 0x93, 0x82, 0x82, 0xfc}});

		friend class GameInstance::TemplateLoader;

		public:
			using Vector = std::vector<std::unique_ptr<IGameInstance>>;
			using RefVector = std::vector<IGameInstance*>;
			using CRefVector = std::vector<const IGameInstance*>;
			using ProfilesVector = std::vector<std::unique_ptr<IGameProfile>>;

		public:
			static bool IsValidInstanceID(const kxf::String& id);
			static bool IsValidProfileID(const kxf::String& id);

			static wxBitmap GetGenericIcon();
			static kxf::String GetGenericIconLocation();

		public:
			static IGameInstance* CreateActive(const IGameInstance& instanceTemplate, const kxf::String& instanceID);
			static IGameInstance* GetActive();
			static void AssignActive(std::unique_ptr<IGameInstance> instance);

			static Vector& GetShallowInstances();
			static IGameInstance* GetShallowInstance(const kxf::String& instanceID);
			static IGameInstance* NewShallowInstance(const kxf::String& instanceID, const GameID& gameID);

			static kxf::String GetGameDefinitionsFolder();
			static kxf::String GetUserGameDefinitionsFolder();
			static void LoadTemplates();
			static void LoadInstances();

			static size_t GetTemplatesCount();
			static Vector& GetTemplates();
			static IGameInstance* GetTemplate(const GameID& id);
			static bool HasTemplate(const GameID& id);

		public:
			static kxf::String GetActiveProfileID();
			static IGameProfile* GetActiveProfile();
			static bool IsActiveProfileID(const kxf::String& id);

		protected:
			wxBitmap LoadIcon(const kxf::String& path) const;
			kxf::String GetDefaultIconLocation() const;

			virtual kxf::String CreateProfileID(const kxf::String& id) const = 0;
			virtual kxf::String CreateDefaultProfileID() const = 0;

			virtual bool ShouldInitProfiles() const = 0;
			virtual bool InitInstance() = 0;

		public:
			virtual ~IGameInstance() = default;

		public:
			virtual bool IsOK() const = 0;
			virtual bool IsTemplate() const = 0;
			virtual kxf::String GetDefinitionFile() const = 0;
		
			// Variables
			virtual IVariableTable& GetVariables() = 0;
			virtual const IVariableTable& GetVariables() const = 0;
			virtual kxf::String ExpandVariablesLocally(const kxf::String& variables) const = 0;
			virtual kxf::String ExpandVariables(const kxf::String& variables) const = 0;

			// Properties
			virtual GameID GetGameID() const = 0;
			virtual kxf::String GetInstanceID() const = 0;
			virtual kxf::String GetGameName() const = 0;
			virtual kxf::String GetGameShortName() const = 0;
		
			virtual int GetSortOrder() const = 0;
			virtual bool IsSystemTemplate() const = 0;
			virtual kxf::String GetIconLocation() const = 0;
			virtual wxBitmap GetIcon(const kxf::Size& iconSize = wxDefaultSize) const = 0;
			bool IsActiveInstance() const;

			virtual kxf::String GetInstanceTemplateDir() const = 0;
			virtual kxf::String GetInstanceDir() const = 0;
			virtual kxf::String GetInstanceRelativePath(const kxf::String& name) const = 0;

			virtual kxf::String GetConfigFile() const = 0;
			virtual kxf::String GetModsDir() const = 0;
			virtual kxf::String GetProfilesDir() const = 0;
			virtual kxf::String GetGameDir() const = 0;
			virtual kxf::String GetVirtualGameDir() const = 0;

			// Instances
			const IGameInstance& GetTemplate() const;
			IGameInstance& GetTemplate();

			bool Deploy(const IGameInstance* baseInstance = nullptr, uint32_t copyOptions = 0);
			bool WithdrawDeploy();
			bool IsDeployed() const;

			// Profiles
			bool HasProfiles() const
			{
				return !GetProfiles().empty();
			}
			size_t GetProfilesCount() const
			{
				return GetProfiles().size();
			}
			virtual const ProfilesVector& GetProfiles() const = 0;
			virtual ProfilesVector& GetProfiles() = 0;

			bool HasProfile(const kxf::String& id) const
			{
				return GetProfile(id) != nullptr;
			}
			virtual const IGameProfile* GetProfile(const kxf::String& id) const = 0;
			virtual IGameProfile* GetProfile(const kxf::String& id) = 0;

			virtual std::unique_ptr<IGameProfile> NewProfile() = 0;
			virtual IGameProfile* CreateProfile(const kxf::String& profileID, const IGameProfile* baseProfile = nullptr, uint32_t copyOptions = 0) = 0;
			virtual IGameProfile* ShallowCopyProfile(const IGameProfile& profile, const kxf::String& nameSuggets = wxEmptyString) = 0;
			virtual bool RemoveProfile(IGameProfile& profile) = 0;
			virtual bool RenameProfile(IGameProfile& profile, const kxf::String& newID) = 0;
			virtual bool ChangeProfileTo(IGameProfile& profile) = 0;
			virtual void LoadSavedProfileOrDefault() = 0;
	};
}

namespace Kortex
{
	class IConfigurableGameInstance:
		public KxRTTI::Interface<IConfigurableGameInstance>,
		public Application::IWithConfig
	{
		KxRTTI_DeclareIID(IConfigurableGameInstance, {0x2ef792f5, 0x69e7, 0x4f81, {0xb1, 0xbc, 0xfd, 0xfb, 0xc5, 0x4c, 0xe0, 0xda}});

		public:
			virtual void OnExit() = 0;
	};
}
