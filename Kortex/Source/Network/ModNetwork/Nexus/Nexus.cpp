#include "stdafx.h"
#include "Nexus.h"
#include <Kortex/NetworkManager.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/Application.hpp>
#include "Utility/String.h"
#include <kxf::UI::Framework/KxCURL.h>
#include <kxf::UI::Framework/KxJSON.h>
#include <kxf::UI::Framework/KxShell.h>
#include <kxf::UI::Framework/KxSystem.h>
#include <kxf::UI::Framework/KxString.h>
#include <kxf::UI::Framework/KxComparator.h>
#include <kxf::UI::Framework/KxIndexedEnum.h>
#include <kxf::UI::Framework/KxWebSocket.h>
#include <Kx/Async/DelayedCall.h>

namespace Kortex::Variables
{
	constexpr auto NexusDomainName = wxS("NexusDomainName");
}

namespace Kortex::NetworkManager
{
	void NexusModNetwork::OnAuthenticated()
	{
		if (m_UpdateChecker.CanIssueNewAutomaticCheck())
		{
			KxAsync::DelayedCall([this]()
			{
				m_UpdateChecker.RunUpdateCheck();
			}, wxTimeSpan::Seconds(3));
		}
	}

	std::unique_ptr<KxCURLSession> NexusModNetwork::NewCURLSession(const kxf::String& address, const kxf::String& apiKey) const
	{
		auto session = INetworkManager::GetInstance()->NewCURLSession(address);

		// Add Nexus specific headers
		session->AddHeader(wxS("APIKey"), apiKey.IsEmpty() ? GetAPIKey() : apiKey);
		session->AddHeader(wxS("Content-Type"), wxS("application/json"));
		session->AddHeader(wxS("Protocol-Version"), wxS("0.15.5"));

		// Allow repository component to read query limits info from response headers
		session->Bind(KxEVT_CURL_RESPONSE_HEADER, &NexusRepository::OnResponseHeader, const_cast<NexusRepository*>(&m_Repository));

		return session;
	}
	kxf::String NexusModNetwork::GetAPIURL() const
	{
		return wxS("https://api.nexusmods.com/v1");
	}
	kxf::String NexusModNetwork::GetAPIKey() const
	{
		if (const NexusValidationReply* info = m_Auth.GetLastValidationReply())
		{
			return info->APIKey;
		}
		return {};
	}
	
	void NexusModNetwork::OnInit()
	{
	}
	void NexusModNetwork::OnExit()
	{
	}
	void NexusModNetwork::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& networkNode)
	{
		m_UpdateChecker.OnLoadInstance(instance, networkNode.GetFirstChildElement(wxS("UpdateChecker")));
	}

	NexusModNetwork::NexusModNetwork()
		:m_Utility(*this),
		m_Auth(*this, m_Utility),
		m_Repository(*this, m_Utility, m_Auth),
		m_UpdateChecker(*this, m_Utility, m_Repository)
	{
		AddComponent(m_Utility);
		AddComponent<ModNetworkAuth>(m_Auth);
		AddComponent<ModNetworkRepository>(m_Repository);
		AddComponent<ModNetworkUpdateChecker>(m_UpdateChecker);
	}

	kxf::ResourceID NexusModNetwork::GetIcon() const
	{
		return ImageResourceID::ModNetwork_Nexus;
	}
	kxf::String NexusModNetwork::GetName() const
	{
		return wxS("NexusMods");
	}
	
	kxf::String NexusModNetwork::TranslateGameIDToNetwork(const GameID& id) const
	{
		kxf::String networkID;
		if (id.IsOK())
		{
			for (const auto& instance: IGameInstance::GetShallowInstances())
			{
				if (instance->GetGameID() == id)
				{
					networkID = instance->GetVariables().GetVariable(Variables::NexusDomainName);
					break;
				}
			}
		}
		else
		{
			// If invalid ID is passed, return ID for current game
			networkID = IGameInstance::GetActive()->GetVariables().GetVariable(Variables::NexusDomainName);
		}

		KxString::MakeLower(networkID);
		return networkID;
	}
	GameID NexusModNetwork::TranslateGameIDFromNetwork(const kxf::String& id) const
	{
		if (!id.IsEmpty())
		{
			for (const auto& instance: IGameInstance::GetShallowInstances())
			{
				const IVariableTable& table = instance->GetVariables();
				kxf::String value = table.GetVariable(Variables::NexusDomainName).AsString();
				if (KxComparator::IsEqual(id, value, true))
				{
					return instance->GetGameID();
				}
			}

			// Try names that doesn't match domain names
			auto GetInstanceGameID = [&](const kxf::String& id)
			{
				if (IGameInstance* instance = IGameInstance::GetShallowInstance(id))
				{
					return instance->GetGameID();
				}
				return GameIDs::NullGameID;
			};

			if (KxComparator::IsEqual(id, wxS("SkyrimSE"), true))
			{
				return GetInstanceGameID(GameIDs::SkyrimSE);
			}
			else if (KxComparator::IsEqual(id, wxS("FalloutNV"), true))
			{
				return GetInstanceGameID(GameIDs::FalloutNV);
			}
			// There's also 'TESO' -> 'ElderScrollsOnline' but Kortex doesn't support TESO.
		}
		return {};
	}
	void NexusModNetwork::ConvertDescriptionText(kxf::String& description) const
	{
		auto RAW = [](const auto& s)
		{
			return kxf::String::FromUTF8Unchecked(s);
		};

		// Trimming
		KxString::Trim(description, true, true);

		// Quotes: \" -> " and \' -> '
		description.Replace("\\\"", "\"", true);
		description.Replace("\\'", "'", true);

		// URL: [url=address]text[/url]
		{
			wxRegEx regEx(RAW(u8R"(\[url=([^\]]+)\]([^\[]+)\[\/url\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<a href="\1">\2</a>)"));
		}

		// URL: [url]address[/url]
		{
			wxRegEx regEx(RAW(u8R"(\[url\]([^\[]+)\[\/url\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<a href="\1">\1</a>)"));
		}

		// URL: [NEXUS ID: number]
		{
			wxRegEx regEx(RAW(u8R"(\[NEXUS\s?ID\:\s?(\d+)\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, kxf::String::Format(RAW(u8R"(<a href="%1\/\1">\0</a>)"), GetModPageBaseURI().BuildUnescapedURI()));
		}

		// Image: [img]address[/img]
		{
			wxRegEx regEx(RAW(u8R"(\[img\]([^\[]+)\[\/img\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<img src="\1"/>)"));
		}

		// Align: [left|right|center|justify]text[/left|right|center|justify]
		{
			wxRegEx regEx(RAW(u8R"(\[(left|right|center|justify)\](.*)\[\/(?:left|right|center|justify)\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<div align="\1">\2</div>)"));
		}

		// Font size: [size=string|number]text[/size]
		{
			wxRegEx regEx(RAW(u8R"((?s)\[size=\\?"?(\d+)\\?"?\](.*?)\[\/size\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<font size="\1">\2</font>)"));
		}

		// Font color: [color="name"]text[/color]
		{
			wxRegEx regEx(RAW(u8R"((?s)\[color=\\?"?(\w+|\#\d+)\\?"?\](.*?)\[\/color\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<font color="\1">\2</font>)"));
		}

		// Font color: [color=#AABBCCDD]text[/color]
		{
			wxRegEx regEx(RAW(u8R"(\[color=#([ABCDEF\d]+)\](.+)\[\/color\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<font color="#\1">\2</font>)"));
		}

		// Font color: [color=#AABBCCDD]text[/color]
		{
			wxRegEx regEx(RAW(u8R"((?s)\[list\](.*?)\[\/list\])"), wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			regEx.ReplaceAll(&description, RAW(u8R"(<ul>\1</ul>)"));
		}

		// Simple container tags: [b]text[/b]
		auto ExpSimple = [&description, &RAW](const kxf::String& tagName, const kxf::String& sTagNameRepl = wxEmptyString)
		{
			kxf::String regExQuery = kxf::String::Format(RAW(u8R"((?s)\[%s\](.*?)\[\/%s\])"), tagName, tagName);
			kxf::String repl = kxf::String::Format(RAW(u8R"(<%s>\1</%s>)"), sTagNameRepl, sTagNameRepl);

			wxRegEx regEx(regExQuery, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			if (regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE))
			{
				regEx.ReplaceAll(&description, repl);
			}
		};

		ExpSimple("b", "b");
		ExpSimple("i", "i");
		ExpSimple("u", "u");
		ExpSimple("\\*", "li");

		// Buggy
		if constexpr(false)
		{
			// URL. Matches string starting only with 'http://', 'https://', 'ftp://' or 'www.'.
			// This tries not to match addresses inside <a></a> tags. And this better to be the last replacement.
			wxRegEx regEx(RAW(u8R"((?:[^<a href="">])(http:\/\/|https:\/\/\ftp:\/\/|www.)([\w_-]+(?:(?:\.[\w_-]+)+))([\w.,@?^=%&:\/~+#-]*[\w@?^=%&\/~+#-])?(?:[^<\/a>]))"), wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE);
			if (regEx.Matches(description, wxRE_DEFAULT|wxRE_ADVANCED|wxRE_ICASE|wxRE_NEWLINE))
			{
				regEx.ReplaceAll(&description, RAW(u8R"(<a href="\1\2\3">\0</a>)"));
			}
		}
	}
	
	KxURI NexusModNetwork::GetModPageBaseURI(const GameID& id) const
	{
		return kxf::String::Format(wxS("https://www.nexusmods.com/%1/mods"), TranslateGameIDToNetwork(id)).MakeLower();
	}
	KxURI NexusModNetwork::GetModPageURI(const ModRepositoryRequest& request) const
	{
		return kxf::String::Format(wxS("%1/%2"),
								GetModPageBaseURI(request.GetGameID()).BuildUnescapedURI(),
								request.GetModID().GetValue()
		);
	}

	std::optional<NexusGameReply> NexusModNetwork::GetGameInfo(const GameID& id) const
	{
		auto connection = NewCURLSession(kxf::String::Format("%1/games/%2",
										 GetAPIURL(), 
										 TranslateGameIDToNetwork(id))
		);
		KxCURLReply reply = connection->Send();
		if (m_Utility.TestRequestError(reply, reply))
		{
			return std::nullopt;
		}

		NexusGameReply info;
		try
		{
			KxJSONObject json = KxJSON::Load(reply);
			m_Utility.ReadGameInfo(json, info);
		}
		catch (...)
		{
			return std::nullopt;
		}
		return info;
	}
	std::vector<NexusGameReply> NexusModNetwork::GetGamesList() const
	{
		auto connection = NewCURLSession(kxf::String::Format("%1/games",
										 GetAPIURL())
		);
		KxCURLReply reply = connection->Send();
		if (m_Utility.TestRequestError(reply, reply))
		{
			return {};
		}

		std::vector<NexusGameReply> infoVector;
		try
		{
			KxJSONObject json = KxJSON::Load(reply);
			infoVector.reserve(json.size());

			for (const KxJSONObject& value: json)
			{
				NexusGameReply& info = infoVector.emplace_back();
				m_Utility.ReadGameInfo(value, info);
			}
		}
		catch (...)
		{
			infoVector.clear();
		}
		return infoVector;
	}

	void NexusModNetwork::OnToolBarMenu(kxf::UI::Menu& menu)
	{
		m_Auth.OnToolBarMenu(menu);
		m_Repository.OnToolBarMenu(menu);
	}
}
