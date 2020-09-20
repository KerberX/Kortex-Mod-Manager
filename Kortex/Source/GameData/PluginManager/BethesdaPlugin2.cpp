#include "stdafx.h"
#include "BethesdaPlugin2.h"
#include <kxf::UI::Framework/KxComparator.h>

namespace Kortex::PluginManager
{
	void BethesdaPlugin2::OnRead(IPluginReader& reader)
	{
		BethesdaPlugin::OnRead(reader);

		const kxf::String ext = GetName().AfterLast(wxS('.'));
		if (KxComparator::IsEqual(ext, wxS("esm")))
		{
			SetMaster(true);
		}
		if (KxComparator::IsEqual(ext, wxS("esl")))
		{
			SetLight(true);
			SetMaster(true);
		}
	}
}
