#include "stdafx.h"
#include "DefaultNetworkManager.h"
#include <Kortex/Application.hpp>
#include <Kortex/ApplicationOptions.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/NetworkManager.hpp>
#include "Network/ModNetwork/Nexus.h"
#include "Network/ModNetwork/LoversLab.h"
#include "Network/ModNetwork/TESALL.h"
#include "Utility/MenuSeparator.h"
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/kxf::UI::AuiToolBar.h>
#include <kxf::UI::Framework/KxComparator.h>
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/kxf::UI::Menu.h>

namespace
{
	bool IsDefaultModNetworkAuthenticated()
	{
		using namespace Kortex;

		const IModNetwork* modNetwork= INetworkManager::GetInstance()->GetDefaultModNetwork();
		const ModNetworkAuth* auth = nullptr;

		return modNetwork && modNetwork->TryGetComponent(auth) && auth->IsAuthenticated();
	};
	template<class TModNetwork, class TContainer> TModNetwork& AddModNetwork(TContainer&& container)
	{
		return static_cast<TModNetwork&>(*container.emplace_back(std::make_unique<TModNetwork>()));
	}
}

namespace Kortex::NetworkManager
{
	void DefaultNetworkManager::OnInit()
	{
		using namespace NetworkManager;
		using namespace Application;
		
		// Create cache folder first
		KxFile(GetCacheDirectory()).CreateFolder();

		// Load default source
		if (IModNetwork* modNetwork = GetModNetworkByName(GetAInstanceOption(OName::ModNetwork).GetAttribute(OName::Default)))
		{
			m_DefaultModNetwork = modNetwork;
		}

		// Let the base class complete initialization
		INetworkManager::OnInit();
	}
	void DefaultNetworkManager::OnExit()
	{
		using namespace Application;

		const IModNetwork* modNetwork = GetDefaultModNetwork();
		GetAInstanceOption(OName::ModNetwork).SetAttribute(OName::Default, modNetwork ? modNetwork->GetName() : wxEmptyString);

		INetworkManager::OnExit();
	}
	void DefaultNetworkManager::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode)
	{
		// Create sources
		m_ModNetworks.reserve(3);
		AddModNetwork<NexusModNetwork>(m_ModNetworks);
		AddModNetwork<LoversLabModNetwork>(m_ModNetworks);
		AddModNetwork<TESALLModNetwork>(m_ModNetworks);

		// Base class will initialize created networks
		INetworkManager::OnLoadInstance(instance, managerNode);
	}

	void DefaultNetworkManager::ValidateAuth()
	{
		for (auto& modNetwork: m_ModNetworks)
		{
			if (auto auth = modNetwork->TryGetComponent<ModNetworkAuth>())
			{
				auth->ValidateAuth();
			}
		}
		AdjustDefaultModNetwork();
	}
	bool DefaultNetworkManager::AdjustDefaultModNetwork()
	{
		if (!IsDefaultModNetworkAuthenticated())
		{
			m_DefaultModNetwork = nullptr;
			for (auto& modNetwork: m_ModNetworks)
			{
				const ModNetworkAuth* auth = nullptr;
				if (modNetwork->TryGetComponent(auth) && auth->IsAuthenticated())
				{
					m_DefaultModNetwork = modNetwork.get();
					return true;
				}
			}
			return false;
		}
		return true;
	}

	void DefaultNetworkManager::OnSetToolbarButton(kxf::UI::AuiToolBarItem& button)
	{
		m_LoginButton = &button;

		ValidateAuth();
		UpdateButton();
	}
	void DefaultNetworkManager::UpdateButton()
	{
		const ModNetworkAuth* auth = nullptr;
		if (m_DefaultModNetwork && m_DefaultModNetwork->TryGetComponent(auth) && auth->IsAuthenticated())
		{
			m_LoginButton->SetLabel(KTr("NetworkManager.SignedIn") + ": " + m_DefaultModNetwork->GetName());
			m_LoginButton->SetBitmap(ImageProvider::GetBitmap(m_DefaultModNetwork->GetIcon()));
		}
		else
		{
			m_LoginButton->SetLabel(KTr("NetworkManager.NotSignedIn"));
			m_LoginButton->SetBitmap(ImageProvider::GetBitmap(IModNetwork::GetGenericIcon()));
		}

		m_LoginButton->GetToolBar().UpdateUI();
	}
	void DefaultNetworkManager::CreateMenu()
	{
		kxf::UI::Menu::EndMenu();
		m_Menu = new kxf::UI::Menu();
		m_LoginButton->AssignDropdownMenu(m_Menu);

		for (const auto& modNetwork: m_ModNetworks)
		{
			if (kxf::UI::Menu* subMenu = new kxf::UI::Menu(); true)
			{
				kxf::UI::MenuItem* rootItem = m_Menu->Add(subMenu, modNetwork->GetName());
				rootItem->SetBitmap(ImageProvider::GetBitmap(modNetwork->GetIcon()));

				ModNetworkAuth* authenticable = modNetwork->TryGetComponent<ModNetworkAuth>();
				ModNetworkRepository* repository = modNetwork->TryGetComponent<ModNetworkRepository>();

				// Add default source toggle
				{
					kxf::UI::MenuItem* item = subMenu->Add(new kxf::UI::MenuItem(wxEmptyString, wxEmptyString, wxITEM_CHECK));
					item->SetClientData(modNetwork.get());
					item->Enable(false);

					if (authenticable)
					{
						if (m_DefaultModNetwork == modNetwork.get())
						{
							item->Check();
							item->SetItemLabel(KTr("NetworkManager.ModNetwork.Default"));
						}
						else if (!authenticable->IsAuthenticated())
						{
							kxf::String label = kxf::String::Format("%1 (%2)", KTr("NetworkManager.ModNetwork.MakeDefault"), KTr("NetworkManager.NotSignedIn"));
							item->SetItemLabel(KxString::MakeCapitalized(label));
						}
						else
						{
							item->Enable();
							item->SetItemLabel(KTr("NetworkManager.ModNetwork.MakeDefault"));
							item->Bind(kxf::UI::MenuEvent::EvtSelect, &DefaultNetworkManager::OnSelectDefaultModSource, this);
						}
					}
					else
					{
						item->SetItemLabel(KTr("NetworkManager.ModNetwork.MakeDefault"));
					}
				}

				// Add sign-in/sign-out items.
				if (authenticable)
				{
					{
						kxf::String label;
						if (bool isAuth = authenticable->IsAuthenticated())
						{
							if (auto credentials = authenticable->LoadCredentials())
							{
								label = kxf::String::Format(wxS("%1: %2"), KTr("NetworkManager.SignOut"), credentials->UserID);
							}
							else
							{
								label = KTr("NetworkManager.SignOut");
							}
						}
						else
						{
							label = KTr("NetworkManager.SignIn");
						}

						kxf::UI::MenuItem* item = subMenu->Add(new kxf::UI::MenuItem(label));
						item->Bind(kxf::UI::MenuEvent::EvtSelect, &DefaultNetworkManager::OnSignInOut, this);
						item->SetBitmap(authenticable->GetUserPicture());
						item->SetClientData(modNetwork.get());
					}
					if (!authenticable->IsAuthenticated())
					{
						kxf::UI::MenuItem* item = subMenu->Add(new kxf::UI::MenuItem(KTr("NetworkManager.RetrySignIn")));
						item->Enable(authenticable->LoadCredentials().has_value());
						item->Bind(kxf::UI::MenuEvent::EvtSelect, [authenticable](kxf::UI::MenuEvent& event)
						{
							authenticable->ValidateAuth();
						});
					}
				}

				// Add limits information display
				if (repository)
				{
					if (ModRepositoryLimits limits = repository->GetRequestLimits(); limits.IsOK() && limits.HasLimits())
					{
						kxf::String label;
						auto AddSeparator = [&label]()
						{
							if (!label.IsEmpty())
							{
								label += wxS("; ");
							}
						};

						if (limits.HasHourlyLimit())
						{
							label += KTrf(wxS("NetworkManager.QueryLimits.Hourly"), limits.GetHourlyRemaining(), limits.GetHourlyTotal());
						}
						if (limits.HasDailyLimit())
						{
							AddSeparator();
							label += KTrf(wxS("NetworkManager.QueryLimits.Daily"), limits.GetDailyRemaining(), limits.GetDailyTotal());
						}
						label = kxf::String::Format(wxS("%1: [%2]"), KTr(wxS("NetworkManager.QueryLimits")), label);

						kxf::UI::MenuItem* item = subMenu->Add(new kxf::UI::MenuItem(label));
						item->SetBitmap(ImageProvider::GetBitmap(limits.AnyLimitDepleted() ? ImageResourceID::Exclamation : ImageResourceID::TickCircleFrame));
						item->SetClientData(modNetwork.get());
						item->Bind(kxf::UI::MenuEvent::EvtSelect, [&modNetwork](kxf::UI::MenuEvent& event)
						{
							KxTaskDialog dialog(&IMainWindow::GetInstance()->GetFrame(), wxID_NONE, KTr("NetworkManager.QueryLimits"));
							dialog.SetMainIcon(kxf::StdIcon::Information);
							dialog.SetOptionEnabled(KxTD_SIZE_TO_CONTENT);
							dialog.SetMessage(KTrf("NetworkManager.QueryLimits.Description", modNetwork->GetName()));
							dialog.ShowModal();
						});
					}
				}

				// Add any custom menu items
				{
					Utility::MenuSeparatorBefore separator(*subMenu);
					modNetwork->OnToolBarMenu(*subMenu);
				}
			}
		}
	}
	void DefaultNetworkManager::QueueUIUpdate()
	{
		BroadcastProcessor::Get().CallAfter([this]()
		{
			kxf::UI::Menu::EndMenu();
			UpdateButton();
		});
	}

	void DefaultNetworkManager::OnSignInOut(kxf::UI::MenuEvent& event)
	{
		IModNetwork* modNetwork = static_cast<IModNetwork*>(event.GetItem()->GetClientData());
		if (auto auth = modNetwork->TryGetComponent<ModNetworkAuth>(); auth && auth->IsAuthenticated())
		{
			KxTaskDialog dialog(event.GetInvokingWindow(), wxID_NONE, KTrf("NetworkManager.SignOutMessage", modNetwork->GetName()), wxEmptyString, KxBTN_YES|KxBTN_NO, KxICON_WARNING);
			if (dialog.ShowModal() == wxID_YES)
			{
				auth->SignOut();
				AdjustDefaultModNetwork();
				QueueUIUpdate();
			}
		}
		else
		{
			// Authentication process can be async, in this case auth handler should call 'OnAuthStateChanged'
			auth->Authenticate();
			if (auth->IsAuthenticated() && !IsDefaultModNetworkAuthenticated())
			{
				m_DefaultModNetwork = modNetwork;
			}
			QueueUIUpdate();
		}
	}
	void DefaultNetworkManager::OnSelectDefaultModSource(kxf::UI::MenuEvent& event)
	{
		using namespace Application;

		IModNetwork* modNetwork = static_cast<IModNetwork*>(event.GetItem()->GetClientData());
		m_DefaultModNetwork = modNetwork;

		UpdateButton();
		GetAInstanceOption(OName::ModNetwork).SetAttribute(OName::Default, modNetwork->GetName());
	}
	void DefaultNetworkManager::OnToolbarButton(kxf::UI::AuiToolBarEvent& event)
	{
		CreateMenu();
		m_LoginButton->ShowDropdownMenuLeftAlign();
	}

	void DefaultNetworkManager::OnAuthStateChanged()
	{
		AdjustDefaultModNetwork();

		CreateMenu();
		UpdateButton();
	}
	kxf::String DefaultNetworkManager::GetCacheDirectory() const
	{
		return IApplication::GetInstance()->GetUserSettingsFolder() + wxS("\\WebCache");
	}

	IModNetwork::RefVector DefaultNetworkManager::GetModNetworks()
	{
		IModNetwork::RefVector refs;
		refs.reserve(m_ModNetworks.size());
		if (m_DefaultModNetwork)
		{
			refs.push_back(m_DefaultModNetwork);
		}

		for (auto& modNetwork: m_ModNetworks)
		{
			if (modNetwork.get() != m_DefaultModNetwork)
			{
				refs.push_back(modNetwork.get());
			}
		}
		return refs;
	}
	IModNetwork* DefaultNetworkManager::GetDefaultModNetwork() const
	{
		return m_DefaultModNetwork;
	}
	IModNetwork* DefaultNetworkManager::GetModNetworkByName(const kxf::String& name) const
	{
		auto FindModNetwork = [this](const kxf::String& name) -> IModNetwork*
		{
			for (const auto& modNetwork: m_ModNetworks)
			{
				if (modNetwork->GetName() == name)
				{
					return modNetwork.get();
				}
			}
			return nullptr;
		};

		if (KxComparator::IsEqual(name, wxS("Nexus"), true) || KxComparator::IsEqual(name, wxS("NexusMods"), true))
		{
			return NexusModNetwork::GetInstance();
		}
		else if (KxComparator::IsEqual(name, wxS("TESALL"), true) || KxComparator::IsEqual(name, wxS("TESALL.RU"), true))
		{
			return TESALLModNetwork::GetInstance();
		}
		return FindModNetwork(name);
	}
}
