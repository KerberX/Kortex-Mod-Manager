#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxIndexedEnum.h>

namespace Kortex::GameConfig
{
	enum class DataTypeID: uint32_t
	{
		None = 0,
		Any = std::numeric_limits<uint32_t>::max(),

		Int8 = 1 << 0,
		Int16 = 1 << 1,
		Int32 = 1 << 2,
		Int64 = 1 << 3,
		UInt8 = 1 << 4,
		UInt16 = 1 << 5,
		UInt32 = 1 << 6,
		UInt64 = 1 << 7,

		Float32 = 1 << 8,
		Float64 = 1 << 9,

		Bool = 1 << 10,
		String = 1 << 11,
		Struct = 1 << 12,
	};
	struct DataTypeDef: public KxIndexedEnum::Definition<DataTypeDef, DataTypeID, kxf::String, false>
	{
		// Serialization is done with lower-cased versions of these identifiers
		inline static const TItem ms_Index[] =
		{
			{DataTypeID::None, wxS("none")},
			{DataTypeID::Any, wxS("any")},

			{DataTypeID::Int8, wxS("int8")},
			{DataTypeID::Int16, wxS("int16")},
			{DataTypeID::Int32, wxS("int32")},
			{DataTypeID::Int64, wxS("int64")},
			{DataTypeID::UInt8, wxS("uint8")},
			{DataTypeID::UInt16, wxS("uint16")},
			{DataTypeID::UInt32, wxS("uint32")},
			{DataTypeID::UInt64, wxS("uint64")},
			{DataTypeID::Float32, wxS("float32")},
			{DataTypeID::Float64, wxS("float64")},

			{DataTypeID::Bool, wxS("bool")},
			{DataTypeID::String, wxS("string")},
			{DataTypeID::Struct, wxS("struct")},

			// Aliases, they are last items because deserialization
			// should not prefer them over definitive types.
			{DataTypeID::Int32, wxS("int")},
			{DataTypeID::UInt32, wxS("uint")},
			{DataTypeID::Float32, wxS("float")},
		};
	};
	using DataTypeValue = KxIndexedEnum::Value<DataTypeDef, DataTypeID::None>;
}

namespace Kortex::GameConfig
{
	enum class TypeDetectorID
	{
		None,

		HungarianNotation,
		DataAnalysis
	};
	struct TypeDetectorDef: public KxIndexedEnum::Definition<TypeDetectorDef, TypeDetectorID, kxf::String, true>
	{
		inline static const TItem ms_Index[] =
		{
			{TypeDetectorID::HungarianNotation, wxS("HungarianNotation")},
			{TypeDetectorID::DataAnalysis, wxS("DataAnalysis")},
		};
	};
	using TypeDetectorValue = KxIndexedEnum::Value<TypeDetectorDef, TypeDetectorID::None>;
}

namespace Kortex::GameConfig
{
	enum class SourceFormat
	{
		None,

		INI,
		XML,
		Registry,
	};
	struct SourceFormatDef: public KxIndexedEnum::Definition<SourceFormatDef, SourceFormat, kxf::String, true>
	{
		inline static const TItem ms_Index[] =
		{
			{SourceFormat::None, wxS("None")},

			{SourceFormat::INI, wxS("INI")},
			{SourceFormat::XML, wxS("XML")},
			{SourceFormat::Registry, wxS("Registry")},
		};
	};
	using SourceFormatValue = KxIndexedEnum::Value<SourceFormatDef, SourceFormat::None>;
}

namespace Kortex::GameConfig
{
	enum class SourceType
	{
		None,
		FSPath,
	};
	struct SourceTypeDef: public KxIndexedEnum::Definition<SourceTypeDef, SourceType, kxf::String, true>
	{
		inline static const TItem ms_Index[] =
		{
			{SourceType::None, wxS("None")},
			{SourceType::FSPath, wxS("FSPath")},
		};
	};
	using SourceTypeValue = KxIndexedEnum::Value<SourceTypeDef, SourceType::None>;
}

namespace Kortex::GameConfig
{
	enum class SortOrderID: uint32_t
	{
		Explicit = 0,
		Ascending,
		Descending
	};
	struct SortOrderDef: public KxIndexedEnum::Definition<SortOrderDef, SortOrderID, kxf::String, true>
	{
		inline static const TItem ms_Index[] =
		{
			{SortOrderID::Explicit, wxS("Explicit")},
			{SortOrderID::Ascending, wxS("Ascending")},
			{SortOrderID::Descending, wxS("Descending")},
		};
	};
	using SortOrderValue = KxIndexedEnum::Value<SortOrderDef, SortOrderID::Explicit>;
}

namespace Kortex::GameConfig
{
	enum class SortOptionsID: uint32_t
	{
		Lexicographical = 0,
		IgnoreCase = 1 << 0,
		DigitsAsNumbers = 1 << 1,
	};
	struct SortOptionsDef: public KxIndexedEnum::Definition<SortOptionsDef, SortOptionsID, kxf::String, false>
	{
		inline static const TItem ms_Index[] =
		{
			{SortOptionsID::Lexicographical, wxS("Lexicographical")},
			{SortOptionsID::IgnoreCase, wxS("IgnoreCase")},
			{SortOptionsID::DigitsAsNumbers, wxS("DigitsAsNumbers")},
		};
	};
	using SortOptionsValue = KxIndexedEnum::Value<SortOptionsDef, SortOptionsID::Lexicographical>;
}

namespace Kortex::GameConfig
{
	enum class SamplesSourceID: uint32_t
	{
		None = 0,
		Range,
		Function,
		ImmediateItems,
	};
	struct SamplesSourceDef: public KxIndexedEnum::Definition<SamplesSourceDef, SamplesSourceID, kxf::String, true>
	{
		inline static const TItem ms_Index[] =
		{
			{SamplesSourceID::None, wxS("None")},
			{SamplesSourceID::Range, wxS("Range")},
			{SamplesSourceID::Function, wxS("Function")},
			{SamplesSourceID::ImmediateItems, wxS("ImmediateItems")},
		};
	};
	using SamplesSourceValue = KxIndexedEnum::Value<SamplesSourceDef, SamplesSourceID::None>;
}

namespace Kortex::GameConfig
{
	enum class EditableBehaviorID: uint32_t
	{
		Auto = 0,
		Inert,
		Selectable,
		Editable,
		EditableIfNoSamples
	};
	struct EditableBehaviorDef: public KxIndexedEnum::Definition<EditableBehaviorDef, EditableBehaviorID, kxf::String, true>
	{
		inline static const TItem ms_Index[] =
		{
			{EditableBehaviorID::Auto, wxS("Auto")},
			{EditableBehaviorID::Inert, wxS("Inert")},
			{EditableBehaviorID::Selectable, wxS("Selectable")},
			{EditableBehaviorID::Editable, wxS("Editable")},
			{EditableBehaviorID::EditableIfNoSamples, wxS("EditableIfNoSamples")},
		};
	};
	using EditableBehaviorValue = KxIndexedEnum::Value<EditableBehaviorDef, EditableBehaviorID::Auto>;
}

namespace Kortex::GameConfig
{
	enum class ColumnID
	{
		Path,
		Name,
		Type,
		Value,
	};
	enum class SerializeFor
	{
		Storage = 0,
		Display
	};
}
