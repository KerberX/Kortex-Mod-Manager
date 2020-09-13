#pragma once
#include <Kortex/Kortex.hpp>
#include "PageBase.h"
class KxComboBox;
class KxButton;

namespace Kortex::PackageProject
{
	class RequirementsSection;
}
namespace Kortex::PackageDesigner
{
	class Workspace;
}
namespace Kortex::PackageDesigner::PageRequirementsNS
{
	class GroupsModel;
	class EntriesListModel;
}

namespace Kortex::PackageDesigner
{
	class PageRequirements: public PageBase
	{
		friend class Workspace;

		private:
			//KProgramOptionAI m_MainOptions;
			
			wxBoxSizer* m_MainSizer = nullptr;
			
			// Sets list
			PageRequirementsNS::GroupsModel* m_GroupsModel = nullptr;
			KxButton* m_DefaultGroupsButton = nullptr;
			//KProgramOptionAI m_GroupsModelOptions;

			// Requirements list
			PageRequirementsNS::EntriesListModel* m_EntriesModel = nullptr;
			//KProgramOptionAI m_EntriesModelOptions;

			// Std requirements list
			KxComboBox* m_StdReqs_Categories = nullptr;
			KxComboBox* m_StdReqs_List = nullptr;
			KxButton* m_StdReqs_Add = nullptr;

		private:
			void OnLoadProject(PackageProject::RequirementsSection& projectRequirements);
			PackageProject::RequirementsSection& GetProjectRequirements() const;
			void SelectComboBoxItem(KxComboBox* control, int itemIndex);
		
			void CreateGroupsControls();
			void CreateEntriesControls();
			void CreateStdReqsControls();
			void LoadStdReqs();
			void OnSelectStdReqCategory(wxCommandEvent& event);
			void OnSelectStdReq(wxCommandEvent& event);
			void OnAddStdReq(wxCommandEvent& event);

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;

		public:
			PageRequirements(Workspace& mainWorkspace, WorkspaceDocument& controller);
			~PageRequirements();

		public:
			kxf::ResourceID GetIcon() const override
			{
				return Imagekxf::ResourceID::Cheque;
			}
			kxf::String GetID() const override;
			kxf::String GetPageName() const override;
	};
}
