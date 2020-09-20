#pragma once
#include <Kortex/Kortex.hpp>

class wxTopLevelWindow;
namespace kxf::UI
{
	class SplitterWindow;

	namespace DataView
	{
		class View;
	}
}

namespace KxDataView2
{
	class View;
}

namespace Kortex
{
	class AppOption;
	class IWorkspaceContainer;
}

namespace Kortex::Application::OptionSerializer
{
	enum class SerializationMode
	{
		Save,
		Load
	};
}

namespace Kortex::Application::OptionSerializer
{
	class UILayout
	{
		public:
			static void DataViewLayout(AppOption& option, SerializationMode mode, kxf::UI::DataView::View* dataView);
			static void SplitterLayout(AppOption& option, SerializationMode mode, kxf::UI::SplitterWindow* window);
			static void WorkspaceContainerLayout(AppOption& option, SerializationMode mode, IWorkspaceContainer& container);
			static void WindowGeometry(AppOption& option, SerializationMode mode, wxTopLevelWindow* window);
	};
}
