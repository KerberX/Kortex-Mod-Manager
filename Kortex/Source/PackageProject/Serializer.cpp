#include "stdafx.h"
#include "Serializer.h"
#include <Kortex/ModManager.hpp>
#include <Kortex/NetworkManager.hpp>
#include "Network/ModNetwork/Nexus.h"
#include "Network/ModNetwork/LoversLab.h"
#include "Network/ModNetwork/TESALL.h"
#include <KxFramework/KxTextFile.h>
#include <KxFramework/KxURI.h>

namespace Kortex::PackageProject
{
	ModSourceItem Serializer::TryParseWebSite(const kxf::String& url, kxf::String* domainNameOut)
	{
		using namespace NetworkManager;
	
		long long id = -1;
		IModNetwork* modNetwork = nullptr;
	
		// https://regex101.com
		kxf::String regEx = kxf::String::FromUTF8Unchecked(u8R"((?:http:\/\/)?(?:https:\/\/)?(?:[^@\n]+@)?(?:www\.)?([^:\/\n]+)(?:.*\/)(?:[^\d]+)(\d+))");
		wxRegEx reURL(regEx, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
		if (reURL.Matches(url))
		{
			// Site name
			kxf::String siteName = KxURI(url).GetServer();
			if (siteName == "tesall.ru")
			{
				modNetwork = TESALLModNetwork::GetInstance();
			}
			else if (siteName == "nexusmods.com" || siteName.AfterFirst('.') == "nexusmods.com" || siteName.Contains("nexus"))
			{
				modNetwork = NexusModNetwork::GetInstance();
			}
			else if (siteName == "loverslab.com")
			{
				modNetwork = LoversLabModNetwork::GetInstance();
			}
			KxUtility::SetIfNotNull(domainNameOut, siteName);
	
			// ID
			reURL.GetMatch(url, 2).ToLongLong(&id);
		}
	
		if (modNetwork)
		{
			return ModSourceItem(modNetwork->GetName(), ModID(id));
		}
		return {};
	}
	kxf::String Serializer::ConvertBBCode(const kxf::String& bbSource)
	{
		kxf::String copy = bbSource;
		NetworkManager::NexusModNetwork::GetInstance()->ConvertDescriptionText(copy);
		return copy;
	}
	kxf::String Serializer::PathNameToPackage(const kxf::String& pathName, ContentType type) const
	{
		switch (type)
		{
			case ContentType::Images:
			{
				kxf::String name = pathName.AfterLast('\\');
				return m_PackageDataRoot + "\\Images\\" + name;
			}
			case ContentType::Documents:
			{
				kxf::String name = pathName.AfterLast('\\');
				return m_PackageDataRoot + "\\Documents\\" + name;
			}
			case ContentType::FileData:
			{
				return pathName;
			}
		};
		return wxEmptyString;
	}
	bool Serializer::CheckTag(const kxf::String& tagName) const
	{
		return IModTagManager::GetInstance()->FindTagByName(tagName) != nullptr;
	}
}
