#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageProject/Serializer.h"
#include "PackageProject/Common.h"
#include "PackageProject/ComponentsSection.h"
#include <KxFramework/KxXML.h>

namespace Kortex
{
	class ModPackageProject;
}
namespace Kortex::PackageProject
{
	class FileItem;
	class RequirementGroup;
}

namespace Kortex::PackageProject
{
	class FOModSerializer: public Serializer
	{
		private:
			kxf::String m_InfoXML;
			kxf::String m_ModuleConfigXML;
			kxf::String m_ProjectFolder;
	
			ModPackageProject* m_ProjectLoad = nullptr;
			const ModPackageProject* m_ProjectSave = nullptr;
			kxf::XMLDocument m_XML;
			bool m_ExportToNativeFormat = false;
	
			kxf::String m_EffectiveArchiveRoot;
			bool m_HasDataFolderAsRoot = false;
			bool m_IsMorrowind = false;
	
		private:
			bool IsRootPathHandlingNeeded() const
			{
				return m_HasDataFolderAsRoot || m_IsMorrowind;
			}
			kxf::String GetDataFolderName(bool withSeparator) const;
			kxf::String MakeProjectPath(const kxf::String& path) const;
			SelectionMode ConvertSelectionMode(const kxf::String& mode) const;
			kxf::String ConvertSelectionMode(SelectionMode mode) const;
			template<class T> void UniqueStringArray(T& array)
			{
				auto it = std::unique(array.begin(), array.end());
				array.erase(it, array.end());
			}
	
		private:
			// Structurize
			void ReadInfo();
	
			void ReadInstallSteps();
			void ReadConditionalSteps(const kxf::XMLNode& stepsArrayNode);
			std::vector<FileItem*> ReadFileData(const kxf::XMLNode& filesArrayNode, ComponentItem* entry = nullptr);
			void UniqueFileData();
			void UniqueImages();
	
			// Serialize
			void WriteInfo();
			void WriteSites(kxf::XMLNode& infoNode, kxf::XMLNode& sitesNode);
	
			void WriteInstallSteps();
			void WriteConditionalSteps(kxf::XMLNode& stepsArrayNode);
			void WriteFileData(kxf::XMLNode& node, const KxStringVector& files, bool alwaysInstall = false);
			void WriteRequirements(kxf::XMLNode& node, const KxStringVector& requiremetSets);
	
		private:
			void InitDataFolderInfo();
			void Init();
	
		public:
			FOModSerializer(const kxf::String& projectFolder = wxEmptyString);
			FOModSerializer(const kxf::String& sInfoXML, const kxf::String& moduleConfigXML, const kxf::String& projectFolder = wxEmptyString);
	
		public:
			void Serialize(const ModPackageProject& project) override;
			void Structurize(ModPackageProject& project) override;
	
			void ExportToNativeFormat(bool value)
			{
				m_ExportToNativeFormat = value;
				SetPackageDataRoot(m_ExportToNativeFormat ? GetDefaultFOModRoot() : GetDefaultKMPRoot());
			}
			void SetEffectiveArchiveRoot(const kxf::String& path)
			{
				m_EffectiveArchiveRoot = path;
				if (!m_EffectiveArchiveRoot.IsEmpty() && m_EffectiveArchiveRoot.Last() == '\\')
				{
					m_EffectiveArchiveRoot.RemoveLast(1);
				}
			}
			
			const kxf::String& GetInfoXML() const
			{
				return m_InfoXML;
			}
			const kxf::String& GetModuleConfigXML() const
			{
				return m_ModuleConfigXML;
			}
	};
}
