#pragma once
#include <Kortex/Kortex.hpp>
#include "Utility/EnumClassOperations.h"

namespace kxf::UI
{
	class SplitterWindow;
	class StatusBarEx;
	class AuiToolBar;
}

namespace Kortex
{
	class IMainWindow;
	class IWorkspace;
}
namespace Kortex::Theme
{
	enum class ColorIndex
	{
		MainWindow,
		Window,
		SplitterSash,
		ToolBar,
		StatusBar,
		Border,
		Caption
	};
	enum class ColorFlag: uint32_t
	{
		None = 0,

		Background = 0,
		Foreground = 1 << 0,

		Default = 0,
		Active = 1 << 1
	};
}
namespace kxf
{
	KxFlagSet_Declare(Kortex::Theme::ColorFlag);
}

namespace Kortex
{
	class IThemeManager
	{
		public:
			using ColorIndex = Theme::ColorIndex;
			using ColorFlag = Theme::ColorFlag;

		public:
			static IThemeManager& GetActive();

		protected:
			void InheritColors(wxWindow* window, const wxWindow* from) const;
			bool IsWin8OrGreater() const;

		public:
			virtual ~IThemeManager() = default;

		public:
			virtual kxf::Color GetColor(ColorIndex index, ColorFlag flags = ColorFlag::None) const = 0;

			virtual void Apply(wxWindow* window) = 0;
			virtual void Apply(IMainWindow* window) = 0;
			virtual void Apply(IWorkspace* window) = 0;
			virtual void Apply(kxf::UI::SplitterWindow* window) = 0;
			virtual void Apply(kxf::UI::AuiToolBar* window) = 0;
			virtual void Apply(kxf::UI::StatusBarEx* window, bool isActive) = 0;
	};
}
