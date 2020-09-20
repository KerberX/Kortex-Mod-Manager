#include "stdafx.h"
#include "IModNetwork.h"
#include <Kortex/NetworkManager.hpp>
#include "GameMods/IGameMod.h"
#include "Utility/Common.h"
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/KxCredentialsDialog.h>

namespace Kortex
{
	kxf::ResourceID IModNetwork::GetGenericIcon()
	{
		return ImageResourceID::ModNetwork_Unknown;
	}

	void IModNetwork::DoOnInit()
	{
		KxFile(GetCacheDirectory()).CreateFolder();
		OnInit();
	}
	void IModNetwork::DoOnExit()
	{
		OnExit();
		RemoveAllComponents();
	}

	KxURI IModNetwork::GetIPBModPageURI(ModID modID, const kxf::String& modSignature) const
	{
		return kxf::String::Format(wxS("%1/%2-%3"), GetModPageBaseURI().BuildUnescapedURI(), modID.GetValue(), modSignature.IsEmpty() ? wxS("x") : modSignature);
	}

	bool IModNetwork::IsDefault() const
	{
		return this == INetworkManager::GetInstance()->GetDefaultModNetwork();
	}
	kxf::String IModNetwork::GetCacheDirectory() const
	{
		return INetworkManager::GetInstance()->GetCacheDirectory() + wxS('\\') + Utility::MakeSafeFileName(GetName());
	}
	kxf::String IModNetwork::GetLocationInCache(const kxf::String& relativePath) const
	{
		return GetCacheDirectory() + wxS('\\') + relativePath;
	}

	KxURI IModNetwork::GetModPageURI(const IGameMod& mod) const
	{
		if (const ModSourceItem* item = mod.GetModSourceStore().GetItem(*this))
		{
			ModRepositoryRequest request(item->GetModInfo());
			request.SetExtraInfo(mod.GetSignature());
			return GetModPageURI(request);
		}
		return {};
	}
	KxURI IModNetwork::GetModPageURI(const DownloadItem& download) const
	{
		if (download.IsOK())
		{
			return GetModPageURI(download.GetNetworkModInfo());
		}
		return {};
	}
}
