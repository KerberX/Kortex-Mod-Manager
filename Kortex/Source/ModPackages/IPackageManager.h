#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IManager.h"
#include "PackageProject/Common.h"
#include "PackageProject/RequirementsSection.h"
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/KxComponentSystem.h>
class kxf::UI::Menu;

namespace Kortex
{
	class IGameMod;
}

namespace Kortex
{
	namespace PackageDesigner::Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	}

	class IPackageManager:
		public ManagerWithTypeInfo<IManager, PackageDesigner::Internal::TypeInfo>,
		public KxComponentContainer,
		public kxf::SingletonPtr<IPackageManager>
	{
		friend class ModPackagesModule;

		public:
			static KxStringVector GetSuppoptedExtensions();
			static kxf::String GetSuppoptedExtensionsFilter();
			static void ExtractAcrhiveWithProgress(wxWindow* window, const kxf::String& filePath, const kxf::String& outPath);

		public:
			static bool IsPathAbsolute(const kxf::String& path);
			static kxf::String GetRequirementFilePath(const PackageProject::RequirementItem* entry);
			static PackageProject::ReqState CheckRequirementState(const PackageProject::RequirementItem* entry);
			static kxf::Version GetRequirementVersionFromBinaryFile(const PackageProject::RequirementItem* entry);
			static kxf::Version GetRequirementVersionFromModManager(const PackageProject::RequirementItem* entry);
			static kxf::Version GetRequirementVersion(const PackageProject::RequirementItem* entry);

		protected:
			void LoadRequirementsGroup(PackageProject::RequirementGroup& group, const kxf::XMLNode& rootNode);

		public:
			IPackageManager();

		public:
			kxf::String GetPackagesFolder() const;
			void SetPackagesFolder(const kxf::String& path) const;

		public:
			virtual const PackageProject::RequirementItem::Vector& GetStdRequirements() const = 0;
			virtual const PackageProject::RequirementItem* FindStdReqirement(const kxf::String& id) const = 0;

			virtual void OnModListMenu(kxf::UI::Menu& menu, const std::vector<IGameMod*>& selectedMods, IGameMod* focusedMod) = 0;
	};
}
