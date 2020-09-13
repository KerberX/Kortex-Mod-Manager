#include "stdafx.h"
#include <Kortex/PluginManager.hpp>
#include <Kortex/ModManager.hpp>
#include <Kortex/Application.hpp>
#include "IGameInstance.h"
#include "IGameProfile.h"
#include "Utility/Common.h"
#include <KxFramework/KxFileStream.h>

namespace
{
	namespace Util
	{
		using namespace Kortex;

		kxf::String GetGlobalRelativePath(const kxf::String& folderName)
		{
			return IGameInstance::GetActive()->GetInstanceRelativePath(wxS("GlobalProfile")) + wxS('\\') + folderName;
		}
		kxf::String GetLocalPath(const kxf::String& id)
		{
			return IGameInstance::GetActive()->GetProfilesDir() + wxS('\\') + id;
		}
		kxf::String GetLocalRelativePath(const kxf::String& id, const kxf::String& name)
		{
			return GetLocalPath(id) + wxS('\\') + name;
		}

		bool CreateLocalFolder(const kxf::String& id, const kxf::String& name)
		{
			return KxFile(GetLocalRelativePath(id, name)).CreateFolder();
		}
	}
}

namespace Kortex
{
	IGameProfile* IGameProfile::GetActive()
	{
		if (IGameInstance* instance = IGameInstance::GetActive())
		{
			return instance->GetActiveProfile();
		}
		return nullptr;
	}

	void IGameProfile::UpdateVariablesUsingActive(IVariableTable& variables)
	{
		variables.SetVariable(Variables::KVAR_GLOBAL_SAVES_DIR, Util::GetGlobalRelativePath(GameInstance::FolderName::Saves));
		variables.SetVariable(Variables::KVAR_GLOBAL_CONFIG_DIR, Util::GetGlobalRelativePath(GameInstance::FolderName::Config));
	}
	kxf::String IGameProfile::ProcessID(const kxf::String& id)
	{
		return Utility::MakeSafeFileName(id);
	}
	bool IGameProfile::CreateLocalFolder(const kxf::String& id, const kxf::String& name)
	{
		return Util::CreateLocalFolder(id, name);
	}

	kxf::String IGameProfile::GetConfigFile() const
	{
		return Util::GetLocalRelativePath(GetID(), wxS("Profile.xml"));
	}
	kxf::String IGameProfile::GetProfileDir() const
	{
		return Util::GetLocalPath(GetID());
	}
	kxf::String IGameProfile::GetProfileRelativePath(const kxf::String& name) const
	{
		return Util::GetLocalRelativePath(GetID(), name);
	}
	kxf::String IGameProfile::GetSavesDir() const
	{
		if (IsLocalSavesEnabled())
		{
			return GetProfileRelativePath(GameInstance::FolderName::Saves);
		}
		else
		{
			return Util::GetGlobalRelativePath(GameInstance::FolderName::Saves);
		}
	}
	kxf::String IGameProfile::GetConfigDir() const
	{
		if (IsLocalSavesEnabled())
		{
			return GetProfileRelativePath(GameInstance::FolderName::Config);
		}
		else
		{
			return Util::GetGlobalRelativePath(GameInstance::FolderName::Config);
		}
	}
	kxf::String IGameProfile::GetOverwritesDir() const
	{
		return GetProfileRelativePath(GameInstance::FolderName::Overwrites);
	}
}

namespace Kortex::GameInstance
{
	ProfileMod::ProfileMod(const kxf::String& signature, bool active, intptr_t priority)
		:m_Signature(signature), m_Priority(priority >= 0 ? priority : std::numeric_limits<intptr_t>::max()), m_IsActive(active)
	{
	}
	ProfileMod::ProfileMod(const IGameMod& mod, bool active)
		:m_Signature(mod.GetSignature()), m_Priority(mod.GetPriority()),  m_IsActive(active)
	{
	}

	IGameMod* ProfileMod::GetMod() const
	{
		if (IModManager* manager = IModManager::GetInstance())
		{
			return manager->FindModBySignature(m_Signature);
		}
		return nullptr;
	}
}

namespace Kortex::GameInstance
{
	ProfilePlugin::ProfilePlugin(const IGamePlugin& plugin, bool active)
		:m_Name(plugin.GetName()), m_Priority(plugin.GetPriority()), m_IsActive(active)
	{
	}
	ProfilePlugin::ProfilePlugin(const kxf::String& name, bool enabled, intptr_t priority)
		: m_Name(name), m_Priority(priority >= 0 ? priority : std::numeric_limits<intptr_t>::max()), m_IsActive(enabled)
	{
	}

	IGamePlugin* ProfilePlugin::GetPlugin() const
	{
		if (IPluginManager* manager = IPluginManager::GetInstance())
		{
			return manager->FindPluginByName(m_Name);
		}
		return nullptr;
	}
}
