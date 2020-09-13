#pragma once
#include <Kortex/Kortex.hpp>
#include "ConfigSection.h"
#include "InfoSection.h"
#include "FileDataSection.h"
#include "InterfaceSection.h"
#include "RequirementsSection.h"
#include "ComponentsSection.h"
#include "Common.h"
#include <KxFramework/kxf::Version.h>

namespace Kortex
{
	class ModPackageProject
	{
		public:
			static kxf::String OperatorToSymbolicName(PackageProject::Operator operatorType);
			static kxf::String OperatorToString(PackageProject::Operator operatorType);
			static PackageProject::Operator StringToOperator(const kxf::String& name, bool allowNone, PackageProject::Operator default);
	
			static KxStringVector CreateOperatorSymNamesList(const std::initializer_list<PackageProject::Operator>& operators);
			static KxStringVector CreateOperatorNamesList(const std::initializer_list<PackageProject::Operator>& operators);
	
		private:
			kxf::Version m_FormatVersion;
			kxf::String m_TargetProfileID;
			kxf::String m_ModID;
	
			// Project parts
			PackageProject::ConfigSection m_Config;
			PackageProject::InfoSection m_Info;
			PackageProject::FileDataSection m_FileData;
			PackageProject::InterfaceSection m_Interface;
			PackageProject::RequirementsSection m_Requirements;
			PackageProject::ComponentsSection m_Components;
	
		public:
			ModPackageProject();
			virtual ~ModPackageProject();
	
		public:
			const kxf::Version& GetFormatVersion() const
			{
				return m_FormatVersion;
			}
			void SetFormatVersion(const kxf::Version& id)
			{
				m_FormatVersion = id;
			}
			
			const kxf::String& GetTargetProfileID() const
			{
				return m_TargetProfileID;
			}
			void SetTargetProfileID(const kxf::String& id)
			{
				m_TargetProfileID = id;
			}
			
			void SetModID(const kxf::String& id);
			kxf::String GetModID() const;
			kxf::String GetModName() const;
			kxf::String GetSignature() const;
	
			PackageProject::ConfigSection& GetConfig()
			{
				return m_Config;
			}
			const PackageProject::ConfigSection& GetConfig() const
			{
				return m_Config;
			}
			
			PackageProject::InfoSection& GetInfo()
			{
				return m_Info;
			}
			const PackageProject::InfoSection& GetInfo() const
			{
				return m_Info;
			}
			
			PackageProject::FileDataSection& GetFileData()
			{
				return m_FileData;
			}
			const PackageProject::FileDataSection& GetFileData() const
			{
				return m_FileData;
			}
			
			PackageProject::InterfaceSection& GetInterface()
			{
				return m_Interface;
			}
			const PackageProject::InterfaceSection& GetInterface() const
			{
				return m_Interface;
			}
			
			PackageProject::RequirementsSection& GetRequirements()
			{
				return m_Requirements;
			}
			const PackageProject::RequirementsSection& GetRequirements() const
			{
				return m_Requirements;
			}
			
			PackageProject::ComponentsSection& GetComponents()
			{
				return m_Components;
			}
			const PackageProject::ComponentsSection& GetComponents() const
			{
				return m_Components;
			}
	};
}

namespace Kortex::PackageProject
{
	class ConditionChecker final
	{
		private:
			bool m_Result = false;
			bool m_IsFirstElement = true;

		public:
			void operator()(bool value, Operator operatorType);

		public:
			bool GetResult() const
			{
				return m_Result;
			}
	};
}
