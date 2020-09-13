#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageCreator/VectorModel.h"
#include "PackageCreator/IDTracker.h"
#include "PackageProject/RequirementsSection.h"
class KxButton;
class KxDataViewComboBox;

namespace Kortex::PackageDesigner::PageRequirementsNS
{
	class EntriesListModel;

	class GroupsModel: public VectorModel<PackageProject::RequirementGroup::Vector>, public IDTracker
	{
		private:
			PackageProject::RequirementsSection* m_Requirements = nullptr;
			KxDataViewComboBox* m_ComboView = nullptr;
			EntriesListModel* m_EntriesModel = nullptr;
			KxButton* m_AddButton = nullptr;
			KxButton* m_RemoveButton = nullptr;
	
		private:
			KxDataViewCtrl* OnCreateDataView(wxWindow* window) override;
			wxWindow* OnGetDataViewWindow() override;
			void OnInitControl() override;
	
			void GetEditorValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) override;
	
			void OnSelectItem(KxDataViewEvent& event);
			void OnActivateItem(KxDataViewEvent& event);
			void OnContextMenu(KxDataViewEvent& event);
			void OnGetStringValue(KxDataViewEvent& event);
	
			void OnAddGroup(bool useDialog = false);
			void OnRemoveGroup(const KxDataViewItem& item);
			void OnClearList();
			bool OnInsertItem(KxDataViewItem& currentItem, KxDataViewItem& droppedItem) override
			{
				OnInsertItemHelperUniquePtr(*GetDataVector(), currentItem, droppedItem);
				return true;
			}
	
			void RefreshComboControl();
			bool DoTrackID(const kxf::String& trackedID, const kxf::String& newID, bool remove) const;
	
		protected:
			IDTracker* GetTracker() override
			{
				return this;
			}
			bool TrackChangeID(const kxf::String& trackedID, const kxf::String& newID) override
			{
				return DoTrackID(trackedID, newID, false);
			}
			bool TrackRemoveID(const kxf::String& trackedID) override
			{
				return DoTrackID(trackedID, wxEmptyString, true);
			}
	
		public:
			void Create(WorkspaceDocument* controller, wxWindow* window, wxSizer* sizer) override;
			void ChangeNotify() override;
	
			PackageProject::RequirementGroup* GetDataEntry(size_t index) const
			{
				if (index < GetItemCount())
				{
					return (*GetDataVector())[index].get();
				}
				return nullptr;
			}
			PackageProject::RequirementGroup* GetDataEntry(const KxDataViewItem& item) const
			{
				return GetDataEntry(GetRow(item));
			}
			
			void SetProject(ModPackageProject& projectData);
			void SetEntriesModel(EntriesListModel* model)
			{
				m_EntriesModel = model;
			}
	};
}
