#pragma once
#include <Kortex/Kortex.hpp>
#include "GameConfig/ConfigManger/ISamplingFunction.h"

namespace Kortex
{
	class IConfigManager;
}

namespace Kortex::GameConfig::SamplingFunction
{
	class GetVirtualKeys: public ISamplingFunction
	{
		private:
			struct VirtualKeyInfo
			{
				using Map = std::unordered_map<uint64_t, VirtualKeyInfo>;

				kxf::String ID;
				kxf::String Name;
				uint64_t Code = 0;
			};

		private:
			SampleValue::Vector& m_Values;
			const IConfigManager& m_Manager;
			VirtualKeyInfo::Map* m_KeyMap = nullptr;

		protected:
			void LoadVirtualKeys();
			void OnCall(const ItemValue::Vector& arguments) override;
			
		public:
			GetVirtualKeys(SampleValue::Vector& values, const IConfigManager& manager)
				:m_Values(values), m_Manager(manager)
			{
			}
	};
}
