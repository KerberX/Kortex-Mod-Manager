#pragma once
#include <Kortex/Kortex.hpp>

namespace Kortex
{
	class IManagerInfo
	{
		public:
			virtual ~IManagerInfo() = default;

		public:
			virtual kxf::String GetID() const = 0;
			virtual kxf::String GetName() const = 0;
	};
}

namespace Kortex
{
	class SimpleManagerInfo: public IManagerInfo
	{
		private:
			kxf::String m_ID;
			kxf::String m_Name;

		public:
			SimpleManagerInfo(const kxf::String& id, const kxf::String& name)
				:m_ID(id), m_Name(name)
			{
			}

		public:
			kxf::String GetID() const override
			{
				return m_ID;
			}
			kxf::String GetName() const override;
	};
}
