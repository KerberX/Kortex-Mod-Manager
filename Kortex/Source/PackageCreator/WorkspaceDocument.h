#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IWorkspaceDocument.h"
#include "PackageProject/ModPackageProject.h"

namespace Kortex
{
	class ModPackage;
	class IGameMod;
}
namespace Kortex::PackageProject
{
	class Serializer;
}

namespace Kortex::PackageDesigner
{
	class Workspace;

	class WorkspaceDocument: public IWorkspaceDocument
	{
		public:
			static kxf::String GetNewProjectName();

		private:
			Workspace& m_Workspace;
			std::unique_ptr<ModPackageProject> m_Project = nullptr;
			kxf::String m_ProjectFile;
			bool m_HasChanges = false;

		protected:
			kxf::String GetSaveConfirmationCaption() const override;
			kxf::String GetSaveConfirmationMessage() const override;

		public:
			WorkspaceDocument(Workspace& workspace)
				:m_Workspace(workspace)
			{
			}

		public:
			ModPackageProject* GetProject() const
			{
				return m_Project.get();
			}
			const kxf::String& GetProjectFilePath() const
			{
				return m_ProjectFile;
			}
			kxf::String GetProjectFileName() const;
			kxf::String GetProjectName() const;
			bool HasProjectFilePath() const
			{
				return !m_ProjectFile.IsEmpty();
			}

			void ChangeNotify();
			bool HasUnsavedChanges() const override
			{
				return m_HasChanges;
			}
			void SaveChanges() override;
			void DiscardChanges() override;

			void NewProject();
			void OpenProject(const kxf::String& filePath);
			void SaveProject();
			void SaveProject(const kxf::String& filePath);
			void ImportProjectFromPackage(const kxf::String& packagePath);
			void CreateProjectFromModEntry(const IGameMod& modEntry);
			void ImportProject(PackageProject::Serializer& serializer);
			void ExportProject(PackageProject::Serializer& serializer);
			void BuildProject(bool buildPreview = false);

			void Reload();
			void LoadView();
			void ResetView();
	};
}
