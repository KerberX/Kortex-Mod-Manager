#include "stdafx.h"
#include "NetworkModule.h"
#include "NetworkManager/DefaultNetworkManager.h"
#include "DownloadManager/DefaultDownloadManager.h"

namespace Kortex
{
	namespace Internal
	{
		const SimpleModuleInfo NetworkModuleTypeInfo("Network", "NetworkModule.Name", "1.0", ImageResourceID::ModNetwork_Unknown);
	}

	void NetworkModule::OnInit()
	{
		wxFileSystem::AddHandler(m_NetworkManager->NewWxFSHandler().release());
	}
	void NetworkModule::OnExit()
	{
	}
	void NetworkModule::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node)
	{
		m_NetworkManager = std::make_unique<NetworkManager::DefaultNetworkManager>();
		m_DownloadManager = CreateManagerIfEnabled<DownloadManager::DefaultDownloadManager>(node);
	}

	NetworkModule::NetworkModule()
		:ModuleWithTypeInfo(Disposition::Global)
	{
	}

	IModule::ManagerRefVector NetworkModule::GetManagers()
	{
		return ToManagersList(m_NetworkManager, m_DownloadManager);
	}
}
