#pragma once
#include <Kortex/Kortex.hpp>
#include "IGameInstance.h"
#include "Application/VariablesTable/VariablesDatabase.h"
#include "Application/VariablesTable/StaticVariableTable.h"
#include "GameID.h"
#include <kxf::UI::Framework/KxXML.h>
#include <kxf::UI::Framework/KxFileStream.h>
class KActiveGameInstance;
class IVariableTable;
class IGameProfile;
class kxf::XMLDocument;

namespace Kortex::GameInstance
{
	class DefaultGameInstance: public KxRTTI::ExtendInterface<DefaultGameInstance, IGameInstance>
	{
		KxRTTI_DeclareIID(DefaultGameInstance, {0x73a46d8d, 0x1a8b, 0x4adc, {0xa5, 0x50, 0xbb, 0xfd, 0x36, 0x2c, 0x31, 0x83}});

		friend class IGameInstance;

		protected:
			GameID m_GameID;
			kxf::String m_InstanceID;
			kxf::String m_DefinitionFile;

			kxf::String m_GameName;
			kxf::String m_GameShortName;
			int m_SortOrder = -1;
			bool m_IsSystemTemplate = true;

			StaticVariableTable m_Variables;
			ProfilesVector m_Profiles;

		protected:
			kxf::String CreateProfileID(const kxf::String& id) const override;
			kxf::String CreateDefaultProfileID() const override;

			void LoadVariables(const kxf::XMLDocument& instanceConfig, const kxf::XMLDocument* userConfig = nullptr);
			kxf::String LoadRegistryVariable(const kxf::XMLNode& node) const;
			void DetectGameArchitecture(const kxf::XMLDocument& instanceConfig);

			virtual bool OnLoadInstance(const kxf::XMLDocument& templateConfig)
			{
				return true;
			}
			bool ShouldInitProfiles() const override
			{
				return false;
			}
			bool InitInstance() override;

		protected:
			DefaultGameInstance() = default;
			void Create(const kxf::String& definitionFile, const kxf::String& instanceID, bool isSystemTemplate)
			{
				m_DefinitionFile = definitionFile;
				m_InstanceID = instanceID;
				m_IsSystemTemplate = isSystemTemplate;
			}

		public:
			DefaultGameInstance(const kxf::String& templateFile, const kxf::String& instanceID, bool isSystemTemplate)
				:m_DefinitionFile(templateFile), m_InstanceID(instanceID), m_IsSystemTemplate(isSystemTemplate)
			{
			}

		public:
			bool IsOK() const override
			{
				return m_GameID.IsOK();
			}
			bool IsTemplate() const override
			{
				return m_InstanceID.IsEmpty();
			}
			kxf::String GetDefinitionFile() const override
			{
				return m_DefinitionFile;
			}
		
			// Variables
			IVariableTable& GetVariables() override
			{
				return m_Variables;
			}
			const IVariableTable& GetVariables() const override
			{
				return m_Variables;
			}
			kxf::String ExpandVariablesLocally(const kxf::String& variables) const override;
			kxf::String ExpandVariables(const kxf::String& variables) const override;

			// Properties
			GameID GetGameID() const override
			{
				return m_GameID;
			}
			kxf::String GetInstanceID() const override
			{
				return m_InstanceID;
			}
			kxf::String GetGameName() const override
			{
				return m_GameName.IsEmpty() ? m_GameShortName : m_GameName;
			}
			kxf::String GetGameShortName() const override
			{
				return m_GameShortName.IsEmpty() ? m_GameName : m_GameShortName;
			}
		
			int GetSortOrder() const override
			{
				return m_SortOrder;
			}
			bool IsSystemTemplate() const override
			{
				return m_IsSystemTemplate;
			}
			
			kxf::String GetIconLocation() const override;
			wxBitmap GetIcon(const kxf::Size& iconSize = wxDefaultSize) const override;

			kxf::String GetInstanceTemplateDir() const override;
			kxf::String GetInstanceDir() const override;
			kxf::String GetInstanceRelativePath(const kxf::String& name) const override;

			kxf::String GetConfigFile() const override;
			kxf::String GetModsDir() const override;
			kxf::String GetProfilesDir() const override;
			kxf::String GetGameDir() const override;
			kxf::String GetVirtualGameDir() const override;

			// Profiles
			const ProfilesVector& GetProfiles() const override
			{
				return m_Profiles;
			}
			ProfilesVector& GetProfiles() override
			{
				return m_Profiles;
			}

			const IGameProfile* GetProfile(const kxf::String& id) const override;
			IGameProfile* GetProfile(const kxf::String& id) override;

			std::unique_ptr<IGameProfile> NewProfile() override;
			IGameProfile* CreateProfile(const kxf::String& profileID, const IGameProfile* baseProfile = nullptr, uint32_t copyOptions = 0) override;
			IGameProfile* ShallowCopyProfile(const IGameProfile& profile, const kxf::String& nameSuggets = wxEmptyString) override;
			bool RemoveProfile(IGameProfile& profile) override;
			bool RenameProfile(IGameProfile& profile, const kxf::String& newID) override;
			bool ChangeProfileTo(IGameProfile& profile) override;
			void LoadSavedProfileOrDefault() override;
	};
}

namespace Kortex::GameInstance
{
	class ConfigurableGameInstance: public KxRTTI::ImplementInterface<ConfigurableGameInstance, DefaultGameInstance, IConfigurableGameInstance>
	{
		private:
			kxf::XMLDocument m_Config;
			const bool m_WasCreatedUsingOnlyInstanceID = false;

		protected:
			void LoadProfiles(const kxf::XMLDocument& instanceConfig);
			void LoadConfigFile();
			bool InitInstance() override;
			bool OnLoadInstance(const kxf::XMLDocument& templateConfig) override;

		public:
			ConfigurableGameInstance(const kxf::String& instanceID);
			ConfigurableGameInstance(const IGameInstance& instanceTemplate, const kxf::String& instanceID);

		public:
			const kxf::XMLDocument& GetConfig() const override
			{
				return m_Config;
			}
			kxf::XMLDocument& GetConfig() override
			{
				return m_Config;
			}
			
			void OnConfigChanged(AppOption& option) override;
			void SaveConfig() override;
			void OnExit() override;
	};
}
