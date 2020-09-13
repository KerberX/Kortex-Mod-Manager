#pragma once
#include <Kortex/Kortex.hpp>

// TODO: These classes aren't really needed. Replace them with something better or remove them.
namespace Kortex::Utility
{
	class WithID
	{
		private:
			kxf::String m_ID;

		public:
			WithID(const kxf::String& id = wxEmptyString)
				:m_ID(id)
			{
			}
			virtual ~WithID() = default;

		public:
			bool IsEmptyID() const
			{
				return m_ID.IsEmpty();
			}
			const kxf::String& RawGetID() const
			{
				return m_ID;
			}
			
			virtual const kxf::String& GetID() const
			{
				return m_ID;
			}
			void SetID(const kxf::String& id)
			{
				m_ID = id;
			}
	};

	class WithName
	{
		private:
			kxf::String m_Name;

		public:
			WithName(const kxf::String& name = wxEmptyString)
				:m_Name(name)
			{
			}
			virtual ~WithName() = default;

		public:
			bool IsEmptyName() const
			{
				return m_Name.IsEmpty();
			}
			const kxf::String& RawGetName() const
			{
				return m_Name;
			}
			
			virtual const kxf::String& GetName() const
			{
				return m_Name;
			}
			void SetName(const kxf::String& value)
			{
				m_Name = value;
			}
	};

	// Tiny class which stores strings name and ID.
	// Its important feature that for ID request it will return name if ID is empty and
	// return for name request will return ID if name is empty.
	// If this is undesirable you can query 'raw' attributes.
	class WithNameAndID: public WithID, public WithName
	{
		public:
			WithNameAndID(const kxf::String& id = wxEmptyString, const kxf::String& name = wxEmptyString)
				:WithID(id), WithName(name)
			{
			}

		public:
			virtual const kxf::String& GetID() const override
			{
				return IsEmptyID() ? RawGetName() : RawGetID();
			}
			virtual const kxf::String& GetName() const override
			{
				return IsEmptyName() ? RawGetID() : RawGetName();
			}
	};
}