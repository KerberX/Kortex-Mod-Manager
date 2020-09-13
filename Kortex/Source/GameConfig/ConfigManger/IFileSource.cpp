#include "stdafx.h"
#include "IFileSource.h"
#include <Kortex/ModManager.hpp>

namespace Kortex::GameConfig
{
	kxf::String IFileSource::ResolveFSLocation(const kxf::String& path) const
	{
		return IModDispatcher::GetInstance()->ResolveLocationPath(path);
	}

	kxf::String IFileSource::GetExpandedFileName() const
	{
		return KVarExp(GetFileName());
	}
	kxf::String IFileSource::GetResolvedFilePath() const
	{
		return ResolveFSLocation(KVarExp(GetFilePath()));
	}
}
