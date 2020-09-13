#pragma once
#include <Kortex/Kortex.hpp>
#include "GameMods/IModImporter.h"
#include <KxFramework/KxXML.h>

namespace Kortex::ModManager
{
	class ModImporterNMM: public IModImporter
	{
		private:
			kxf::String m_InstanceDirectory;
			kxf::XMLDocument m_ProfileManagerXML;

			std::vector<std::pair<kxf::String, kxf::String>> m_ProfilesList;
			GameID m_TargetGameID;
			const IGameInstance* m_TargetGame = nullptr;
			bool m_CanImport = false;

		private:
			kxf::String ProcessDescription(const kxf::String& path) const;

			GameID GetGameID(const kxf::String& name);
			void LoadOptions();
			kxf::String GetDataFolderName() const;
			kxf::String GetProfileDirectory() const;

			void CopySavesAndConfig(Utility::OperationWithProgressDialogBase* context);
			void CopyMods(Utility::OperationWithProgressDialogBase* context);
			void ReadPlugins(Utility::OperationWithProgressDialogBase* context);
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
				return "Nexus Mod Manager";
			}
			kxf::String GetAdditionalInfo() const override;
			kxf::String GetCurrentProfile() const override
			{
				return wxEmptyString;
			}
			KxStringVector GetAvailableProfiles() const override;
	};
}
