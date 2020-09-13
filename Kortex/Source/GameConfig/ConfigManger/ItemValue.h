#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "DataType.h"

namespace Kortex::GameConfig
{
	class Item;
	class ItemOptions;

	class ItemValue
	{
		public:
			using Vector = std::vector<ItemValue>;

		private:
			wxAny m_Value;
			TypeID m_Type;

		private:
			void DoDeserialize(const kxf::String& stringValue, const Item& item);
			void DeserializeAsBool(TypeID inputType, const kxf::String& stringValue);
			void DeserializeAsSignedInteger(TypeID inputType, const kxf::String& stringValue);
			void DeserializeAsUnsignedInteger(TypeID inputType, const kxf::String& stringValue);
			void DeserializeAsFloat(TypeID inputType, const kxf::String& stringValue);
			void DeserializeAsString(TypeID inputType, const kxf::String& stringValue);

			kxf::String DoSerialize(const Item& item, SerializeFor mode) const;
			kxf::String SerializeFromBool(TypeID outputType, const Item& item, SerializeFor mode) const;
			kxf::String SerializeFromSignedInteger(TypeID outputType, const Item& item) const;
			kxf::String SerializeFromUnsignedInteger(TypeID outputType, const Item& item) const;
			kxf::String SerializeFromFloat(TypeID outputType, const Item& item) const;
			kxf::String SerializeFromString(TypeID outputType, const Item& item) const;

		public:
			ItemValue(TypeID type = {})
				:m_Type(type)
			{
			}

			template<class T> explicit ItemValue(const T& value)
			{
				Assign(value);
			}
			explicit ItemValue(const wxAny& value)
			{
				Assign(value);
			}
			explicit ItemValue(wxAny&& value)
			{
				Assign(std::move(value));
			}

			ItemValue(ItemValue&& other) noexcept
			{
				*this = std::move(other);
			}
			ItemValue(const ItemValue& other)
			{
				*this = other;
			}

		public:
			TypeID GetType() const
			{
				return m_Type;
			}
			void SetType(TypeID type)
			{
				m_Type = type;
			}

			kxf::String Serialize(const Item& item, SerializeFor mode = SerializeFor::Storage) const;
			bool Deserialize(const kxf::String& value, const Item& item);

			bool IsNull() const
			{
				return m_Value.IsNull();
			}
			void MakeNull()
			{
				m_Value.MakeNull();
			}

			template<class T> bool As(T& value) const
			{
				return m_Value.GetAs(&value);
			}
			template<> bool As(wxAny& value) const = delete;
			bool AsAny(wxAny& value) const
			{
				value = m_Value;
				return true;
			}

			template<class T> T As() const
			{
				static_assert(std::is_default_constructible_v<T>);

				T value;
				if (m_Value.GetAs(&value))
				{
					return value;
				}
				return {};
			}
			template<> wxAny As() const = delete;
			const wxAny& AsAny() const
			{
				return m_Value;
			}

			template<class T> bool Assign(T&& value)
			{
				m_Value = value;
				m_Type = TypeID::GetByCType<std::decay_t<T>>();
				return !IsNull();
			}
			template<> bool Assign(const wxAny& value)
			{
				m_Value = value;
				m_Type = DataTypeID::Any;
				return !IsNull();
			}
			template<> bool Assign(wxAny&& value)
			{
				m_Value = std::move(value);
				m_Type = DataTypeID::Any;

				value.MakeNull();
				return !IsNull();
			}
			
		public:
			ItemValue& operator=(ItemValue&& other) noexcept
			{
				Assign(std::move(other.m_Value));

				m_Type = other.m_Type;
				other.m_Type = DataTypeID::None;

				return *this;
			}
			ItemValue& operator=(const ItemValue& other)
			{
				Assign(other.m_Value);
				m_Type = other.m_Type;

				return *this;
			}
	};
}
