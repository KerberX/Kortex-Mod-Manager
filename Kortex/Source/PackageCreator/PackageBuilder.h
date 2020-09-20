#pragma once
#include <Kortex/Kortex.hpp>
#include "Utility/OperationWithProgress.h"
#include "Utility/TempFolderKeeper.h"
#include <kxf::UI::Framework/KxArchiveEvent.h>

namespace Kortex
{
	class ModPackageProject;
	class GenericArchive;
}
namespace Kortex::PackageProject
{
	class FileItem;
}

namespace Kortex::PackageDesigner
{
	enum class BuildError
	{
		Success = 0,
		Generic,
		PackagePath,
	};

	class PackageBuilder: public Utility::TempFolderKeeper
	{
		friend class PackageBuilderOperation;

		private:
			const ModPackageProject& m_Project;
			Utility::OperationWithProgressBase& m_Thread;	
			kxf::String m_PackagePath;
			const bool m_BuildPreview = false;

			std::unique_ptr<GenericArchive> m_Archive;
			KxStringVector m_SourceFiles;
			KxStringVector m_ArchivePaths;
			KxStringVector m_MissingFiles;
			BuildError m_Status = BuildError::Generic;

		private:
			kxf::String GetTempFolder() const;
			kxf::String GetTempPackagePath() const;
			kxf::String GetImagePath(const kxf::String& fileName) const;
			kxf::String GetDocumentPath(const kxf::String& fileName) const;
			kxf::String GetFileDataEntryPath(const PackageProject::FileItem* fileDataEntry, const kxf::String& fileName) const;

			void SetPackagePath(const kxf::String& path)
			{
				m_PackagePath = path;
			}

		private:
			virtual void CheckProject();
			virtual void Configure();
			virtual void WritePackageConfig();
			virtual void ProcessInfo();
			virtual void ProcessInterface();
			virtual void ProcessFileData();

		public:
			PackageBuilder(const ModPackageProject& project, Utility::OperationWithProgressBase& thread, bool previewBuild = false);
			~PackageBuilder();

		public:
			BuildError GetStatus() const
			{
				return m_Status;
			}
			bool IsOK() const
			{
				return m_Status == BuildError::Success;
			}	
			KxStringVector GetMissingFiles() const
			{
				return m_MissingFiles;
			}
		
			const kxf::String& GetPackagePath() const;
			bool IsPrevievBuild() const
			{
				return m_BuildPreview;
			}

			bool Run();
	};
}

namespace Kortex::PackageDesigner
{
	class PackageBuilderOperation: public Utility::OperationWithProgressDialog<KxArchiveEvent>
	{
		private:
			const ModPackageProject& m_Project;
			kxf::String m_PackagePath;
			bool m_BuildPreview = false;

			KxStringVector m_MissingFiles;
			BuildError m_CheckStatus = BuildError::Generic;
			bool m_BuildOK = false;
			bool m_Cancelled = false;

		private:
			void EntryHandler();
			void OnEndHandler();

		public:
			PackageBuilderOperation(const ModPackageProject& project, bool previewBuild = false);

		public:
			void SetPrevievBuild(bool value)
			{
				m_BuildPreview = value;
			}
	};
}
