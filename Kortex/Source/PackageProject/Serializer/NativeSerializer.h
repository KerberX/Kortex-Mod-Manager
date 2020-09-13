#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageProject/Serializer.h"
#include <KxFramework/KxXML.h>

namespace Kortex
{
	class ModPackageProject;
}

namespace Kortex::PackageProject
{
	class NativeSerializer: public Serializer
	{
		private:
			const bool m_AsProject = false;
			ModPackageProject* m_ProjectLoad = nullptr;
			const ModPackageProject* m_ProjectSave = nullptr;
			kxf::String m_Data;
			kxf::XMLDocument m_XML;
			
		private:
			void ReadBase();
			void ReadConfig();
			void ReadInfo();
			void ReadInterface();
			void ReadFiles();
			void ReadRequirements();
			void ReadComponents();
			
			kxf::XMLNode WriteBase();
			void WriteConfig(kxf::XMLNode& baseNode);
			void WriteInfo(kxf::XMLNode& baseNode);
			void WriteInterface(kxf::XMLNode& baseNode);
			void WriteFiles(kxf::XMLNode& baseNode);
			void WriteRequirements(kxf::XMLNode& baseNode);
			void WriteComponents(kxf::XMLNode& baseNode);
	
		public:
			NativeSerializer(bool asProject = false)
				:m_AsProject(asProject)
			{
				SetPackageDataRoot(GetDefaultFOModRoot());
			}
			
		public:
			void Serialize(const ModPackageProject& project) override;
			void Structurize(ModPackageProject& project) override;
			
			const kxf::String& GetData() const
			{
				return m_Data;
			}
			void SetData(const kxf::String& sData)
			{
				m_Data = sData;
			}
	};
}
