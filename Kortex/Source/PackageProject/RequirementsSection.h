#pragma once
#include <Kortex/Kortex.hpp>
#include "ProjectSection.h"
#include "Utility/WithNameAndID.h"
#include <KxFramework/kxf::Version.h>

namespace Kortex::PackageProject
{
	class RequirementItem: public Utility::WithNameAndID
	{
		public:
			using Vector = std::vector<std::unique_ptr<RequirementItem>>;
			using RefVector = std::vector<RequirementItem*>;
	
		private:
			kxf::String m_Object;
			kxf::Version m_RequiredVersion;
	
			mutable bool m_CurrentVersionChecked = false;
			mutable kxf::Version m_CurrentVersion;
			
			Operator m_RequiredVersionFunction;
			ObjectFunction m_ObjectFunction;
	
			mutable bool m_ObjectFunctionResultChecked = false;
			mutable ReqState m_ObjectFunctionResult = ReqState::Unknown;
	
			kxf::String m_Description;
			kxf::String m_BinaryVersionKind = "FileVersion";
			
			bool m_OverallStatusCalculated = false;
			bool m_OverallStatus = false;
	
			ReqType m_Type = ReqType::Auto;
			kxf::String m_Category;
			KxStringVector m_Dependencies;
	
		public:
			RequirementItem(ReqType type = ReqType::Auto);
			~RequirementItem();
	
		public:
			const kxf::String& GetObject() const
			{
				return m_Object;
			}
			void SetObject(const kxf::String& value)
			{
				m_Object = value;
			}
			
			ObjectFunction GetObjectFunction() const
			{
				return m_ObjectFunction;
			}
			void SetObjectFunction(ObjectFunction state)
			{
				m_ObjectFunction = state;
			}
			ReqState GetObjectFunctionResult() const;
			void ResetObjectFunctionResult();

			const kxf::Version& GetRequiredVersion() const
			{
				return m_RequiredVersion;
			}
			void SetRequiredVersion(const kxf::String& value)
			{
				m_RequiredVersion = value;
			}
			Operator GetRequiredVersionOperator() const
			{
				return m_RequiredVersionFunction;
			}
			void SetRequiredVersionOperator(Operator operatorType)
			{
				m_RequiredVersionFunction = operatorType;
			}
			
			const kxf::Version& GetCurrentVersion() const;
			void SetCurrentVersion(const kxf::String& value)
			{
				m_CurrentVersion = value;
			}
			void ResetCurrentVersion();
			bool CheckVersion() const;
			
			const kxf::String& GetDescription() const
			{
				return m_Description;
			}
			void SetDescription(const kxf::String& value)
			{
				m_Description = value;
			}
			
			const kxf::String& GetBinaryVersionKind() const
			{
				return m_BinaryVersionKind;
			}
			void SetBinaryVersionKind(const kxf::String& value)
			{
				m_BinaryVersionKind = value;
			}
			
			bool IsTypeStd() const;
			bool IsTypeSystem() const;
			bool IsTypeUserEditable() const;
			
			ReqType GetType() const
			{
				return m_Type;
			}
			bool SetType(ReqType type, bool force = false);
			bool ConformToType();
			
			const kxf::String& GetCategory() const
			{
				return m_Category;
			}
			void SetCategory(const kxf::String& value)
			{
				m_Category = value;
			}
			
			KxStringVector& GetDependencies()
			{
				return m_Dependencies;
			}
			const KxStringVector& GetDependencies() const
			{
				return m_Dependencies;
			}
			
			bool GetOverallStatus() const
			{
				return m_OverallStatus;
			}
			bool CalcOverallStatus();
	};
}

namespace Kortex::PackageProject
{
	class RequirementGroup
	{
		public:
			using Vector = std::vector<std::unique_ptr<RequirementGroup>>;
			using RefVector = std::vector<RequirementGroup*>;
	
		public:
			static kxf::String GetFlagNamePrefix();
			static kxf::String GetFlagName(const kxf::String& id);
	
		private:
			kxf::String m_ID;
			Operator m_Operator;
			RequirementItem::Vector m_Items;
	
			bool m_GroupStatus = false;
			bool m_GroupStatusCalculated = false;
	
		public:
			RequirementGroup();
			~RequirementGroup();
	
		public:
			const kxf::String& GetID() const
			{
				return m_ID;
			}
			void SetID(const kxf::String& id)
			{
				m_ID = id;
			}
			kxf::String GetFlagName() const
			{
				return GetFlagName(GetID());
			}
	
			Operator GetOperator() const
			{
				return m_Operator;
			}
			void SetOperator(Operator value)
			{
				m_Operator = value;
			}
	
			RequirementItem::Vector& GetItems()
			{
				return m_Items;
			}
			const RequirementItem::Vector& GetItems() const
			{
				return m_Items;
			}
	
			RequirementItem* FindItem(const kxf::String& id) const;
			bool HasItemWithID(const kxf::String& id) const
			{
				return FindItem(id) != nullptr;
			}
	
			bool CalcGroupStatus();
			bool GetGroupStatus() const
			{
				return m_GroupStatus;
			}
	};
}

namespace Kortex::PackageProject
{
	class RequirementsSection: public ProjectSection
	{
		public:
			static const Operator ms_DefaultGroupOperator = Operator::And;
			static const Operator ms_DefaultVersionOperator = Operator::GreaterThanOrEqual;
			static const ObjectFunction ms_DefaultObjectFunction = ObjectFunction::None;
			static const ReqType ms_DefaultTypeDescriptor = ReqType::Auto;
	
		public:
			static ObjectFunction StringToObjectFunction(const kxf::String& name);
			static kxf::String ObjectFunctionToString(ObjectFunction state);
	
			static ReqType StringToTypeDescriptor(const kxf::String& name);
			static kxf::String TypeDescriptorToString(ReqType type);
	
			static bool CompareVersions(Operator operatorType, const kxf::Version& current, const kxf::Version& required);
	
		private:
			RequirementGroup::Vector m_Groups;
			KxStringVector m_DefaultGroup;
	
		public:
			RequirementsSection(ModPackageProject& project);
			virtual ~RequirementsSection();
	
		public:
			RequirementGroup::Vector& GetGroups()
			{
				return m_Groups;
			}
			const RequirementGroup::Vector& GetGroups() const
			{
				return m_Groups;
			}
	
			bool IsDefaultGroupEmpty() const
			{
				return m_DefaultGroup.empty();
			}
			const KxStringVector& GetDefaultGroup() const
			{
				return m_DefaultGroup;
			}
			KxStringVector& GetDefaultGroup()
			{
				return m_DefaultGroup;
			}
			bool IsDefaultGroupContains(const kxf::String& groupID) const;
	
			RequirementGroup* FindGroupWithID(const kxf::String& id) const;
			bool HasSetWithID(const kxf::String& id) const
			{
				return FindGroupWithID(id) != nullptr;
			}
	
			KxStringVector GetFlagNames() const;
			bool CalcOverallStatus(const KxStringVector& groups) const;
	};
}
