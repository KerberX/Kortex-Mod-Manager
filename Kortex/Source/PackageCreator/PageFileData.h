#pragma once
#include <Kortex/Kortex.hpp>
#include "PageBase.h"

namespace Kortex
{
	class Workspace;
}
namespace Kortex::PackageProject
{
	class FileDataSection;
}
namespace Kortex::PackageDesigner::PageFileDataNS
{
	class MainListModel;
	class ContentModel;
}

namespace Kortex::PackageDesigner
{
	class PageFileData: public PageBase
	{
		friend class Workspace;

		private:
			KxSplitterWindow* m_Pane = nullptr;
			//KProgramOptionAI m_MainOptions;

			// Folders
			KxPanel* m_MainListPane = nullptr;
			PageFileDataNS::MainListModel* m_MainListModel = nullptr;
			//KProgramOptionAI m_MainListOptions;

			// Files
			KxPanel* m_FolderContentPane = nullptr;
			PageFileDataNS::ContentModel* m_ContentListModel = nullptr;
			//KProgramOptionAI m_ContentListModelOptions;

		private:
			void OnLoadProject(PackageProject::FileDataSection& projectFileData);
			PackageProject::FileDataSection& GetProjectFileData() const;

			void CreateMainListControls();
			void CreateFolderContentControls();

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;

		public:
			PageFileData(Workspace& mainWorkspace, WorkspaceDocument& controller);
			~PageFileData();

		public:
			kxf::ResourceID GetIcon() const override
			{
				return Imagekxf::ResourceID::Folder;
			}
			kxf::String GetID() const override;
			kxf::String GetPageName() const override;

			PageFileDataNS::MainListModel* GetMainListModel() const
			{
				return m_MainListModel;
			}
			PageFileDataNS::ContentModel* GetFolderContentModel() const
			{
				return m_ContentListModel;
			}
	};
}
