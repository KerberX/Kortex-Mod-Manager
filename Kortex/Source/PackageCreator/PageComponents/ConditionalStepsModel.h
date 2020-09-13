#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageCreator/VectorModel.h"
#include "PackageProject/ComponentsSection.h"
#include "PackageProject/FileDataSection.h"
#include <KxFramework/KxStdDialog.h>

namespace Kortex::PackageDesigner::PageComponentsNS
{
	class ConditionalStepsModel: public VectorModel<PackageProject::ConditionalComponentStep::Vector>
	{
		private:
			void OnInitControl() override;
	
			void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) override;
	
			void OnActivateItem(KxDataViewEvent& event);
			void OnContextMenu(KxDataViewEvent& event);
	
			void OnAddStep();
			void OnRemoveStep(const KxDataViewItem& item);
			void OnClearList();
			bool OnInsertItem(KxDataViewItem& currentItem, KxDataViewItem& droppedItem) override
			{
				OnInsertItemHelperUniquePtr(*GetDataVector(), currentItem, droppedItem);
				return true;
			}
	
		public:
			ConditionalStepsModel(WorkspaceDocument* controller)
			{
				m_Controller = controller;
			}
	
		public:
			PackageProject::ConditionalComponentStep* GetDataEntry(size_t index) const
			{
				if (index < GetItemCount())
				{
					return GetDataVector()->at(index).get();
				}
				return nullptr;
			}
	
			void SetDataVector();
			void SetDataVector(VectorType& data);
	};
}

namespace Kortex::PackageDesigner::PageComponentsNS
{
	class ConditionalStepsDialog: public KxStdDialog, public ConditionalStepsModel
	{
		private:
			wxWindow* m_ViewPane = nullptr;
			//KProgramOptionAI m_WindowOptions;
			//KProgramOptionAI m_ViewOptions;
	
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
			ConditionalStepsDialog(wxWindow* parent, const kxf::String& caption, WorkspaceDocument* controller, bool isAutomatic);
			~ConditionalStepsDialog();
	};
}
