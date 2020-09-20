#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/Resources/ImageResourceID.h"
#include <kxf::UI::Framework/kxf::Version.h>
#include <kxf::UI::Framework/KxURI.h>

namespace Kortex
{
	class IModule;
}

namespace Kortex::Application::About
{
	class INode
	{
		protected:
			class LicenseData
			{
				friend class INode;

				private:
					kxf::String m_License;
					bool m_ShouldLoad = true;

				public:
					operator const kxf::String&() const
					{
						return m_License;
					}
			};

		public:
			using Vector = std::vector<std::unique_ptr<INode>>;

		public:
			enum class Type
			{
				Application,
				Software,
				Resource,
			};

		protected:
			kxf::String GetLocation(Type type) const;
			kxf::String ReadLicense(Type type) const;
			const kxf::String& LoadLicense(LicenseData& data, Type type) const;

		public:
			virtual ~INode() = default;

		public:
			virtual kxf::String GetName() const = 0;
			virtual kxf::Version GetVersion() const = 0;
			virtual kxf::ResourceID GetIconID() const = 0;
			virtual KxURI GetURI() const = 0;

			virtual bool HasLicense() const = 0;
			virtual kxf::String GetLicense() const = 0;
	};
}

namespace Kortex::Application::About
{
	class AppNode: public INode
	{
		private:
			mutable LicenseData m_Licence;

		public:
			kxf::String GetName() const override;
			kxf::Version GetVersion() const override;
			kxf::ResourceID GetIconID() const override;
			KxURI GetURI() const override;

			bool HasLicense() const override;
			kxf::String GetLicense() const override;

	};
}

namespace Kortex::Application::About
{
	class ModuleNode: public INode
	{
		private:
			const IModule& m_Module;
			mutable LicenseData m_Licence;

		public:
			ModuleNode(const IModule& module)
				:m_Module(module)
			{
			}

		public:
			kxf::String GetName() const override;
			kxf::Version GetVersion() const override;
			kxf::ResourceID GetIconID() const override;
			KxURI GetURI() const override
			{
				return {};
			}
			
			bool HasLicense() const override;
			kxf::String GetLicense() const override;
	};
}

namespace Kortex::Application::About
{
	class GenericNode: public INode
	{
		private:
			const Type m_Type;
			const kxf::String m_Name;
			const kxf::Version m_Version;
			const KxURI m_URL;
			const kxf::ResourceID m_IconID;
			mutable LicenseData m_Licence;

		public:
			GenericNode(Type type, const kxf::String& name, const kxf::Version& version, const kxf::String& url, const kxf::ResourceID& iconID)
				:m_Type(type), m_Name(name), m_Version(version), m_URL(url), m_IconID(iconID)
			{
			}

		public:
			kxf::String GetName() const override
			{
				return m_Name;
			}
			kxf::Version GetVersion() const override
			{
				return m_Version;
			}
			kxf::ResourceID GetIconID() const override
			{
				return m_IconID;
			}
			KxURI GetURI() const override
			{
				return m_URL;
			}
			
			bool HasLicense() const override
			{
				return !LoadLicense(m_Licence, m_Type).IsEmpty();
			}
			kxf::String GetLicense() const override
			{
				return LoadLicense(m_Licence, m_Type);
			}
	};
}

namespace Kortex::Application::About
{
	class SoftwareNode: public GenericNode
	{
		public:
			SoftwareNode(const kxf::String& name, const kxf::Version& version, const kxf::String& url, const kxf::ResourceID& iconID = ImageResourceID::None)
				:GenericNode(Type::Software, name, version, url, iconID)
			{
			}
	};
}

namespace Kortex::Application::About
{
	class ResourceNode: public INode
	{
		private:
			const kxf::String m_Name;
			const KxURI m_URL;
			mutable LicenseData m_Licence;

		public:
			ResourceNode(const kxf::String& name, const kxf::String& url)
				:m_Name(name), m_URL(url)
			{
			}

		public:
			kxf::String GetName() const override
			{
				return m_Name;
			}
			kxf::Version GetVersion() const override
			{
				return kxf::Version();
			}
			kxf::ResourceID GetIconID() const override
			{
				return {};
			}
			KxURI GetURI() const override
			{
				return m_URL;
			}
			
			bool HasLicense() const override
			{
				return !LoadLicense(m_Licence, Type::Resource).IsEmpty();
			}
			kxf::String GetLicense() const override
			{
				return LoadLicense(m_Licence, Type::Resource);
			}
	};
}
