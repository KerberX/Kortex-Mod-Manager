#include "stdafx.h"
#include "Item.h"
#include <Kortex/NetworkManager.hpp>

namespace Kortex
{
	bool ModSourceItem::IsOK() const
	{
		// Items with known mod network are valid if both mod network and mod ID are valid.
		// Unknown items are valid if at least name is present.
		if (HasModNetwork())
		{
			return HasModInfo();
		}
		else
		{
			return HasName();
		}
	}
	bool ModSourceItem::IsEmptyValue() const
	{
		return !HasModInfo() && !HasURI();
	}

	void ModSourceItem::Load(const kxf::XMLNode& node)
	{
		// Load name. This function will automatically set IModNetwork instance if it exist
		SetName(node.GetAttribute(wxS("Name")));

		// Load source data
		if (HasModNetwork())
		{
			NetworkModInfo modInfo(node.GetAttributeInt(wxS("ModID"), ModID().GetValue()), node.GetAttributeInt(wxS("FileID"), ModFileID().GetValue()));
			if (!modInfo.IsEmpty())
			{
				m_Data = std::move(modInfo);
			}
		}
		else if (kxf::String uri = node.GetAttribute(wxS("URI")); !uri.IsEmpty())
		{
			m_Data = uri;
		}
		else if (kxf::String url = node.GetAttribute(wxS("URL")); !url.IsEmpty())
		{
			m_Data = url;
		}
	}
	void ModSourceItem::Save(kxf::XMLNode& node) const
	{
		// Save name
		node.SetAttribute(wxS("Name"), GetName());

		// Save actual source info. As mod and file IDs for known sources (represented by IModNetwork instance)
		// and as web-address for everything else
		if (NetworkModInfo modInfo; TryGetModInfo(modInfo))
		{
			node.SetAttribute(wxS("ModID"), modInfo.GetModID().GetValue());
			if (modInfo.HasFileID())
			{
				node.SetAttribute(wxS("FileID"), modInfo.GetFileID().GetValue());
			}
		}
		else if (KxURI uri; TryGetURI(uri))
		{
			node.SetAttribute(wxS("URI"), uri.BuildUnescapedURI());
		}
	}

	IModNetwork* ModSourceItem::GetModNetwork() const
	{
		if (auto modNetwork = std::get_if<IModNetwork*>(&m_ID))
		{
			return *modNetwork;
		}
		else if (const kxf::String* name = std::get_if<kxf::String>(&m_ID))
		{
			return INetworkManager::GetInstance()->GetModNetworkByName(*name);
		}
		return nullptr;
	}
	
	kxf::String ModSourceItem::GetName() const
	{
		if (auto modNetwork = std::get_if<IModNetwork*>(&m_ID); modNetwork && *modNetwork)
		{
			return (*modNetwork)->GetName();
		}
		else if (const kxf::String* name = std::get_if<kxf::String>(&m_ID))
		{
			return *name;
		}
		return {};
	}
	void ModSourceItem::SetName(const kxf::String& name)
	{
		if (IModNetwork* modNetwork = INetworkManager::GetInstance()->GetModNetworkByName(name))
		{
			m_ID = modNetwork;
		}
		else
		{
			m_ID = name;
		}
	}

	KxURI ModSourceItem::GetURI(const GameID& gameID) const
	{
		IModNetwork* modNetwork = nullptr;
		if (const KxURI* uri = std::get_if<KxURI>(&m_Data); uri && uri->IsOk())
		{
			return *uri;
		}
		else if (NetworkModInfo modInfo; TryGetModInfo(modInfo) && TryGetModNetwork(modNetwork))
		{
			return modNetwork->GetModPageURI(modInfo);
		}
		return {};
	}
	NetworkModInfo ModSourceItem::GetModInfo() const
	{
		if (const NetworkModInfo* modInfo = std::get_if<NetworkModInfo>(&m_Data))
		{
			return *modInfo;
		}
		return {};
	}
}
