#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "DataType.h"
class kxf::XMLNode;

namespace Kortex::GameConfig
{
	class ITypeDetector
	{
		public:
			virtual ~ITypeDetector() = default;

		public:
			virtual bool RequiresValueData() const = 0;
			virtual TypeID GetType(const kxf::String& valueName, const kxf::String& valueData) const = 0;
	};
}

namespace Kortex::GameConfig
{
	class HungarianNotationTypeDetector: public ITypeDetector
	{
		private:
			std::unordered_map<kxf::String, TypeID> m_TypeMap;

		public:
			HungarianNotationTypeDetector(const kxf::XMLNode& rootNode);

		public:
			bool RequiresValueData() const override
			{
				return false;
			}
			TypeID GetType(const kxf::String& valueName, const kxf::String& valueData) const override;
	};
}

namespace Kortex::GameConfig
{
	class DataAnalysisTypeDetector: public ITypeDetector
	{
		public:
			bool RequiresValueData() const override
			{
				return true;
			}
			TypeID GetType(const kxf::String& valueName, const kxf::String& valueData) const override;
	};
}
