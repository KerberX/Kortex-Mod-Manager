#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/Options/Macros.h"

namespace Kortex::CmdLineName
{
	KortexDefOption(InstanceID);
	KortexDefOption(DownloadLink);
	KortexDefOption(GlobalConfigPath);
}

namespace Kortex
{
	struct CmdLine
	{
		kxf::String Executable;
		kxf::String Arguments;
	};
	struct CmdLineParameters
	{
		kxf::String InstanceID;
		kxf::String DownloadLink;
		kxf::String GlobalConfigPath;
	};
}
