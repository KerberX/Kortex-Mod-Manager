#include "stdafx.h"
#include <Kortex/PluginManager.hpp>
#include <Kortex/ModManager.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxFileStream.h>
#include <kxf::UI::Framework/KxINI.h>
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxComparator.h>

namespace Kortex::PluginManager
{
	void BethesdaPluginManagerMW::OnInit()
	{
		BethesdaPluginManager::OnInit();
	}
	void BethesdaPluginManagerMW::OnExit()
	{
		BethesdaPluginManager::OnExit();
	}
	void BethesdaPluginManagerMW::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode)
	{
		BethesdaPluginManager::OnLoadInstance(instance, managerNode);
	}

	void BethesdaPluginManagerMW::ReadOrderMW(const KxINI& ini)
	{
		auto files = IModDispatcher::GetInstance()->Find(m_PluginsLocation, [](const FileTreeNode& node)
		{
			return node.IsFile();
		}, false);

		// Load all names from 'Game Files Order' section. Discard non-existing files.
		KxStringVector loadOrder = ini.GetKeyNames("Game Files Order");
		for (const kxf::String& name: loadOrder)
		{
			// Check whether plugin with this name exist
			auto it = std::find_if(files.begin(), files.end(), [&name](const FileTreeNode* node)
			{
				return KxComparator::IsEqual(node->GetName(), name);
			});

			if (it != files.end())
			{
				if (CheckExtension(name))
				{
					GetPlugins().emplace_back(CreatePlugin((*it)->GetFullPath(), false));
				}
			}
		}

		// Load files form 'Data Files' folder. Don't add already existing
		for (const FileTreeNode* fileNode: files)
		{
			if (CheckExtension(fileNode->GetName()))
			{
				auto& entry = GetPlugins().emplace_back(CreatePlugin(fileNode->GetFullPath(), false));
			}
		}
	}
	void BethesdaPluginManagerMW::ReadActiveMW(const KxINI& ini)
	{
		KxStringVector activeOrder = ini.GetKeyNames("Game Files");
		for (const kxf::String& nameID: activeOrder)
		{
			kxf::String name = ini.GetValue("Game Files", nameID);
			IGamePlugin* entry = FindPluginByName(name);
			if (entry)
			{
				entry->SetActive(true);
			}
		}
	}
	void BethesdaPluginManagerMW::WriteOrderMW(KxINI& ini) const
	{
		wxDateTime fileTime = wxDateTime::Now() - wxTimeSpan(0, GetPlugins().size());
		const wxTimeSpan timeStep(0, 1); // One minute

		int i = 0;
		ini.RemoveSection("Game Files Order");
		for (const GameInstance::ProfilePlugin& listItem: IGameInstance::GetActive()->GetActiveProfile()->GetPlugins())
		{
			if (const IGamePlugin* plugin = listItem.GetPlugin())
			{
				ini.SetValue("Game Files Order", kxf::String::Format("GameFile%1", i), OnWriteToLoadOrder(*plugin));
				i++;

				if (ShouldChangeFileModificationDate())
				{
					KxFile(plugin->GetFullPath()).SetFileTime(fileTime, KxFILETIME_MODIFICATION);
					fileTime.Add(timeStep);
				}
			}
		}
	}
	void BethesdaPluginManagerMW::WriteActiveMW(KxINI& ini) const
	{
		int i = 0;
		ini.RemoveSection("Game Files");
		for (auto& entry: GetPlugins())
		{
			if (entry->IsActive())
			{
				ini.SetValue("Game Files", kxf::String::Format("GameFile%1", i), OnWriteToActiveOrder(*entry));
				i++;
			}
		}
	}
	kxf::String BethesdaPluginManagerMW::GetMorrowindINI() const
	{
		return IGameInstance::GetActiveProfile()->GetConfigDir() + wxS('\\') + m_PluginsListFile;
	}

	void BethesdaPluginManagerMW::LoadNativeOrderBG()
	{
		ClearPlugins();

		KxFileStream stream(GetMorrowindINI(), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
		KxINI ini(stream);
		if (ini.IsOK())
		{
			ReadOrderMW(ini);
			ReadActiveMW(ini);

			if (ShouldSortByFileModificationDate())
			{
				SortByDate();
			}
		}
	}
	void BethesdaPluginManagerMW::LoadNativeActiveBG()
	{
	}
	void BethesdaPluginManagerMW::SaveNativeOrderBG() const
	{
		KxFileStream stream(GetMorrowindINI(), KxFileStream::Access::RW, KxFileStream::Disposition::OpenAlways, KxFileStream::Share::Read);
		KxINI ini(stream);

		WriteActiveMW(ini);
		WriteOrderMW(ini);

		stream.Rewind();
		stream.SetAllocationSize();
		ini.Save(stream);

		//IWorkspace::ScheduleReloadOf<KGameConfigWorkspace>();
	}

	BethesdaPluginManagerMW::BethesdaPluginManagerMW()
		:m_PluginsListFile(wxS("Morrowind.ini"))
	{
		m_PluginsLocation = wxS("Data Files");
	}
	BethesdaPluginManagerMW::~BethesdaPluginManagerMW()
	{
	}

	void BethesdaPluginManagerMW::Save() const
	{
		BethesdaPluginManager::Save();
	}
	void BethesdaPluginManagerMW::Load()
	{
		BethesdaPluginManager::Load();
	}
	void BethesdaPluginManagerMW::LoadNativeOrder()
	{
		BethesdaPluginManager::LoadNativeOrder();
	}
}
