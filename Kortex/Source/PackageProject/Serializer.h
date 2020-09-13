#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include <Kortex/ModManager.hpp>
#include "Network/ModSourceStore.h"

namespace Kortex
{
	class ModPackageProject;
}

namespace Kortex::PackageProject
{
	class Serializer
	{
		public:
			static const kxf::String& GetDefaultFOModRoot()
			{
				static const kxf::String root = "FOMod";
				return root;
			}
			static const kxf::String& GetDefaultKMPRoot()
			{
				static const kxf::String root = "KortexPackage";
				return root;
			}
			
			static ModSourceItem TryParseWebSite(const kxf::String& url, kxf::String* domainNameOut = nullptr);
			static kxf::String ConvertBBCode(const kxf::String& bbSource);
	
		private:
			kxf::String m_PackageDataRoot;
			
		protected:
			kxf::String PathNameToPackage(const kxf::String& pathName, ContentType type) const;
			bool CheckTag(const kxf::String& tagName) const;
			
		public:
			virtual ~Serializer() = default;
			
		public:
			virtual void Serialize(const ModPackageProject& project) = 0;
			virtual void Structurize(ModPackageProject& project) = 0;
			
			const kxf::String& GetPackageDataRoot(const kxf::String& path) const
			{
				return m_PackageDataRoot;
			}
			void SetPackageDataRoot(const kxf::String& path)
			{
				m_PackageDataRoot = path;
			}
	};
}
