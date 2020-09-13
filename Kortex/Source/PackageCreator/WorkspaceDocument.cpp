#include "stdafx.h"
#include "WorkspaceDocument.h"
#include "Workspace.h"
#include "PackageBuilder.h"
#include <Kortex/Application.hpp>
#include <Kortex/ModManager.hpp>
#include "PackageProject/Common.h"
#include "PackageProject/Serializer/NativeSerializer.h"
#include "PackageProject/Serializer/FOModSerializer.h"
#include "ModPackages/ModPackage.h"
#include "Utility/OperationWithProgress.h"
#include <Kortex/Application.hpp>
#include <KxFramework/KxTaskDialog.h>
#include <KxFramework/KxTextFile.h>

namespace Kortex::PackageDesigner
{
	kxf::String WorkspaceDocument::GetNewProjectName()
	{
		return KTr("PackageCreator.NewProjectName");
	}

	kxf::String WorkspaceDocument::GetSaveConfirmationCaption() const
	{
		return KTr("PackageCreator.SaveChanges.Caption");
	}
	kxf::String WorkspaceDocument::GetSaveConfirmationMessage() const
	{
		return KTr("PackageCreator.SaveChanges.Message");
	}

	kxf::String WorkspaceDocument::GetProjectFileName() const
	{
		kxf::String name = m_ProjectFile.AfterLast('\\');
		return name.IsEmpty() ? GetNewProjectName() : name;
	}
	kxf::String WorkspaceDocument::GetProjectName() const
	{
		// Name -> ID -> translated name -> project file name
		if (m_Project)
		{
			const kxf::String& name = m_Project->GetInfo().GetName();
			if (!name.IsEmpty())
			{
				return name;
			}
			else
			{
				const kxf::String& id = m_Project->GetModID();
				if (!id.IsEmpty())
				{
					return id;
				}
				else
				{
					const kxf::String& translatedName = m_Project->GetInfo().GetTranslatedName();
					if (!translatedName.IsEmpty())
					{
						return translatedName;
					}
					else
					{
						kxf::String fileName = m_ProjectFile.AfterLast('\\').BeforeFirst('.');
						if (!fileName.IsEmpty())
						{
							return fileName;
						}
					}
				}
			}
		}
		return GetNewProjectName();
	}

	void WorkspaceDocument::ChangeNotify()
	{
		m_HasChanges = true;
		m_Workspace.RefreshWindowTitleForProject();

		BroadcastProcessor::Get().ProcessEvent(EvtChanged);
	}
	void WorkspaceDocument::SaveChanges()
	{
		PackageProject::NativeSerializer serializer(true);
		serializer.Serialize(*m_Project);
		KxTextFile::WriteToFile(m_ProjectFile, serializer.GetData());

		m_HasChanges = false;
		m_Workspace.RefreshWindowTitleForProject();

		BroadcastProcessor::Get().ProcessEvent(EvtSaved);
	}
	void WorkspaceDocument::DiscardChanges()
	{
		m_HasChanges = false;
		NewProject();

		BroadcastProcessor::Get().ProcessEvent(EvtDiscarded);
	}

	void WorkspaceDocument::NewProject()
	{
		m_ProjectFile.Clear();
		m_Project = std::make_unique<ModPackageProject>();
		Reload();
	}
	void WorkspaceDocument::OpenProject(const kxf::String& filePath)
	{
		m_ProjectFile = filePath;
		m_Project = std::make_unique<ModPackageProject>();

		PackageProject::NativeSerializer serializer(true);
		serializer.SetData(KxTextFile::ReadToString(filePath));
		serializer.Structurize(*m_Project);

		Reload();
	}
	void WorkspaceDocument::SaveProject()
	{
		SaveChanges();
	}
	void WorkspaceDocument::SaveProject(const kxf::String& filePath)
	{
		m_ProjectFile = filePath;
		SaveChanges();
	}
	void WorkspaceDocument::ImportProjectFromPackage(const kxf::String& packagePath)
	{
		m_ProjectFile.Clear();
		m_Project = std::make_unique<ModPackageProject>();

		ModPackage(packagePath, *m_Project);
		m_Project->GetConfig().SetInstallPackageFile(packagePath);

		Reload();
	}
	void WorkspaceDocument::CreateProjectFromModEntry(const IGameMod& modEntry)
	{
		m_ProjectFile.Clear();
		m_Project = std::make_unique<ModPackageProject>();

		/* Info and config */
		m_Project->SetModID(modEntry.GetID());

		m_Project->GetConfig().SetInstallPackageFile(modEntry.GetPackageFile());

		PackageProject::InfoSection& info = m_Project->GetInfo();
		info.SetName(modEntry.GetName());
		info.SetVersion(modEntry.GetVersion());
		info.SetAuthor(modEntry.GetAuthor());
		info.SetDescription(modEntry.GetDescription());
		info.GetModSourceStore() = modEntry.GetModSourceStore();
		info.GetTagStore() = modEntry.GetTagStore();

		/* Interface */
		PackageProject::ImageItem& imageEntry = m_Project->GetInterface().GetImages().emplace_back(PackageProject::ImageItem());
		imageEntry.SetPath(modEntry.GetInfoFile());
		imageEntry.SetVisible(true);

		m_Project->GetInterface().SetMainImage(imageEntry.GetPath());

		Reload();
	}
	void WorkspaceDocument::ImportProject(PackageProject::Serializer& serializer)
	{
		m_ProjectFile.Clear();
		m_Project = std::make_unique<ModPackageProject>();
		serializer.Structurize(*m_Project);
		Reload();
	}
	void WorkspaceDocument::ExportProject(PackageProject::Serializer& serializer)
	{
		serializer.Serialize(*m_Project);
	}
	void WorkspaceDocument::BuildProject(bool buildPreview)
	{
		auto thread = new PackageBuilderOperation(*m_Project, buildPreview);
		thread->SetDialogCaption(KTrf("PackageCreator.Build.Caption", GetProjectName()));
		thread->Run();
	}

	void WorkspaceDocument::Reload()
	{
		ResetView();
		LoadView();
		m_Workspace.RefreshWindowTitleForProject();
	}
	void WorkspaceDocument::LoadView()
	{
		m_Workspace.DoLoadAllPages();
	}
	void WorkspaceDocument::ResetView()
	{
	}
}
