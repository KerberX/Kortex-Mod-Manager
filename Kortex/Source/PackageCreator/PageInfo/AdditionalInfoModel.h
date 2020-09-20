#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageCreator/VectorModel.h"
#include "PackageProject/InfoSection.h"
#include <kxf::UI::Framework/KxStdDialog.h>

namespace Kortex::PackageDesigner::PageInfoNS
{
	class AdditionalInfoModel: public VectorModel<Utility::LabeledValue::Vector>
	{
		private:
			PackageProject::InfoSection* m_InfoData = nullptr;
			bool m_UseInlineEditor = false;
			
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
	
			Utility::LabeledValue* GetDataEntry(const KxDataViewItem& item)
			{
				return GetDataEntry(GetRow(item));
			}
			const Utility::LabeledValue* GetDataEntry(const KxDataViewItem& item) const
			{
				return GetDataEntry(GetRow(item));
			}
	
			void SetDataVector();
			void SetDataVector(VectorType& data, PackageProject::InfoSection* info);
	
			void UseInlineEditor(bool value)
			{
				m_UseInlineEditor = value;
			}
	};
}

namespace Kortex::PackageDesigner::PageInfoNS
{
	class AdditionalInfoDialog: public KxStdDialog, public AdditionalInfoModel
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
			AdditionalInfoDialog(wxWindow* parent, const kxf::String& caption, WorkspaceDocument* controller, bool useInloneEditor = false);
			virtual ~AdditionalInfoDialog();
	};
}
