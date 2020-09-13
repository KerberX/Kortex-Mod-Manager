#pragma once
#include <Kortex/Kortex.hpp>
#include "LabeledValue.h"
class KxAuiToolBar;
class KxURI;

namespace Kortex
{
	class kxf::ResourceID;
}

namespace Kortex::Utility::UI
{
	KxAuiToolBarItem* CreateToolBarButton(KxAuiToolBar* toolBar,
										  const kxf::String& label,
										  const kxf::ResourceID& imageID = {},
										  wxItemKind kind = wxITEM_NORMAL,
										  int index = -1);

	// Shows a dialog that asks user to confirm opening a URI in default browser. Returns true if the user has agreed.
	bool AskOpenURL(const KxURI& uri, wxWindow* parent = nullptr);
	bool AskOpenURL(const LabeledValue::Vector& urlList, wxWindow* parent = nullptr);

	// Creates placeholder for KxHTMLWindow to be showed when actual content is unavailable.
	// Window is required if you want correct text color.
	kxf::String MakeHTMLWindowPlaceholder(const kxf::String& text, const wxWindow* window = nullptr);

	bool SetSearchMask(kxf::String& storage, const kxf::String& newMask);
	bool CheckSearchMask(const kxf::String& mask, const kxf::String& string);
}
