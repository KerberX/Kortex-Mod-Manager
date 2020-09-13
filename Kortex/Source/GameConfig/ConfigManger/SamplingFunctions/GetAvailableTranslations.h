#pragma once
#include <Kortex/Kortex.hpp>
#include "GameConfig/ConfigManger/ISamplingFunction.h"

namespace Kortex::GameConfig::SamplingFunction
{
	class GetAvailableTranslations: public ISamplingFunction
	{
		private:
			SampleValue::Vector& m_Values;

		protected:
			void OnCall(const ItemValue::Vector& arguments) override;
			
		public:
			GetAvailableTranslations(SampleValue::Vector& values)
				:m_Values(values)
			{
			}
	};
}
