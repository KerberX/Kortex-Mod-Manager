#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IThemeManager.h"

namespace Kortex::Theme
{
	class Default: public IThemeManager
	{
		private:
			void AsWindow(wxWindow* window);

		public:
			KxColor GetColor(ColorIndex index, ColorFlag flags = ColorFlag::None) const override;

			void Apply(wxWindow* window) override;
			void Apply(IWorkspace* window) override;
			void Apply(IMainWindow* window) override;
			void Apply(kxf::UI::SplitterWindow* window) override;
			void Apply(kxf::UI::AuiToolBar* window) override;
			void Apply(kxf::UI::StatusBarEx* window, bool isActive) override;
	};
}
