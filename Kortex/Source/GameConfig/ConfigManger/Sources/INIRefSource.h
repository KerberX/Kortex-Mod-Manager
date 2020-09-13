#pragma once
#include <Kortex/Kortex.hpp>
#include "GameConfig/ConfigManger/ISource.h"
#include <KxFramework/KxINI.h>

namespace Kortex::GameConfig
{
	class INIRefSource: public ISource
	{
		private:
			KxINI& m_INI;
			bool m_IsOpened = false;

		protected:
			const KxINI& GetINI() const
			{
				return m_INI;
			}
			KxINI& GetINI()
			{
				return m_INI;
			}

		public:
			INIRefSource(KxINI& ini)
				:m_INI(ini)
			{
			}

		public:
			// ISource
			SourceFormatValue GetFormat() const override
			{
				return SourceFormat::INI;
			}
			kxf::String GetPathDescription() const override
			{
				return {};
			}

			bool IsOpened() const override
			{
				return m_IsOpened;
			}
			bool Open() override;
			bool Save() override;
			void Close() override;

			bool WriteValue(const Item& item, const ItemValue& value) override;
			bool ReadValue(Item& item, ItemValue& value) const override;
			void LoadUnknownItems(ItemGroup& group) override;
	};
}
