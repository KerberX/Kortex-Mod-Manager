#include "stdafx.h"
#include "IPackageManager.h"
#include "ModPackagesModule.h"
#include <Kortex/Application.hpp>
#include <Kortex/ApplicationOptions.hpp>
#include <Kortex/PluginManager.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/ModManager.hpp>
#include <Kortex/Common/Packages.hpp>
#include "PackageProject/ModPackageProject.h"
#include "PackageCreator/Workspace.h"
#include "Archive/GenericArchive.h"
#include "Utility/Common.h"
#include "Utility/OperationWithProgress.h"
#include <KxFramework/KxXML.h>
#include <KxFramework/KxLibrary.h>
#include <KxFramework/KxShell.h>
#include <KxFramework/KxFileStream.h>
#include <KxFramework/KxArchiveEvent.h>
#include <KxFramework/KxTaskDialog.h>
#include <KxFramework/KxFileBrowseDialog.h>

namespace Kortex
{
	namespace PackageDesigner::Internal
	{
		const SimpleManagerInfo TypeInfo("PackageManager", "PackageManager.Name");
	}

	KxStringVector IPackageManager::GetSuppoptedExtensions()
	{
		return {wxS("kmp"), wxS("smi"), wxS("fomod"), wxS("7z"), wxS("zip"), wxS("rar")};
	}
	kxf::String IPackageManager::GetSuppoptedExtensionsFilter()
	{
		return Utility::MakeExtensionsFilter(GetSuppoptedExtensions());
	}
	void IPackageManager::ExtractAcrhiveWithProgress(wxWindow* window, const kxf::String& filePath, const kxf::String& outPath)
	{
		auto thread = new Utility::OperationWithProgressDialog<KxArchiveEvent>(true, wxGetTopLevelParent(window));
		thread->OnRun([thread, filePath, outPath]()
		{
			GenericArchive archive(filePath);
			thread->LinkHandler(&archive, KxArchiveEvent::EvtProcess);
			const bool success = archive.ExtractToDirectory(outPath);

			// Set extraction successfulness status
			thread->SetClientData(success ? reinterpret_cast<void*>(1) : nullptr);
		});
		thread->OnEnd([thread, window, outPath]()
		{
			// Show warning message if something went wrong
			if (thread->GetClientData() == nullptr)
			{
				KxTaskDialog dialog(window, KxID_NONE, KTrf("InstallWizard.LoadFailed.Caption", outPath), KTr("InstallWizard.LoadFailed.Message"), KxBTN_OK, KxICON_ERROR);
				dialog.ShowModal();
			}
		});
		thread->SetDialogCaption(KTr("ModManager.ExtractingPackageFiles"));
		thread->Run();
	}

	bool IPackageManager::IsPathAbsolute(const kxf::String& path)
	{
		return path.IsEmpty() || (path.Length() >= 2 && path[1] == wxS(':'));
	}
	kxf::String IPackageManager::GetRequirementFilePath(const PackageProject::RequirementItem* entry)
	{
		kxf::String path = KVarExp(entry->GetObject());
		if (IsPathAbsolute(path))
		{
			// Is object is absolute file path, return it as is.
			return path;
		}
		else
		{
			return IModDispatcher::GetInstance()->ResolveLocationPath(path);
		}
	}

	PackageProject::ReqState IPackageManager::CheckRequirementState(const PackageProject::RequirementItem* entry)
	{
		using namespace PackageDesigner;
		using namespace PackageProject;

		ObjectFunction objectFunc = entry->GetObjectFunction();
		switch (objectFunc)
		{
			case ObjectFunction::None:
			{
				return ReqState::True;
			}
			case ObjectFunction::ModActive:
			case ObjectFunction::ModInactive:
			{
				if (!entry->GetID().IsEmpty())
				{
					bool isActive = IModManager::GetInstance()->IsModActive(entry->GetID());
					return FromInt<ReqState>(objectFunc == ObjectFunction::ModActive ? isActive : !isActive);
				}
				return ReqState::False;
			}
			case ObjectFunction::PluginActive:
			case ObjectFunction::PluginInactive:
			{
				if (!entry->GetObject().IsEmpty())
				{
					IPluginManager* manager = IPluginManager::GetInstance();
					if (manager)
					{
						if (!manager->HasPlugins())
						{
							manager->Load();
						}

						bool isActive = manager->IsPluginActive(entry->GetObject());
						return FromInt<ReqState>(objectFunc == ObjectFunction::PluginActive ? isActive : !isActive);
					}
					return ReqState::Unknown;
				}
				return ReqState::False;
			}
			case ObjectFunction::FileExist:
			case ObjectFunction::FileNotExist:
			{
				if (!entry->GetObject().IsEmpty())
				{
					bool isExist = KxFile(GetRequirementFilePath(entry)).IsExist();
					return FromInt<ReqState>(objectFunc == ObjectFunction::FileExist ? isExist : !isExist);
				}
				return ReqState::False;
			}
		};
		return ReqState::Unknown;
	}
	kxf::Version IPackageManager::GetRequirementVersionFromBinaryFile(const PackageProject::RequirementItem* entry)
	{
		kxf::Version version;

		KxFile file(GetRequirementFilePath(entry));
		if (file.IsFileExist())
		{
			KxLibraryVersionInfo versionInfo = file.GetVersionInfo();
			if (versionInfo.IsOK())
			{
				auto TryWith = [&versionInfo](const kxf::String& name) -> kxf::Version
				{
					kxf::Version version(versionInfo.GetString(name));
					if (version.IsOK())
					{
						return version;
					}
					return KxNullVersion;
				};

				version = TryWith(entry->GetBinaryVersionKind());
				if (!version.IsOK())
				{
					version = TryWith("ProductVersion");
					if (!version.IsOK())
					{
						version = TryWith("FileVersion");
						if (!version.IsOK())
						{
							version = TryWith("ProductVersionString");
							if (!version.IsOK())
							{
								version = TryWith("FileVersionString");
							}
						}
					}
				}
			}

			// No versions available, use file modification time
			if (!version.IsOK())
			{
				version = file.GetFileTime(KxFILETIME_MODIFICATION);
			}
		}
		return version;
	}
	kxf::Version IPackageManager::GetRequirementVersionFromModManager(const PackageProject::RequirementItem* entry)
	{
		const IGameMod* modEntry = IModManager::GetInstance()->FindModByID(entry->GetID());
		if (modEntry)
		{
			return modEntry->GetVersion();
		}
		return KxNullVersion;
	}
	kxf::Version IPackageManager::GetRequirementVersion(const PackageProject::RequirementItem* entry)
	{
		kxf::Version modVersion = GetRequirementVersionFromModManager(entry);
		return modVersion.IsOK() ? modVersion : GetRequirementVersionFromBinaryFile(entry);
	}

	void IPackageManager::LoadRequirementsGroup(PackageProject::RequirementGroup& group, const kxf::XMLNode& rootNode)
	{
		using namespace PackageDesigner;
		using namespace PackageProject;

		for (kxf::XMLNode node = rootNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
		{
			auto& item = group.GetItems().emplace_back(std::make_unique<RequirementItem>(ReqType::System));
			item->SetID(KVarExp(node.GetAttribute("ID")));
			item->SetCategory(KVarExp(node.GetAttribute("Category")));
			item->SetName(KVarExp(node.GetFirstChildElement("Name").GetValue()));

			// Object
			kxf::XMLNode objectNode = node.GetFirstChildElement("Object");
			item->SetObject(objectNode.GetValue());
			item->SetObjectFunction(RequirementsSection::StringToObjectFunction(objectNode.GetAttribute("Function")));

			// Version
			kxf::XMLNode versionNode = node.GetFirstChildElement("Version");
			item->SetRequiredVersion(versionNode.GetValue());
			item->SetRequiredVersionOperator(ModPackageProject::StringToOperator(versionNode.GetAttribute("Operator"), false, RequirementsSection::ms_DefaultVersionOperator));
			item->SetBinaryVersionKind(versionNode.GetAttribute("BinaryVersionKind"));

			// Description
			item->SetDescription(node.GetFirstChildElement("Description").GetValue());

			// Dependencies
			kxf::XMLNode dependenciesArrayNode = node.GetFirstChildElement("Dependencies");
			for (kxf::XMLNode node = dependenciesArrayNode.GetFirstChildElement(); node.IsOK(); node = node.GetNextSiblingElement())
			{
				item->GetDependencies().emplace_back(node.GetValue());
			}
		}
	}

	IPackageManager::IPackageManager()
		:ManagerWithTypeInfo(ModPackagesModule::GetInstance())
	{
	}

	kxf::String IPackageManager::GetPackagesFolder() const
	{
		using namespace Application;
		return GetAInstanceOption(OName::Packages).GetAttribute(OName::Location);
	}
	void IPackageManager::SetPackagesFolder(const kxf::String& path) const
	{
		using namespace Application;
		GetAInstanceOption(OName::Packages).SetAttribute(OName::Location, path);
	}
}
