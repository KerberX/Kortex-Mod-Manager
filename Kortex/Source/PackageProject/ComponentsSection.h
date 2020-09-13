#pragma once
#include <Kortex/Kortex.hpp>
#include "ProjectSection.h"
#include "Utility/LabeledValue.h"
#include "Utility/WithNameAndID.h"

namespace Kortex::PackageProject
{
	class FlagItem: public Utility::LabeledValue, public wxObject
	{
		public:
			using Vector = std::vector<FlagItem>;
			
		public:
			static kxf::String GetDeletedFlagPrefix();
			
		private:
			bool HasLabel() const = delete;
			const kxf::String& GetRawLabel() const = delete;
			const kxf::String& GetLabel() const = delete;
			void SetLabel(const kxf::String& label) = delete;
			
		public:
			FlagItem(const kxf::String& value, const kxf::String& name = wxEmptyString);
			~FlagItem() = default;
			
		public:
			const kxf::String& GetValue() const
			{
				return Utility::LabeledValue::GetValue();
			}
			void SetValue(const kxf::String& value)
			{
				Utility::LabeledValue::SetValue(value);
			}
			
			bool HasName() const
			{
				return Utility::LabeledValue::HasLabel();
			}
			const kxf::String& GetName() const
			{
				return Utility::LabeledValue::GetRawLabel();
			}
			kxf::String GetDeletedName() const
			{
				return GetDeletedFlagPrefix() + GetName();
			}
			void SetName(const kxf::String& value)
			{
				Utility::LabeledValue::SetLabel(value);
			}
	};
}

namespace Kortex::PackageProject
{
	class Condition: public wxObject
	{
		public:
			using Vector = std::vector<Condition>;
	
		private:
			FlagItem::Vector m_Flags;
			Operator m_Operator;
	
		public:
			bool HasFlags() const
			{
				return !m_Flags.empty();
			}
			FlagItem::Vector& GetFlags()
			{
				return m_Flags;
			}
			const FlagItem::Vector& GetFlags() const
			{
				return m_Flags;
			}
	
			Operator GetOperator() const
			{
				return m_Operator;
			}
			void SetOperator(Operator value)
			{
				m_Operator = value;
			}
	};
}

namespace Kortex::PackageProject
{
	class ConditionGroup
	{
		private:
			Condition::Vector m_Conditions;
			Operator m_Operator;
	
		public:
			bool HasConditions() const
			{
				return !m_Conditions.empty();
			}
			Condition::Vector& GetConditions()
			{
				return m_Conditions;
			}
			const Condition::Vector& GetConditions() const
			{
				return m_Conditions;
			}
			Condition& GetOrCreateFirstCondition()
			{
				if (m_Conditions.empty())
				{
					return m_Conditions.emplace_back();
				}
				else
				{
					return m_Conditions.front();
				}
			}
	
			Operator GetOperator() const
			{
				return m_Operator;
			}
			void SetOperator(Operator value)
			{
				m_Operator = value;
			}
	};
}

namespace Kortex::PackageProject
{
	class ComponentItem: public Utility::WithName
	{
		public:
			using Vector = std::vector<std::unique_ptr<ComponentItem>>;
			using RefVector = std::vector<ComponentItem*>;
	
		private:
			kxf::String m_Image;
			kxf::String m_Description;
			KxStringVector m_FileData;
			KxStringVector m_Requirements;
			TypeDescriptor m_TypeDescriptorDefault;
			TypeDescriptor m_TypeDescriptorConditional = TypeDescriptor::Invalid;
			TypeDescriptor m_TypeDescriptorCurrent = TypeDescriptor::Invalid;
			ConditionGroup m_TypeDescriptorConditions;
			Condition m_ConditionFlags;
	
		public:
			ComponentItem();
			~ComponentItem();
	
		public:
			const kxf::String& GetImage() const
			{
				return m_Image;
			}
			void SetImage(const kxf::String& value)
			{
				m_Image = value;
			}
	
			const kxf::String& GetDescription() const
			{
				return m_Description;
			}
			void SetDescription(const kxf::String& value)
			{
				m_Description = value;
			}
	
			KxStringVector& GetFileData()
			{
				return m_FileData;
			}
			const KxStringVector& GetFileData() const
			{
				return m_FileData;
			}
	
			KxStringVector& GetRequirements()
			{
				return m_Requirements;
			}
			const KxStringVector& GetRequirements() const
			{
				return m_Requirements;
			}
	
			TypeDescriptor GetTDDefaultValue() const
			{
				return m_TypeDescriptorDefault;
			}
			void SetTDDefaultValue(TypeDescriptor type)
			{
				m_TypeDescriptorDefault = type;
			}
			
			TypeDescriptor GetTDConditionalValue() const
			{
				return m_TypeDescriptorConditional;
			}
			void SetTDConditionalValue(TypeDescriptor type)
			{
				m_TypeDescriptorConditional = type;
			}
			
			TypeDescriptor GetTDCurrentValue() const
			{
				return m_TypeDescriptorCurrent != TypeDescriptor::Invalid ? m_TypeDescriptorCurrent : GetTDDefaultValue();
			}
			void SetTDCurrentValue(TypeDescriptor type)
			{
				m_TypeDescriptorCurrent = type;
			}
	
			ConditionGroup& GetTDConditionGroup()
			{
				return m_TypeDescriptorConditions;
			}
			const ConditionGroup& GetTDConditionGroup() const
			{
				return m_TypeDescriptorConditions;
			}
	
			Condition& GetConditionFlags()
			{
				return m_ConditionFlags;
			}
			const Condition& GetConditionFlags() const
			{
				return m_ConditionFlags;
			}
	};
}

namespace Kortex::PackageProject
{
	class ComponentGroup: public Utility::WithName
	{
		public:
			using Vector = std::vector<std::unique_ptr<ComponentGroup>>;
	
		private:
			SelectionMode m_SelectionMode;
			ComponentItem::Vector m_Items;
	
		public:
			ComponentGroup();
			~ComponentGroup();
	
		public:
			SelectionMode GetSelectionMode() const
			{
				return m_SelectionMode;
			}
			void SetSelectionMode(SelectionMode type)
			{
				m_SelectionMode = type;
			}
	
			ComponentItem::Vector& GetItems()
			{
				return m_Items;
			}
			const ComponentItem::Vector& GetItems() const
			{
				return m_Items;
			}
	};
}

namespace Kortex::PackageProject
{
	class ComponentStep: public Utility::WithName
	{
		public:
			using Vector = std::vector<std::unique_ptr<ComponentStep>>;
	
		private:
			ConditionGroup m_Conditions;
			ComponentGroup::Vector m_Items;
	
		public:
			ComponentStep();
			~ComponentStep();
	
		public:
			ConditionGroup& GetConditionGroup()
			{
				return m_Conditions;
			}
			const ConditionGroup& GetConditionGroup() const
			{
				return m_Conditions;
			}
	
			ComponentGroup::Vector& GetGroups()
			{
				return m_Items;
			}
			const ComponentGroup::Vector& GetGroups() const
			{
				return m_Items;
			}		
	};
}

namespace Kortex::PackageProject
{
	class ConditionalComponentStep
	{
		public:
			using Vector = std::vector<std::unique_ptr<ConditionalComponentStep>>;
	
		private:
			ConditionGroup m_Conditions;
			KxStringVector m_Items;
	
		public:
			ConditionalComponentStep();
			~ConditionalComponentStep();
			
		public:
			ConditionGroup& GetConditionGroup()
			{
				return m_Conditions;
			}
			const ConditionGroup& GetConditionGroup() const
			{
				return m_Conditions;
			}
	
			KxStringVector& GetItems()
			{
				return m_Items;
			}
			const KxStringVector& GetItems() const
			{
				return m_Items;
			}
	};
}

namespace Kortex::PackageProject
{
	class ComponentsSection: public ProjectSection
	{
		public:
			static const Operator ms_DefaultFlagsOperator = Operator::And;
			static const SelectionMode ms_DefaultSelectionMode = SelectionMode::Any;
			static const TypeDescriptor ms_DefaultTypeDescriptor = TypeDescriptor::Optional;
	
		public:
			static TypeDescriptor StringToTypeDescriptor(const kxf::String& name, TypeDescriptor default = ms_DefaultTypeDescriptor);
			static kxf::String TypeDescriptorToString(TypeDescriptor type);
			static kxf::String TypeDescriptorToTranslation(TypeDescriptor type);
	
			static SelectionMode StringToSelectionMode(const kxf::String& name);
			static kxf::String SelectionModeToString(SelectionMode type);
			static kxf::String SelectionModeToTranslation(SelectionMode type);
	
		private:
			enum class FlagAttribute
			{
				Name,
				Value,
			};
	
		private:
			KxStringVector m_RequiredFileData;
			ComponentStep::Vector m_Steps;
			ConditionalComponentStep::Vector m_ConditionalSteps;
	
		private:
			KxStringVector GetFlagsAttributes(FlagAttribute index) const;
	
		public:
			ComponentsSection(ModPackageProject& project);
			~ComponentsSection();
	
		public:
			KxStringVector& GetRequiredFileData()
			{
				return m_RequiredFileData;
			}
			const KxStringVector& GetRequiredFileData() const
			{
				return m_RequiredFileData;
			}
			
			ComponentStep::Vector& GetSteps()
			{
				return m_Steps;
			}
			const ComponentStep::Vector& GetSteps() const
			{
				return m_Steps;
			}
			
			ConditionalComponentStep::Vector& GetConditionalSteps()
			{
				return m_ConditionalSteps;
			}
			const ConditionalComponentStep::Vector& GetConditionalSteps() const
			{
				return m_ConditionalSteps;
			}
			
			KxStringVector GetFlagsNames() const;
			KxStringVector GetFlagsValues() const;
	};
}
