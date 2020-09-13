#pragma once
#include <Kortex/Kortex.hpp>

namespace Kortex
{
	class IModuleInfo
	{
		public:
			virtual ~IModuleInfo() = default;

		public:
			virtual kxf::String GetID() const = 0;
			virtual kxf::String GetName() const = 0;
			virtual kxf::Version GetVersion() const = 0;
			virtual kxf::ResourceID GetImageID() const = 0;
	};
}

namespace Kortex
{
	class SimpleModuleInfo: public IModuleInfo
	{
		private:
			kxf::String m_ID;
			kxf::String m_Name;
			kxf::Version m_Version;
			kxf::ResourceID m_ImageID;

		public:
			SimpleModuleInfo(const kxf::String& id, const kxf::String& name, const kxf::Version& version, const kxf::ResourceID& imageID)
				:m_ID(id), m_Name(name), m_Version(version), m_ImageID(imageID)
			{
			}

		public:
			kxf::String GetID() const override
			{
				return m_ID;
			}
			kxf::String GetName() const override;
			kxf::Version GetVersion() const override
			{
				return m_Version;
			}
			kxf::ResourceID GetImageID() const override
			{
				return m_ImageID;
			}
	};
}
