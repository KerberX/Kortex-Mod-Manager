#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageCreator/VectorModel.h"
#include "PackageProject/InfoSection.h"
#include <KxFramework/KxStdDialog.h>

namespace Kortex::PackageDesigner::PageInfoNS
{
	class DocumentsModel: public VectorModel<Utility::LabeledValue::Vector>
	{
		private:
			PackageProject::InfoSection* m_InfoData = nullptr;
	
		private:
			void OnInitControl() override;
	
			void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) override;
	
			void OnActivateItem(KxDataViewEvent& event);
			void OnContextMenu(KxDataViewEvent& event);
	
			void OnAddEntry();
			void OnRemoveEntry(const KxDataViewItem& item);
			void OnClearList();
			bool OnInsertItem(KxDataViewItem& currentItem, KxDataViewItem& droppedItem) override
			{
				OnInsertItemHelperPrimitive(*GetDataVector(), currentItem, droppedItem);
				return true;
			}
			KxStringVector OpenFileDialog(bool isMultiple = true) const;
	
		public:
			const Utility::LabeledValue* GetDataEntry(size_t index) const
			{
				if (index < GetItemCount())
				{
					return &GetDataVector()->at(index);
				}
				return nullptr;
			}
			Utility::LabeledValue* GetDataEntry(size_t index)
			{
				if (index < GetItemCount())
				{
					return &GetDataVector()->at(index);
				}
				return nullptr;
			}
	
			void SetDataVector();
			void SetDataVector(VectorType& data, PackageProject::InfoSection* info);
	};
}

namespace Kortex::PackageDesigner::PageInfoNS
{
	class DocumentsDialog: public KxStdDialog, public DocumentsModel
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
	
		public:
			DocumentsDialog(wxWindow* parent, const kxf::String& caption, WorkspaceDocument* controller);
			virtual ~DocumentsDialog();
	};
}
