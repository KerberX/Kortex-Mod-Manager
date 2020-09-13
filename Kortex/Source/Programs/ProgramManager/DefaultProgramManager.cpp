#include "stdafx.h"
#include "DefaultProgramManager.h"
#include "DefaultProgramItem.h"
#include "Programs/ProgramEvent.h"
#include "Workspace.h"
#include <Kortex/Application.hpp>
#include <Kortex/Common/Programs.hpp>

namespace Kortex::ProgramManager
{
	namespace ProgramManager::Internal
	{
		const SimpleManagerInfo TypeInfo("DefaultProgramManager", "ProgramManager.Name");
	}

	void DefaultProgramManager::OnInit()
	{
		LoadUserPrograms();
	}
	void DefaultProgramManager::OnExit()
	{
		SaveUserPrograms();
	}
	void DefaultProgramManager::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode)
	{
		LoadProgramsFromXML(m_DefaultPrograms, managerNode.GetFirstChildElement("DefaultPrograms"));
	}
	void DefaultProgramManager::CreateWorkspaces()
	{
		new Workspace();
	}

	void DefaultProgramManager::LoadUserPrograms()
	{
		kxf::XMLNode node = GetAInstanceOption(Application::OName::UserPrograms).GetNode();
		LoadProgramsFromXML(m_UserPrograms, node);
	}
	void DefaultProgramManager::SaveUserPrograms() const
	{
		auto option = GetAInstanceOption(Application::OName::UserPrograms);
		kxf::XMLNode rootNode = option.GetNode();
		rootNode.ClearNode();

		for (const auto& entry: m_UserPrograms)
		{
			kxf::XMLNode node = rootNode.NewElement(wxS("Entry"));
			entry->Save(node);
		}
		option.NotifyChange();
	}

	IWorkspace::RefVector DefaultProgramManager::EnumWorkspaces() const
	{
		return ToWorkspacesList(Workspace::GetInstance());
	}

	std::unique_ptr<IProgramItem> DefaultProgramManager::NewProgram()
	{
		return std::make_unique<DefaultProgramItem>();
	}
	void DefaultProgramManager::RemoveProgram(IProgramItem& programEntry)
	{
		auto it = std::remove_if(m_UserPrograms.begin(), m_UserPrograms.end(), [&programEntry](const auto& item)
		{
			return item.get() == &programEntry;
		});
		if (it != m_UserPrograms.end())
		{
			m_UserPrograms.erase(it, m_UserPrograms.end());
		}
	}
	void DefaultProgramManager::LoadDefaultPrograms()
	{
		for (const auto& programEntry: m_DefaultPrograms)
		{
			m_UserPrograms.emplace_back(std::make_unique<DefaultProgramItem>(static_cast<DefaultProgramItem&>(*programEntry)));
		}
	}
}
