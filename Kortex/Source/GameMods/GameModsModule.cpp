#include "stdafx.h"
#include "GameModsModule.h"
#include <Kortex/ModManager.hpp>
#include <Kortex/ModStatistics.hpp>
#include <Kortex/ModTagManager.hpp>
#include "ModManager/DefaultModManager.h"
#include "ModManager/DefaultModDispatcher.h"

namespace Kortex
{
	namespace Internal
	{
		const SimpleModuleInfo GameModsTypeInfo("GameMods", "GameModsModule.Name", "1.3", ImageResourceID::Puzzle);
	}

	void GameModsModule::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node)
	{
		m_TagManager = std::make_unique<ModTagManager::DefaultTagManager>();
		m_ModManager = std::make_unique<ModManager::DefaultModManager>();
		m_ModDispatcher = std::make_unique<ModManager::DefaultModDispatcher>();
		m_ModStatistics = std::make_unique<ModStatistics::DefaultStatistics>();
	}
	void GameModsModule::OnInit()
	{
	}
	void GameModsModule::OnExit()
	{
	}

	GameModsModule::GameModsModule()
		:ModuleWithTypeInfo(Disposition::Global)
	{
	}

	IModule::ManagerRefVector GameModsModule::GetManagers()
	{
		return ToManagersList(m_ModManager, m_TagManager);
	}
}
