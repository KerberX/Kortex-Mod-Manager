#pragma once
#include <Kortex/Kortex.hpp>

// TODO: Revise usage of this class. Remove it if needed
namespace Kortex::Utility
{
	class LabeledValue: public wxClientDataContainer
	{
		public:
			using Vector = std::vector<LabeledValue>;
			using RefVector = std::vector<LabeledValue*>;

		private:
			kxf::String m_Value;
			kxf::String m_Label;

		public:
			LabeledValue(const kxf::String& value, const kxf::String& label = wxEmptyString)
				:m_Value(value), m_Label(label)
			{
			}
			virtual ~LabeledValue() = default;

		public:
			bool HasValue() const
			{
				return !m_Value.IsEmpty();
			}
			const kxf::String& GetValue() const
			{
				return m_Value;
			}
			void SetValue(const kxf::String& value)
			{
				m_Value = value;
			}
		
			bool HasLabel() const
			{
				return !m_Label.IsEmpty();
			}
			const kxf::String& GetRawLabel() const
			{
				return m_Label;
			}
			const kxf::String& GetLabel() const
			{
				return HasLabel() ? GetRawLabel() : GetValue();
			}
			void SetLabel(const kxf::String& label)
			{
				m_Label = label;
			}
	};
}

namespace Kortex::Utility
{
	inline LabeledValue::Vector ExpandVariablesInVector(const LabeledValue::Vector& items)
	{
		LabeledValue::Vector newItems;
		newItems.reserve(items.size());
		for (const LabeledValue& item: items)
		{
			newItems.emplace_back(KVarExp(item.GetValue()), KVarExp(item.GetRawLabel()));
		}
		return newItems;
	}
}
