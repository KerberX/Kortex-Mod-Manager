#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageProject/Serializer.h"
#include <KxFramework/KxXML.h>
#include <KxFramework/kxf::Version.h>

namespace Kortex
{
	class ModPackageProject;
}

namespace Kortex::PackageProject
{
	class RequirementItem;

	class LegacySerializer: public Serializer
	{
		private:
			kxf::Version m_ProjectVersion;
			ModPackageProject* m_Project = nullptr;
			kxf::String m_Data;
			kxf::XMLDocument m_XML;
			
		private:
			kxf::String ConvertMultiLine(const kxf::String& source) const;
			kxf::String ConvertVariable(const kxf::String& sOldVariable) const;
			void AddSite(const kxf::String& url);
			void FixRequirementID(RequirementItem* entry) const;
			bool IsComponentsUsed() const;
			void ReadInterface3x4x5x(const kxf::String& sLogoNodeName);
			void ReadFiles3x4x();
			
		private:
			kxf::Version ReadBase();
			void ReadConfig();
			
			// 3.0+
			void ReadInfo3x();
			void ReadFiles3x();
			void ReadInterface3x();
			void ReadRequirements3x();
			void ReadComponents3x();
			
			// 4.0+
			void ReadInfo4x();
			void ReadFiles4x();
			void ReadInterface4x();
			void ReadRequirements4x();
			void ReadComponents4x();
			
			// 5.0+
			void ReadInfo5x();
			void ReadInterface5x();
			void ReadFiles5x();
			void ReadRequirements5x();
			void ReadComponents5x();
			void ReadINI5x();
	
		public:
			void Serialize(const ModPackageProject& project) override
			{
			}
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
