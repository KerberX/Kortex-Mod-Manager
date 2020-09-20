#include "stdafx.h"
#include "ModImporterMO.h"
#include <Kortex/Application.hpp>
#include <Kortex/PluginManager.hpp>
#include <Kortex/SaveManager.hpp>
#include <Kortex/ModManager.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/NetworkManager.hpp>
#include <Kortex/DownloadManager.hpp>
#include <Kortex/ProgramManager.hpp>
#include <Kortex/GameConfig.hpp>
#include "Network/ModNetwork/Nexus.h"
#include "Network/ModNetwork/LoversLab.h"
#include "Network/ModNetwork/TESALL.h"
#include "PackageProject/Serializer.h"
#include "Utility/String.h"
#include "Utility/OperationWithProgress.h"
#include <kxf::UI::Framework/KxFileFinder.h>
#include <kxf::UI::Framework/KxTextFile.h>
#include <kxf::UI::Framework/KxINI.h>

namespace Kortex::ModManager
{
	kxf::String& ModImporterMO::DecodeUTF8(kxf::String& string) const
	{
		// Find and replace all '\xABC' 5-char hex patterns to corresponding UTF-8 codes
		for (size_t i = 0; i < string.Length(); i++)
		{
			size_t pos = string.find(wxS("\\x"), i);
			if (pos != kxf::String::npos)
			{
				unsigned long value = 0;
				if (string.Mid(pos + 2, 3).ToCULong(&value, 16) && value != 0)
				{
					wxUniChar c(value);
					string.replace(pos, 5, c);
				}
			}
		}
		return string;
	}
	kxf::String& ModImporterMO::ProcessFilePath(kxf::String& path) const
	{
		DecodeUTF8(path);

		path.Replace(wxS("/"), wxS("\\"));
		path.Replace(wxS("\\\\"), wxS("\\"));
		path.Replace(wxS("\\\""), wxS("\""));
		return path;
	}
	kxf::String& ModImporterMO::ProcessDescription(kxf::String& path) const
	{
		DecodeUTF8(path);

		// Fix line terminators
		path.Replace(wxS("\\r\\n"), wxS("\r\n"));
		path.Replace(wxS("\\r"), wxS("\r"));
		path.Replace(wxS("\\n"), wxS("\n"));

		// Escape mnemonics
		path.Replace(wxS("&#39;"), wxS("\'"));
		path.Replace(wxS("&#34;"), wxS("\""));
		path.Replace(wxS("&#38;"), wxS("&"));
		path.Replace(wxS("&60;"), wxS("<"));
		path.Replace(wxS("&62;"), wxS(">"));
		path.Replace(wxS("&33;"), wxS("!"));

		// Remove quotes from beginning and at the end
		if (!path.IsEmpty())
		{
			if (path[0] == wxS('\"'))
			{
				path.Remove(0, 1);
			}
		}
		if (!path.IsEmpty())
		{
			if (path.Last() == wxS('\"'))
			{
				path.RemoveLast(1);
			}
		}

		// Convert BB code
		path = PackageProject::Serializer::ConvertBBCode(path);
		return path;
	}

	GameID ModImporterMO::TranslateGameIDToNetwork(const kxf::String& name)
	{
		if (!name.IsEmpty())
		{
			// TES
			if (name == "Morrowind")
			{
				return GameIDs::Morrowind;
			}
			if (name == "Oblivion")
			{
				return GameIDs::Oblivion;
			}
			if (name == "Skyrim")
			{
				return GameIDs::Skyrim;
			}
			if (name == "Skyrim Special Edition")
			{
				return GameIDs::SkyrimSE;
			}
			if (name == "Skyrim VR")
			{
				return GameIDs::SkyrimVR;
			}

			// Fallout
			if (name == "Fallout 3")
			{
				return GameIDs::Fallout3;
			}
			if (name == "New Vegas" || name == "TTW")
			{
				return GameIDs::FalloutNV;
			}
			if (name == "Fallout 4")
			{
				return GameIDs::Fallout4;
			}
			if (name == "Fallout 4 VR")
			{
				return GameIDs::Fallout4VR;
			}
		}
		return GameIDs::NullGameID;
	}
	void ModImporterMO::LoadOptions()
	{
		// Game name
		m_TargetGameID = TranslateGameIDToNetwork(m_Options.GetValue("General", "gameName"));
		m_TargetInstance = IGameInstance::GetTemplate(m_TargetGameID);
		m_ModManagerName = m_TargetGameID.IsOK() ? "Mod Organizer 2.x" : "Mod Organizer 1.x";

		// Current mod list
		m_CurrentProfile = m_Options.GetValue("General", "selected_profile");

		// Directories
		kxf::String baseDirectory = m_Options.GetValue("Settings", "base_directory");
		if (!baseDirectory.IsEmpty())
		{
			m_InstanceDirectory = baseDirectory;
		}

		auto LoadAndExpand = [this](kxf::String& saveInto, const kxf::String& optionName, const kxf::String& defaultPath)
		{
			kxf::String value = m_Options.GetValue("Settings", optionName);
			if (!value.IsEmpty())
			{
				ProcessFilePath(value);
				value.Replace("%BASE_DIR%", m_InstanceDirectory);
				saveInto = value;
			}
			else
			{
				saveInto = m_InstanceDirectory + '\\' + defaultPath;
			}
		};
		LoadAndExpand(m_DownloadsDirectory, "download_directory", "downloads");
		LoadAndExpand(m_ModsDirectory, "mod_directory", "mods");
		LoadAndExpand(m_ProfilesDirectory, "profiles_directory", "profiles");
	}
	kxf::String ModImporterMO::GetDataFolderName() const
	{
		if (m_TargetGameID == GameIDs::Morrowind)
		{
			return "Data Files";
		}
		else
		{
			return "Data";
		}
	}
	kxf::String ModImporterMO::GetProfileDirectory() const
	{
		return m_ProfilesDirectory + '\\' + GetSelectedProfile();
	}

	void ModImporterMO::ReadExecutables(Utility::OperationWithProgressDialogBase* context)
	{
		context->SetDialogCaption(kxf::String::Format("%1 \"%2\"", KTr("Generic.Import"), IProgramManager::GetInstance()->GetManagerInfo().GetName()));

		const kxf::String sectionName("customExecutables");
		long long count = -1;
		if (m_Options.GetValue(sectionName, "size").ToLongLong(&count) && count > 0)
		{
			for (size_t i = 1; i <= (size_t)count; i++)
			{
				if (!context->CanContinue())
				{
					return;
				}

				auto GetValue = [this, &sectionName, i](const auto& name)
				{
					return m_Options.GetValue(sectionName, kxf::String::Format("%1\\%2", i, name));
				};

				IProgramItem& entry = IProgramManager::GetInstance()->EmplaceProgram();
				entry.SetName(GetValue("title"));
				entry.SetExecutable(ProcessFilePath(GetValue("binary")));
				entry.SetArguments(ProcessFilePath(GetValue("arguments")));
				entry.SetWorkingDirectory(ProcessFilePath(GetValue("workingDirectory")));

				if (entry.GetName().IsEmpty())
				{
					entry.SetName(entry.GetExecutable().AfterLast('\\').BeforeLast('.'));
				}
			}
		}
	}
	void ModImporterMO::CopySaves(Utility::OperationWithProgressDialogBase* context)
	{
		if (ISaveManager* saveManager = ISaveManager::GetInstance())
		{
			context->SetDialogCaption(kxf::String::Format("%1 \"%2\"", KTr("Generic.Import"), saveManager->GetManagerInfo().GetName()));

			KxEvtFile source(GetProfileDirectory() + "\\Saves");
			context->LinkHandler(&source, KxEVT_FILEOP_COPY_FOLDER);
			source.CopyFolder(KxFile::NullFilter, saveManager->GetConfig().GetLocation(), true, true);

			saveManager->ScheduleWorkspacesReload();
		}
	}
	void ModImporterMO::CopyMods(Utility::OperationWithProgressDialogBase* context)
	{
		IGameProfile* profile = IGameInstance::GetActiveProfile();
		GameInstance::ProfileMod::Vector& currentModList = profile->GetMods();
		currentModList.clear();

		auto AddMod = [&currentModList](IGameMod& mod, bool isActive)
		{
			currentModList.emplace_back(mod, isActive);
		};
		auto GetModFolder = [this](const kxf::String& name)
		{
			return m_ModsDirectory + '\\' + name;
		};

		size_t counter = 0;
		KxStringVector modsList = KxTextFile::ReadToArray(GetProfileDirectory() + "\\ModList.txt");

		// MO stores mod list in reverse order.
		for (auto it = modsList.rbegin(); it != modsList.rend(); ++it)
		{
			if (!context->CanContinue())
			{
				return;
			}

			counter++;
			kxf::String& name = *it;
			if (!name.IsEmpty())
			{
				wxUniChar c = name[0];
				if (c == '+' || c == '-')
				{
					// Check if mod enabled and remove state char
					const bool isModActive = c == '+';
					name.Remove(0, 1);

					// Notify
					context->SetDialogCaption(kxf::String::Format("%1 \"%2\", %3/%4", KTr("Generic.Import"), name, counter, modsList.size()));

					// Check mod existence
					IGameMod* existingMod = IModManager::GetInstance()->FindModByID(name);
					if (existingMod && ShouldSkipExistingMods())
					{
						AddMod(*existingMod, isModActive);
						continue;
					}

					// Load 'meta.ini'
					kxf::String modFolder = GetModFolder(name);
					KxFileStream metaStream(modFolder + "\\Meta.ini");
					KxINI modINI(metaStream);

					// Write data
					IGameMod& mod = IModManager::GetInstance()->EmplaceMod();
					mod.SetName(name);
					mod.SetActive(isModActive);
					mod.SetVersion(modINI.GetValue("General", "version"));
					mod.SetPackageFile(ProcessFilePath(modINI.GetValue("General", "installationFile")));
					mod.SetDescription(ProcessDescription(modINI.GetValue("General", "nexusDescription")));

					// NexusID
					ModID nexusID = modINI.GetValueInt("General", "modid", ModID::GetInvalidValue());
					if (nexusID)
					{
						mod.GetModSourceStore().AssignWith<NetworkManager::NexusModNetwork>(nexusID);
					}

					// Install date
					wxDateTime date = KxFile(modFolder).GetFileTime(KxFILETIME_CREATION);
					if (isModActive)
					{
						mod.SetInstallTime(date);
					}
					else
					{
						mod.SetUninstallTime(date);
					}

					// If such mod already exist, try create unique ID
					if (existingMod)
					{
						mod.SetID(kxf::String::Format("[MO] %1", name));
					}
					else
					{
						mod.SetID(name);
					}

					// Save
					AddMod(mod, isModActive);
					mod.Save();

					// Copy mod contents
					kxf::String destination = mod.GetModFilesDir() + wxS('\\') + GetDataFolderName();

					KxEvtFile source(modFolder);
					context->LinkHandler(&source, KxEVT_FILEOP_COPY_FOLDER);
					source.CopyFolder(KxFile::NullFilter, destination, true, true);

					// Remove 'meta.ini' file
					KxFile(destination + "\\Meta.ini").RemoveFile();

					// Update mod content
					mod.UpdateFileTree();
				}
			}
		}
		profile->SaveConfig();
		IModDispatcher::GetInstance()->UpdateVirtualTree();
	}
	void ModImporterMO::ReadPlugins(Utility::OperationWithProgressDialogBase* context)
	{
		if (IPluginManager* pluginManager = IPluginManager::GetInstance())
		{
			context->SetDialogCaption(kxf::String::Format("%1 \"%2\"", KTr("Generic.Import"), pluginManager->GetManagerInfo().GetName()));

			KxStringVector activePlugins = KxTextFile::ReadToArray(GetProfileDirectory() + "\\Plugins.txt");
			KxStringVector allPlugins = KxTextFile::ReadToArray(GetProfileDirectory() + "\\LoadOrder.txt");

			IGameProfile* profile = IGameInstance::GetActiveProfile();
			GameInstance::ProfilePlugin::Vector& currentPluginsList = profile->GetPlugins();
			currentPluginsList.clear();

			for (kxf::String& name: allPlugins)
			{
				if (!context->CanContinue())
				{
					return;
				}

				if (!name.IsEmpty())
				{
					if (name[0] == '*')
					{
						name.Remove(0, 1);
					}
				}
				currentPluginsList.emplace_back(name, Utility::String::IsStringsContain(activePlugins, name, false));
			}
			profile->SaveConfig();

			pluginManager->Load();
			pluginManager->ScheduleWorkspacesReload();
		}
	}
	void ModImporterMO::CopyGameConfig(Utility::OperationWithProgressDialogBase* context)
	{
		if (IGameConfigManager* configManager = IGameConfigManager::GetInstance())
		{
			context->SetDialogCaption(kxf::String::Format("%1 \"%2\"", KTr("Generic.Import"), configManager->GetManagerInfo().GetName()));

			configManager->ForEachGroup([this](GameConfig::ItemGroup& group)
			{
				GameConfig::IFileSource* fileSource = nullptr;
				GameConfig::ISource& source = group.GetSource();
				if (source.QueryInterface(fileSource))
				{
					KxFile file(GetProfileDirectory() + '\\' + fileSource->GetExpandedFileName());
					if (file.IsFileExist())
					{
						source.Close();
						file.CopyFile(fileSource->GetResolvedFilePath(), true);
					}
				}
				return true;
			});
			configManager->Load();
			configManager->ScheduleWorkspacesReload();
		}
	}
	void ModImporterMO::CopyDownloads(Utility::OperationWithProgressDialogBase* context)
	{
		IDownloadManager* manager = IDownloadManager::GetInstance();
		manager->PauseAllActive();

		KxFileFinder finder(m_DownloadsDirectory, "*.meta");
		for (KxFileItem item = finder.FindNext(); item.IsOK(); item = finder.FindNext())
		{
			KxEvtFile archiveFile(item.GetFullPath().BeforeLast('.'));
			if (item.IsNormalItem() && item.IsFile())
			{
				KxFileStream stream(item.GetFullPath(), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting);
				KxINI ini(stream);

				DownloadItemBuilder download;
				download.SetTargetGame(ini.GetValue("General", "gameName"));

				ModNetworkRepository* repository = nullptr;
				IModNetwork* modNetwork = INetworkManager::GetInstance()->GetModNetworkByName(ini.GetValue("General", "repository"));
				if (modNetwork && modNetwork->TryGetComponent(repository))
				{
					download.SetModRepository(*repository);
				}
				download.SetDownloadDate(archiveFile.GetFileTime(KxFileTime::KxFILETIME_CREATION));
				download.SetURI(ini.GetValue("General", "url").AfterFirst('"').BeforeLast('"'));

				download.SetModID(ini.GetValueInt("General", "modID", -1));
				download.SetFileID(ini.GetValueInt("General", "fileID", -1));
				download.SetName(item.GetName().BeforeLast('.'));
				download.SetDisplayName(ini.GetValue("General", "modName"));
				download.SetVersion(ini.GetValue("General", "version"));

				const int64_t size = archiveFile.GetFileSize();
				download.SetTotalSize(size);
				download.SetDownloadedSize(size);
				download.Hide(ini.GetValueBool("General", "removed", false));

				if (ini.GetValueBool("General", "paused", false))
				{
					download.ResumeFrom(size);
				}

				if (DownloadItem* item = download.Save())
				{
					context->LinkHandler(&archiveFile, KxEVT_FILEOP_COPY);
					archiveFile.CopyFile(item->GetLocalPath(), false);
				}
			}
		}
	}

	void ModImporterMO::SetDirectory(const kxf::String& path)
	{
		m_InstanceDirectory = path;
		if (!KxFileFinder::IsDirectoryEmpty(m_InstanceDirectory))
		{
			KxFileStream stream(m_InstanceDirectory + '\\' + "ModOrganizer.ini", KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting);
			m_Options.Load(stream);
			if (m_Options.IsOK())
			{
				LoadOptions();
				m_CanImport = true;
			}
		}
	}
	void ModImporterMO::Import(Utility::OperationWithProgressDialogBase* context)
	{
		if (context->CanContinue())
		{
			ReadExecutables(context);
		}
		if (context->CanContinue())
		{
			CopyGameConfig(context);
		}
		if (context->CanContinue())
		{
			CopySaves(context);
		}
		if (context->CanContinue())
		{
			CopyDownloads(context);
		}

		// Mods
		if (context->CanContinue())
		{
			CopyMods(context);
		}

		// This should go after mods
		if (context->CanContinue())
		{
			ReadPlugins(context);
		}
	}

	bool ModImporterMO::CanImport() const
	{
		if (m_CanImport)
		{
			// Allow import from unknown managed game, as MO1 doesn't store its name.
			if (m_TargetGameID.IsOK())
			{
				return m_TargetInstance != nullptr;
			}
			return true;
		}
		return false;
	}
	kxf::String ModImporterMO::GetAdditionalInfo() const
	{
		kxf::String info;

		// Version
		kxf::String version = m_Options.GetValue("General", "version");
		if (version.IsEmpty())
		{
			version = KxFile(m_InstanceDirectory + '\\' + "ModOrganizer.exe").GetVersionInfo().GetString("FileVersion");
			if (version.IsEmpty())
			{
				version = "<unknown>";
			}
		}
		info << KTr("Generic.Version") << ": " << version;

		// Target profile
		if (m_TargetInstance)
		{
			info << "\r\n" << KTr("ModManager.Import.ManagedGame") << ": " << m_TargetInstance->GetGameName();
		}

		return info;
	}
	KxStringVector ModImporterMO::GetAvailableProfiles() const
	{
		KxStringVector profiles;

		KxFileFinder finder(m_InstanceDirectory + "\\profiles", "*");
		for (KxFileItem item = finder.FindNext(); item.IsOK(); item = finder.FindNext())
		{
			if (item.IsNormalItem() && item.IsDirectory() && !KxFileFinder::IsDirectoryEmpty(item.GetFullPath()))
			{
				profiles.push_back(item.GetName());
			}
		}
		return profiles;
	}
}
