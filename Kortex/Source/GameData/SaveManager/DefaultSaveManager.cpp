#include "stdafx.h"
#include "DefaultSaveManager.h"
#include "Workspace.h"
#include "BaseGameSave.h"
#include "EmptySaveFile.h"
#include "BethesdaSave/Morrowind.h"
#include "BethesdaSave/Oblivion.h"
#include "BethesdaSave/Skyrim.h"
#include "BethesdaSave/SkyrimSE.h"
#include "BethesdaSave/Fallout3.h"
#include "BethesdaSave/FalloutNV.h"
#include "BethesdaSave/Fallout4.h"
#include <Kortex/SaveManager.hpp>
#include <Kortex/GameInstance.hpp>
#include <KxFramework/KxFileFinder.h>

namespace
{
	template<class T> bool TryCreateSaveObject(std::unique_ptr<Kortex::IGameSave>& ptr, const kxf::String& requestedImpl, const wxChar* thisImpl)
	{
		if (requestedImpl == thisImpl)
		{
			ptr = std::make_unique<T>();
			return true;
		}
		return false;
	}
}

namespace Kortex::SaveManager
{
	void DefaultSaveManager::CreateWorkspaces()
	{
		new Workspace();
	}
	void DefaultSaveManager::OnSavesLocationChanged(BroadcastEvent& event)
	{
		ScheduleWorkspacesReload();
	}

	void DefaultSaveManager::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode)
	{
		m_Config.OnLoadInstance(instance, managerNode);
	}
	void DefaultSaveManager::OnInit()
	{
		m_BroadcastReciever.Bind(ProfileEvent::EvtChanged, &DefaultSaveManager::OnSavesLocationChanged, this);
		m_BroadcastReciever.Bind(ProfileEvent::EvtSelected, &DefaultSaveManager::OnSavesLocationChanged, this);
	}
	void DefaultSaveManager::OnExit()
	{
	}

	IWorkspace::RefVector DefaultSaveManager::EnumWorkspaces() const
	{
		return ToWorkspacesList(Workspace::GetInstance());
	}
	std::unique_ptr<IGameSave> DefaultSaveManager::NewSave() const
	{
		const kxf::String requestedImplementation = m_Config.GetSaveImplementation();
		std::unique_ptr<IGameSave> object;

		TryCreateSaveObject<BethesdaSave::Morrowind>(object, requestedImplementation, wxS("BethesdaMorrowind")) ||
		TryCreateSaveObject<BethesdaSave::Oblivion>(object, requestedImplementation, wxS("BethesdaOblivion")) ||
		TryCreateSaveObject<BethesdaSave::Skyrim>(object, requestedImplementation, wxS("BethesdaSkyrim")) ||
		TryCreateSaveObject<BethesdaSave::SkyrimSE>(object, requestedImplementation, wxS("BethesdaSkyrimSE")) ||

		TryCreateSaveObject<BethesdaSave::Fallout3>(object, requestedImplementation, wxS("BethesdaFallout3")) ||
		TryCreateSaveObject<BethesdaSave::FalloutNV>(object, requestedImplementation, wxS("BethesdaFalloutNV")) ||
		TryCreateSaveObject<BethesdaSave::Fallout4>(object, requestedImplementation, wxS("BethesdaFallout4")) ||
		TryCreateSaveObject<EmptySaveFile>(object, requestedImplementation, wxS("Sacred2"));

		return object;
	}

	void DefaultSaveManager::UpdateActiveFilters(const KxStringVector& filters)
	{
		m_ActiveFilters = filters;
		ClearSaves();

		for (const kxf::String& filter: m_ActiveFilters)
		{
			KxFileFinder finder(m_Config.GetLocation(), filter);
			for (KxFileItem item = finder.FindNext(); item.IsOK(); item = finder.FindNext())
			{
				auto entry = NewSave();
				if (entry && entry->Create(item.GetFullPath()))
				{
					EmplaceSave(std::move(entry));
				}
			}
		}

		BroadcastProcessor::Get().ProcessEvent(SaveEvent::EvtFiltersChanged);
	}
}

namespace Kortex::SaveManager
{
	void Config::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node)
	{
		m_SaveImplementation = node.GetAttribute("SaveImplementation");
		m_Location = node.GetFirstChildElement("Location").GetValue();

		// Load file filters
		for (kxf::XMLNode entryNode = node.GetFirstChildElement("FileFilters").GetFirstChildElement(); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
		{
			m_FileFilters.emplace_back(entryNode.GetValue(), KVarExp(entryNode.GetAttribute("Label")));
		}

		// Multi-file save config
		m_PrimarySaveExt = node.GetFirstChildElement("PrimaryExtension").GetValue();
		m_SecondarySaveExt = node.GetFirstChildElement("SecondaryExtension").GetValue();

		// Bitmap size
		constexpr int defaultScreenshotHeight = 96;
		if (kxf::XMLNode sizeNode = node.GetFirstChildElement("ScreenshotSize"); sizeNode.IsOK())
		{
			if (double ratio = sizeNode.GetAttributeFloat("Ratio"); ratio > 0)
			{
				if (int width = sizeNode.GetAttributeInt("Width"); width > 0)
				{
					m_BitmapSize.FromWidth(width, ratio);
				}
				else if (int height = sizeNode.GetAttributeInt("Height", defaultScreenshotHeight); height > 0)
				{
					m_BitmapSize.FromHeight(height, ratio);
				}
			}
			else
			{
				int width = sizeNode.GetAttributeInt("Width");
				int height = sizeNode.GetAttributeInt("Height");
				if (width > 0 && height > 0)
				{
					m_BitmapSize = Utility::BitmapSize(width, height);
				}
			}
		}
		if (!m_BitmapSize.IsFullySpecified())
		{
			m_BitmapSize.FromHeight(defaultScreenshotHeight, Utility::BitmapSize::r16_9);
		}
	}

	kxf::String Config::GetSaveImplementation() const
	{
		return KVarExp(m_SaveImplementation);
	}
	kxf::String Config::GetLocation() const
	{
		return KVarExp(m_Location);
	}
}
