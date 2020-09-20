#include "stdafx.h"
#include "VisualStudio.h"
#include <Kortex/Theme.hpp>
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxSystemSettings.h>
#include <kxf::UI::Framework/kxf::UI::SplitterWindow.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/kxf::UI::StatusBarEx.h>

namespace Kortex::Theme
{
	void VisualStudio::AsWindow(wxWindow* window)
	{
		window->SetBackgroundColour(GetColor(ColorIndex::Window));
		window->SetForegroundColour(GetColor(ColorIndex::Window, ColorFlag::Foreground));
	}

	KxColor VisualStudio::GetColor(ColorIndex index, ColorFlag flags) const
	{
		switch (index)
		{
			case ColorIndex::MainWindow:
			{
				if (flags & ColorFlag::Foreground)
				{
					return {255, 255, 255};
				}
				else
				{
					return {93, 107, 153};
				}
			}
			case ColorIndex::Window:
			{
				if (flags & ColorFlag::Foreground)
				{
					return {255, 255, 255};
				}
				else
				{
					return {204, 213, 240};
				}
			}
			case ColorIndex::SplitterSash:
			{
				return {47, 54, 77};
			}
			case ColorIndex::ToolBar:
			{
				if (flags & ColorFlag::Foreground)
				{
					return {0, 0, 0};
				}
				else
				{
					return {204, 213, 240};
				}
			}
			case ColorIndex::StatusBar:
			{
				if (flags & ColorFlag::Foreground)
				{
					return {255, 255, 255};
				}
				else
				{
					if (flags & ColorFlag::Active)
					{
						return {162, 75, 64};
					}
					else
					{
						return {64, 80, 141};
					}
				}
			}
			case ColorIndex::Caption:
			{
				if (flags & ColorFlag::Foreground)
				{
					if (flags & ColorFlag::Active)
					{
						return {0, 0, 0};
					}
					else
					{
						return {255, 255, 255};
					}
				}
				else
				{
					if (flags & ColorFlag::Active)
					{
						return {245, 204, 132};
					}
					else
					{
						return {64, 86, 141};
					}
				}
			}
			case ColorIndex::Border:
			{
				return KxSystemSettings::GetColor(wxSYS_COLOUR_ACTIVEBORDER);
			}
		};
		return wxNullColour;
	}

	void VisualStudio::Apply(wxWindow* window)
	{
		wxWindowUpdateLocker redrawLock(window);
		AsWindow(window);
	}
	void VisualStudio::Apply(IWorkspace* workspace)
	{
		wxWindow* window = &workspace->GetWindow();
		wxWindowUpdateLocker redrawLock(window);

		AsWindow(window);
	}
	void VisualStudio::Apply(IMainWindow* mainWindow)
	{
		wxWindow* window = &mainWindow->GetFrame();
		wxWindowUpdateLocker redrawLock(window);

		window->SetBackgroundColour(GetColor(ColorIndex::MainWindow, ColorFlag::Background));
		window->SetForegroundColour(GetColor(ColorIndex::MainWindow, ColorFlag::Foreground));
	}
	void VisualStudio::Apply(kxf::UI::SplitterWindow* window)
	{
		wxWindowUpdateLocker redrawLock(window);

		AsWindow(window);
		window->SetSashColor(window->GetParent()->GetBackgroundColour());
	}
	void VisualStudio::Apply(kxf::UI::AuiToolBar* window)
	{
		wxWindowUpdateLocker redrawLock(window);

		window->SetBorderColor(GetColor(ColorIndex::Border));
		window->SetForegroundColour(GetColor(ColorIndex::ToolBar, ColorFlag::Foreground));
		window->SetBackgroundColour(GetColor(ColorIndex::ToolBar, ColorFlag::Background));
	}
	void VisualStudio::Apply(kxf::UI::StatusBarEx* window, bool isActive)
	{
		wxWindowUpdateLocker redrawLock(window);
		const ColorFlag flags = isActive ? ColorFlag::Active : ColorFlag::None;

		window->SetSeparatorsVisible(false);
		window->SetBorderColor(wxNullColour);
		window->SetBackgroundColour(GetColor(ColorIndex::StatusBar, flags|ColorFlag::Background));
		window->SetForegroundColour(GetColor(ColorIndex::StatusBar, flags|ColorFlag::Foreground));
	}
}
