#include "stdafx.h"
#include "DefaultPackageManager.h"
#include "PackageCreator/Workspace.h"
#include <Kortex/Application.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/ModManager.hpp>
#include <kxf::UI::Framework/kxf::UI::Menu.h>
#include <kxf::UI::Framework/KxShell.h>
#include <kxf::UI::Framework/KxFileBrowseDialog.h>

namespace Kortex::PackageDesigner
{
	void DefaultPackageManager::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode)
	{
		LoadRequirementsGroup(m_StandardRequirements, managerNode.GetFirstChildElement("Requirements"));
		
		// Find script extender
		kxf::String xSEID = instance.GetVariables().GetVariable(Variables::KVAR_SCRIPT_EXTENDER_ID).AsString();
		if (PackageProject::RequirementItem* xSE = m_StandardRequirements.FindItem(xSEID))
		{
			m_WithScriptExtender.Assign(*xSE);
			AddComponent<IWithScriptExtender>(m_WithScriptExtender);
		}
	}
	void DefaultPackageManager::OnInit()
	{
	}
	void DefaultPackageManager::OnExit()
	{
	}
	void DefaultPackageManager::CreateWorkspaces()
	{
		new Workspace();
	}

	IWorkspace::RefVector DefaultPackageManager::EnumWorkspaces() const
	{
		return ToWorkspacesList(Workspace::GetInstance());
	}

	void DefaultPackageManager::OnModListMenu(kxf::UI::Menu& menu, const std::vector<IGameMod*>& selectedMods, IGameMod* focusedMod)
	{
		const bool isFixedMod = focusedMod->QueryInterface<ModManager::FixedGameMod>();
		const bool isPriorityGroup = focusedMod->QueryInterface<ModManager::PriorityGroup>();
		const bool isNormalMod = !isFixedMod && !isPriorityGroup;
		const bool isPackageExist = isNormalMod && focusedMod->IsPackageFileExist();

		kxf::UI::Menu* packageMenu = new kxf::UI::Menu();
		kxf::UI::MenuItem* packageMenuItem = menu.Add(packageMenu, KTr("ModManager.Menu.Package"));
		if (isNormalMod && (!focusedMod || selectedMods.size() != 1))
		{
			packageMenuItem->Enable(false);
			return;
		}

		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Package.Open")));
			item->Enable(isPackageExist);
			item->SetDefault();
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				IModManager::GetInstance()->InstallModFromPackage(focusedMod->GetPackageFile());
			});
		}
		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Package.OpenLocation")));
			item->Enable(isPackageExist);
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::FolderOpen));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				KxShell::OpenFolderAndSelectItem(focusedMod->GetPackageFile());
			});
		}
		packageMenu->AddSeparator();

		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Package.Assign")));
			item->Enable(!isFixedMod);
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::BoxSearchResult));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				KxFileBrowseDialog dialog(event.GetMenu()->GetWindow(), wxID_NONE, KxFBD_OPEN);
				dialog.SetFolder(focusedMod->GetPackageFile().BeforeLast('\\'));
				dialog.AddFilter("*.kmp;*.smi;*.fomod;*.7z;*.zip;", KTr("FileFilter.AllSupportedFormats"));
				dialog.AddFilter("*.kmp", KTr("FileFilter.ModPackage"));
				dialog.AddFilter("*.smi", KTr("FileFilter.ModPackageSMI"));
				dialog.AddFilter("*.fomod", KTr("FileFilter.ModPackageFOMod"));
				dialog.AddFilter("*.7z;*.zip;", KTr("FileFilter.Archives"));
				if (dialog.ShowModal() == wxID_OK)
				{
					focusedMod->SetPackageFile(dialog.GetResult());
					focusedMod->Save();

					BroadcastProcessor::Get().ProcessEvent(ModEvent::EvtChanged, *focusedMod);
				}
			});
		}
		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Package.Remove")));
			item->Enable(isPackageExist);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				KxShell::FileOperation(focusedMod->GetPackageFile(), KxFS_FILE, KxFOF_DELETE, true, false, event.GetMenu()->GetWindow());
			});
		}
		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Package.Extract")));
			item->Enable(isPackageExist);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				KxFileBrowseDialog dialog(event.GetMenu()->GetWindow(), wxID_NONE, KxFBD_OPEN_FOLDER);
				if (dialog.ShowModal() == wxID_OK)
				{
					// Extract archive in mod name folder inside the specified one.
					kxf::String outPath = dialog.GetResult() + wxS('\\') + focusedMod->GetSafeName();

					ExtractAcrhiveWithProgress(event.GetMenu()->GetWindow(), focusedMod->GetPackageFile(), outPath);
				}
			});
		}
		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Package.ImportProject")));
			item->Enable(isPackageExist && Workspace::GetInstance() != nullptr);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				Workspace* workspace = Workspace::GetInstance();

				workspace->ImportProjectFromPackage(focusedMod->GetPackageFile());
				workspace->SwitchHere();
			});
		}
		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Package.CreateProject")));
			item->Enable(isPackageExist);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				Workspace* workspace = Workspace::GetInstance();

				workspace->CreateProjectFromModEntry(*focusedMod);
				workspace->SwitchHere();
			});
		}
		packageMenu->AddSeparator();

		{
			kxf::UI::MenuItem* item = packageMenu->Add(new kxf::UI::MenuItem(KTr("ModManager.Menu.Properties")));
			item->Enable(isPackageExist);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [focusedMod](kxf::UI::MenuEvent& event)
			{
				KxShell::Execute(event.GetMenu()->GetWindow(), focusedMod->GetPackageFile(), wxS("properties"));
			});
		}
	}
}
