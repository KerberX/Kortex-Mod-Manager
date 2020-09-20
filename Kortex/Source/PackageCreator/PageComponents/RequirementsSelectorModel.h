#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageCreator/VectorModel.h"
#include "PackageProject/ComponentsSection.h"
#include "PackageProject/RequirementsSection.h"
#include <kxf::UI::Framework/KxStdDialog.h>

namespace Kortex::PackageDesigner::PageComponentsNS
{
	using RequirementsSelectorItem = std::pair<PackageProject::RequirementGroup*, bool>;

	class RequirementsSelectorModel: public VectorModel<std::vector<RequirementsSelectorItem>>
	{
		private:
			PackageProject::RequirementsSection* m_ReqData = nullptr;
			std::vector<RequirementsSelectorItem> m_DataVector;
	
		private:
			void OnInitControl() override;
	
			void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) override;
			bool OnInsertItem(KxDataViewItem& currentItem, KxDataViewItem& droppedItem) override
			{
				OnInsertItemHelperPrimitive(*GetDataVector(), currentItem, droppedItem);
				return true;
			}
	
		public:
			const RequirementsSelectorItem* GetDataEntry(size_t index) const
			{
				if (index < GetItemCount())
				{
					return &GetDataVector()->at(index);
				}
				return nullptr;
			}
			RequirementsSelectorItem* GetDataEntry(size_t index)
			{
				if (index < GetItemCount())
				{
					return &GetDataVector()->at(index);
				}
				return nullptr;
			}
	
			void SetDataVector();
			void SetDataVector(const KxStringVector& data, PackageProject::RequirementsSection* reqData);
			KxStringVector GetSelectedItems() const;
	};
}

namespace Kortex::PackageDesigner::PageComponentsNS
{
	class RequirementsSelectorDialog: public KxStdDialog, public RequirementsSelectorModel
	{
		private:
			wxWindow* m_ViewPane = nullptr;
	
		private:
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_ViewPane;
			}
			wxWindow* GetDialogFocusCtrl() const override
			{
				return GetView();
			}
	
		public:
			RequirementsSelectorDialog(wxWindow* parent, const kxf::String& caption, WorkspaceDocument* controller);
			~RequirementsSelectorDialog();
	};
}
