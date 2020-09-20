#include "stdafx.h"
#include <Kortex/NetworkManager.hpp>
#include "LoversLab.h"

namespace Kortex::NetworkManager
{
	LoversLabModNetwork::LoversLabModNetwork()
	{
	}

	kxf::String LoversLabModNetwork::GetAPIURL() const
	{
		return wxS("https://www.loverslab.com/api");
	}

	kxf::ResourceID LoversLabModNetwork::GetIcon() const
	{
		return ImageResourceID::ModNetwork_LoversLab;
	}
	kxf::String LoversLabModNetwork::GetName() const
	{
		return wxS("LoversLab");
	}

	KxURI LoversLabModNetwork::GetModPageBaseURI(const GameID& id) const
	{
		return wxS("https://www.loverslab.com/files/file");
	}
	KxURI LoversLabModNetwork::GetModPageURI(const ModRepositoryRequest& request) const
	{
		return GetIPBModPageURI(request.GetModID(), request.GetExtraInfo<kxf::String>());
	}
}
