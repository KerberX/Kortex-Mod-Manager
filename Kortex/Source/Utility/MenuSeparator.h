#pragma once
#include <Kortex/Kortex.hpp>
class kxf::UI::Menu;

namespace Kortex::Utility
{
	class MenuSeparator
	{
		public:
			enum class Where
			{
				Before,
				After,
			};

		private:
			kxf::UI::Menu& m_Menu;
			const size_t m_ItemCount = 0;
			const Where m_Where = Where::After;

		public:
			MenuSeparator(kxf::UI::Menu& menu, Where insertWhere);
			MenuSeparator(const MenuSeparator&) = delete;
			~MenuSeparator();

		public:
			MenuSeparator& operator=(const MenuSeparator&) = delete;
	};
}

namespace Kortex::Utility
{
	class MenuSeparatorBefore: public MenuSeparator
	{
		public:
			MenuSeparatorBefore(kxf::UI::Menu& menu)
				:MenuSeparator(menu, Where::Before)
			{
			}
	};

	class MenuSeparatorAfter: public MenuSeparator
	{
		public:
			MenuSeparatorAfter(kxf::UI::Menu& menu)
				:MenuSeparator(menu, Where::After)
			{
			}
	};
}
