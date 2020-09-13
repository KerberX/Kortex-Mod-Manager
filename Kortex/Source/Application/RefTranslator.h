#pragma once
#include <Kortex/Kortex.hpp>
#include "ITranslator.h"

namespace Kortex
{
	class RefTranslator: public ITranslator
	{
		private:
			kxf::optional_ptr<kxf::ILocalizationPackage> m_TranslationRef;

		protected:
			std::optional<kxf::String> DoGetString(const kxf::String& id) const override;
			std::optional<kxf::String> DoGetString(kxf::StdID id) const override;
			
		public:
			RefTranslator(const kxf::ILocalizationPackage& translation)
				:m_TranslationRef(translation)
			{
			}

		public:
			const kxf::ILocalizationPackage& GetTranslation() const
			{
				return *m_TranslationRef;
			}
	};
}
