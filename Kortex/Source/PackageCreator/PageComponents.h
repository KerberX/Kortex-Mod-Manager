#pragma once
#include <Kortex/Kortex.hpp>
#include "PageBase.h"
#include "PackageProject/ComponentsSection.h"
class KxImageView;

namespace Kortex::PackageDesigner
{
	class Workspace;
}
namespace Kortex::PackageProject
{
	class ComponentsSection;
}
namespace Kortex::PackageDesigner::PageComponentsNS
{
	class FileDataSelectorComboBox;
	class ComponentsModel;
}

namespace Kortex::PackageDesigner
{
	class PageComponents: public PageBase
	{
		friend class Workspace;

		public:
			static kxf::String FormatArrayToText(const KxStringVector& array);
			static kxf::String ConditionToString(const PackageProject::Condition& condition, bool isRequired);
			static kxf::String ConditionGroupToString(const PackageProject::ConditionGroup& conditionGroup);

		private:
			//KProgramOptionAI m_MainOptions;
			//KProgramOptionAI m_ComponentsOptions;
			
			wxBoxSizer* m_MainSizer = nullptr;
			PageComponentsNS::ComponentsModel* m_ComponentsModel = nullptr;
			
			// Misc controls
			PageComponentsNS::FileDataSelectorComboBox* m_RequiredFilesModel = nullptr;
			KxImageView* m_EntryImage = nullptr;

		private:
			PackageProject::ComponentsSection& GetProjectComponents() const;
			void OnLoadProject(PackageProject::ComponentsSection& projectComponents);
			
			void CreateComponentsView();
			void CreateMiscControls();

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;

		public:
			PageComponents(Workspace& mainWorkspace, WorkspaceDocument& controller);
			~PageComponents();

		public:
			kxf::ResourceID GetIcon() const override
			{
				return ImageResourceID::Block;
			}
			kxf::String GetID() const override;
			kxf::String GetPageName() const override;
	};
}
