#include "stdafx.h"
#include "Default.h"
#include <Kortex/Theme.hpp>
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxSystem.h>
#include <kxf::UI::Framework/KxTopLevelWindow.h>
#include <kxf::UI::Framework/KxSystemSettings.h>
#include <kxf::UI::Framework/kxf::UI::SplitterWindow.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/kxf::UI::StatusBarEx.h>

namespace Kortex::Theme
{
	void Default::AsWindow(wxWindow* window)
	{
		window->SetBackgroundColour(GetColor(ColorIndex::Window));
		window->SetForegroundColour(GetColor(ColorIndex::Window, ColorFlag::Foreground));
	}

	KxColor Default::GetColor(ColorIndex index, ColorFlag flags) const
	{
		switch (index)
		{
			case ColorIndex::MainWindow:
			{
				if (flags & ColorFlag::Foreground)
				{
					return KxSystemSettings::GetColor(wxSYS_COLOUR_WINDOWTEXT);
				}
				else
				{
					return KxSystemSettings::GetColor(wxSYS_COLOUR_APPWORKSPACE);
				}
			}
			case ColorIndex::Window:
			{
				if (flags & ColorFlag::Foreground)
				{
					return KxSystemSettings::GetColor(wxSYS_COLOUR_WINDOWTEXT);
				}
				else
				{
					return KxSystemSettings::GetColor(wxSYS_COLOUR_WINDOW);
				}
			}
			case ColorIndex::SplitterSash:
			{
				return KxSystemSettings::GetColor(wxSYS_COLOUR_WINDOW);
			}
			case ColorIndex::ToolBar:
			case ColorIndex::StatusBar:
			{
				if (flags & ColorFlag::Foreground)
				{
					if (flags & ColorFlag::Active)
					{
						return KxSystemSettings::GetColor(wxSYS_COLOUR_HIGHLIGHTTEXT);
					}
					else
					{
						return KxSystemSettings::GetColor(wxSYS_COLOUR_WINDOWTEXT);
					}					
				}
				else
				{
					if (flags & ColorFlag::Active)
					{
						if (IsWin8OrGreater())
						{
							return KxTopLevelWindow<>::DWMGetGlassColor().ChangeLightness(75);
						}
						else
						{
							return KxSystemSettings::GetColor(wxSYS_COLOUR_HOTLIGHT);
						}
					}
					else
					{
						if (IsWin8OrGreater())
						{
							return KxTopLevelWindow<>::DWMGetGlassColor();
						}
						else
						{
							return KxSystemSettings::GetColor(wxSYS_COLOUR_MENUHILIGHT).ChangeLightness(160);
						}
					}
				}
			}
			case ColorIndex::Caption:
			{
				if (flags & ColorFlag::Foreground)
				{
					if (flags & ColorFlag::Active)
					{
						return KxSystemSettings::GetColor(wxSYS_COLOUR_CAPTIONTEXT);
					}
					else
					{
						return KxSystemSettings::GetColor(wxSYS_COLOUR_INACTIVECAPTIONTEXT);
					}
				}
				else
				{
					if (flags & ColorFlag::Active)
					{
						return KxSystemSettings::GetColor(wxSYS_COLOUR_ACTIVECAPTION);
					}
					else
					{
						return KxSystemSettings::GetColor(wxSYS_COLOUR_INACTIVECAPTION);
					}
				}
			}
			case ColorIndex::Border:
			{
				if (flags & ColorFlag::Active)
				{
					return KxSystemSettings::GetColor(wxSYS_COLOUR_ACTIVEBORDER);
				}
				else
				{
					return KxSystemSettings::GetColor(wxSYS_COLOUR_INACTIVEBORDER);
				}
			}
		};
		return wxNullColour;
	}

	void Default::Apply(wxWindow* window)
	{
		wxWindowUpdateLocker redrawLock(window);
		AsWindow(window);
	}
	void Default::Apply(IWorkspace* workspace)
	{
		wxWindow* window = &workspace->GetWindow();
		wxWindowUpdateLocker redrawLock(window);

		AsWindow(window);
	}
	void Default::Apply(IMainWindow* mainWindow)
	{
		wxWindow* window = &mainWindow->GetFrame();
		wxWindowUpdateLocker redrawLock(window);

		window->SetBackgroundColour(GetColor(ColorIndex::MainWindow, ColorFlag::Background));
		window->SetForegroundColour(GetColor(ColorIndex::MainWindow, ColorFlag::Foreground));
	}
	void Default::Apply(kxf::UI::SplitterWindow* window)
	{
		wxWindowUpdateLocker redrawLock(window);
		InheritColors(window, window->GetParent());

		window->SetSashColor(window->GetParent()->GetBackgroundColour());
	}
	void Default::Apply(kxf::UI::AuiToolBar* window)
	{
		wxWindowUpdateLocker redrawLock(window);

		window->SetBorderColor(GetColor(ColorIndex::Border));
		window->SetForegroundColour(GetColor(ColorIndex::ToolBar, ColorFlag::Foreground));
		window->SetBackgroundColour(GetColor(ColorIndex::ToolBar, ColorFlag::Background));
	}
	void Default::Apply(kxf::UI::StatusBarEx* window, bool isActive)
	{
		wxWindowUpdateLocker redrawLock(window);

		const ColorFlag flags = isActive ? ColorFlag::Active : ColorFlag::None;
		window->SetSeparatorsVisible(false);
		window->SetBorderColor(wxNullColour);
		window->SetBackgroundColour(GetColor(ColorIndex::StatusBar, ColorFlag::Background|flags));
		window->SetForegroundColour(GetColor(ColorIndex::StatusBar, ColorFlag::Foreground|flags));
	}
}
