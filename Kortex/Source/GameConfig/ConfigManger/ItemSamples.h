#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "ItemValue.h"
class kxf::XMLNode;

namespace Kortex::GameConfig
{
	class Item;

	enum class CompareResult
	{
		LessThan = 1,
		Equal = 2,
		GreaterThan = 3
	};
	CompareResult CompareStrings(const kxf::String& v1, const kxf::String& v2, SortOptionsValue sortOptions);
}

namespace Kortex::GameConfig
{
	enum class IntrinsicSamplingFunctionID: uint32_t
	{
		None = 0,
		FindFiles,
		GetAvailableTranslations,
		GetStartupWorkspaces,
		GetVideoAdapters,
		GetVideoModes,
		GetVirtualKeys,
	};
	struct IntrinsicSamplingFunctionDef: public KxIndexedEnum::Definition<IntrinsicSamplingFunctionDef, IntrinsicSamplingFunctionID, kxf::String, true>
	{
		inline static const TItem ms_Index[] =
		{
			{IntrinsicSamplingFunctionID::None, wxS("None")},
			{IntrinsicSamplingFunctionID::FindFiles, wxS("Intrinsic/FindFiles")},
			{IntrinsicSamplingFunctionID::GetAvailableTranslations, wxS("Intrinsic/GetAvailableTranslations")},
			{IntrinsicSamplingFunctionID::GetStartupWorkspaces, wxS("Intrinsic/GetStartupWorkspaces")},
			{IntrinsicSamplingFunctionID::GetVideoAdapters, wxS("Intrinsic/GetVideoAdapters")},
			{IntrinsicSamplingFunctionID::GetVideoModes, wxS("Intrinsic/GetVideoModes")},
			{IntrinsicSamplingFunctionID::GetVirtualKeys, wxS("Intrinsic/GetVirtualKeys")},
		};
	};
	using IntrinsicSamplingFunctionValue = KxIndexedEnum::Value<IntrinsicSamplingFunctionDef, IntrinsicSamplingFunctionID::None>;
}

namespace Kortex::GameConfig
{
	class SampleValue
	{
		public:
			using Vector = std::vector<SampleValue>;

		private:
			ItemValue m_Value;
			kxf::String m_Label;

		public:
			SampleValue() = default;
			SampleValue(SampleValue&& other)
			{
				*this = std::move(other);
			}
			SampleValue(const SampleValue& other)
			{
				*this = other;
			}

			template<class T> SampleValue(const T& value)
				:m_Value(value)
			{
			}

		public:
			const ItemValue& GetValue() const
			{
				return m_Value;
			}
			ItemValue& GetValue()
			{
				return m_Value;
			}

			bool HasLabel() const
			{
				return !m_Label.IsEmpty();
			}
			kxf::String GetLabel() const
			{
				return m_Label.IsEmpty() ? m_Value.As<kxf::String>() : m_Label;
			}
			void SetLabel(const kxf::String& label)
			{
				m_Label = label;
			}
	
		public:
			SampleValue& operator=(SampleValue&& other)
			{
				m_Value = std::move(other.m_Value);
				m_Label = std::move(other.m_Label);

				return *this;
			}
			SampleValue& operator=(const SampleValue& other)
			{
				m_Value = other.m_Value;
				m_Label = other.m_Label;

				return *this;
			}
	};
}

namespace Kortex::GameConfig
{
	class ISamplingFunction;

	class ItemSamples
	{
		friend class ISamplingFunction;

		private:
			Item& m_Item;
			SamplesSourceValue m_SourceType;
			SortOrderValue m_SortOrder;
			SortOptionsValue m_SortOptions;
			
			kxf::String m_SamplingFunctionName;
			IntrinsicSamplingFunctionValue m_IntrinsicSampligFunction;

			SampleValue::Vector m_Values;
			wxAny m_MinValue;
			wxAny m_MaxValue;
			wxAny m_Step;

		private:
			size_t LoadImmediateItems(const kxf::XMLNode& rootNode);
			void SortImmediateItems();
			void InvokeIntrinsicSamplingFunction(const ItemValue::Vector& arguments);
			template<class T> SortOrderValue LoadRange(T min, T max, T step)
			{
				if constexpr(std::is_signed_v<T>)
				{
					m_Values.reserve(std::abs(max - min));
				}

				m_MinValue = min;
				m_MaxValue = max;
				m_Step = step;

				for (T i = min; i <= max; i += step)
				{
					SampleValue& sample = m_Values.emplace_back();
					sample.GetValue().Assign(i);
				}
				return step >= 0 ? SortOrderID::Ascending : SortOrderID::Descending;
			}

			SampleValue::Vector& GetSampleValues()
			{
				return m_Values;
			}
			template<class TItems, class TFunctor> static void DoForEachItem(TItems&& items, TFunctor&& func)
			{
				for (auto&& item: items)
				{
					func(item);
				}
			}

		public:
			ItemSamples(Item& item, const kxf::XMLNode& samplesNode = {});

		public:
			void Load(const kxf::XMLNode& samplesNode);
			
			bool IsEmpty() const
			{
				return m_Values.empty();
			}
			size_t GetCount() const
			{
				return m_Values.size();
			}
			Item& GetItem() const
			{
				return m_Item;
			}

			SamplesSourceValue GetSourceType() const
			{
				return m_SourceType;
			}
			SortOrderValue GetSortOrder() const
			{
				return m_SortOrder;
			}
			SortOptionsValue GetSortOptions() const
			{
				return m_SortOptions;
			}
			
			kxf::String GetSamplingFunctionName() const
			{
				return m_SamplingFunctionName;
			}
			IntrinsicSamplingFunctionValue GetIntrinsicSamplingFunction() const
			{
				return m_IntrinsicSampligFunction;
			}

			bool HasStep() const;
			bool HasBoundValues() const;
			template<class T> void GetBoundValues(T& min, T& max) const
			{
				static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

				if (m_MinValue.IsNull() || !m_MinValue.GetAs(&min))
				{
					min = std::numeric_limits<T>::lowest();
				}
				if (m_MaxValue.IsNull() || !m_MaxValue.GetAs(&max))
				{
					max = std::numeric_limits<T>::max();
				}
			}

			template<class TFunctor> void ForEachSample(TFunctor&& func) const
			{
				DoForEachItem(m_Values, func);
			}
			template<class TFunctor> void ForEachSample(TFunctor&& func)
			{
				DoForEachItem(m_Values, func);
			}
			
			const SampleValue* GetSampleByIndex(size_t index) const
			{
				if (index < m_Values.size())
				{
					return &m_Values[index];
				}
				return nullptr;
			}
			const SampleValue* FindSampleByValue(const ItemValue& value, size_t* index = nullptr) const;
			const SampleValue* FindSampleByLabel(const kxf::String& label, size_t* index = nullptr) const;
	};
}
