#pragma once
#include <Kortex/Kortex.hpp>
#include "GameMods/IModImporter.h"
#include <kxf::UI::Framework/KxINI.h>

namespace Kortex::ModManager
{
	class ModImporterMO: public IModImporter
	{
		private:
			kxf::String m_InstanceDirectory;
			kxf::String m_ModsDirectory;
			kxf::String m_ProfilesDirectory;
			kxf::String m_DownloadsDirectory;
			KxINI m_Options;

			kxf::String m_CurrentProfile;
			kxf::String m_ModManagerName;
			GameID m_TargetGameID;
			const IGameInstance* m_TargetInstance = nullptr;
			bool m_CanImport = false;

		private:
			kxf::String& DecodeUTF8(kxf::String& path) const;
			kxf::String& ProcessFilePath(kxf::String& path) const;
			kxf::String& ProcessDescription(kxf::String& path) const;

			GameID TranslateGameIDToNetwork(const kxf::String& name);
			void LoadOptions();
			kxf::String GetDataFolderName() const;
			kxf::String GetProfileDirectory() const;

			void ReadExecutables(Utility::OperationWithProgressDialogBase* context);
			void CopySaves(Utility::OperationWithProgressDialogBase* context);
			void CopyMods(Utility::OperationWithProgressDialogBase* context);
			void ReadPlugins(Utility::OperationWithProgressDialogBase* context);
			void CopyGameConfig(Utility::OperationWithProgressDialogBase* context);
			void CopyDownloads(Utility::OperationWithProgressDialogBase* context);

		public:
			void SetDirectory(const kxf::String& path) override;
			void Import(Utility::OperationWithProgressDialogBase* context) override;
			
			bool CanImport() const override;
			GameID GetTargetGameID() const override
			{
				return m_TargetGameID;
			}
			kxf::String GetModManagerName() const override
			{
				return m_ModManagerName;
			}
			kxf::String GetAdditionalInfo() const override;
			kxf::String GetCurrentProfile() const override
			{
				return m_CurrentProfile;
			}
			KxStringVector GetAvailableProfiles() const override;
	};
}
