#include "stdafx.h"
#include <Kortex/NetworkManager.hpp>
#include "TESALL.h"

namespace Kortex::NetworkManager
{
	TESALLModNetwork::TESALLModNetwork()
	{
	}

	kxf::ResourceID TESALLModNetwork::GetIcon() const
	{
		return ImageResourceID::ModNetwork_TESALL;
	}
	kxf::String TESALLModNetwork::GetName() const
	{
		return wxS("TESALL.RU");
	}

	KxURI TESALLModNetwork::GetModPageBaseURI(const GameID& id) const
	{
		return wxS("http://tesall.ru/files/file");
	}
	KxURI TESALLModNetwork::GetModPageURI(const ModRepositoryRequest& request) const
	{
		return GetIPBModPageURI(request.GetModID(), request.GetExtraInfo<kxf::String>());
	}
}
