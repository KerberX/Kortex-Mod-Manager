#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageCreator/VectorModel.h"
#include "PackageCreator/IDTracker.h"
#include "PackageProject/FileDataSection.h"
class KxDataViewSpinRenderer;

namespace Kortex::Utility
{
	class OperationWithProgressBase;
}

namespace Kortex::PackageDesigner::PageFileDataNS
{
	class ContentModel;

	class MainListModel: public VectorModel<PackageProject::FileItem::Vector>, public IDTracker
	{
		private:
			PackageProject::FileDataSection* m_FileData = nullptr;
			ContentModel* m_ContentModel = nullptr;
			
			KxDataViewSpinEditor* m_PriorityRenderer = nullptr;
	
		private:
			void OnInitControl() override;
	
			void GetEditorValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) override;
	
			void AddEverythingFromPath(const kxf::String& filePath, PackageProject::FolderItem& fileEntry, Utility::OperationWithProgressBase& context);
			bool DoTrackID(const kxf::String& trackedID, const kxf::String& newID, bool remove);
	
			void OnActivateItem(KxDataViewEvent& event);
			void OnSelectItem(KxDataViewEvent& event);
			void OnContextMenu(KxDataViewEvent& event);
			void OnAllItemsMenuSelect(KxDataViewColumn* column) override;
	
			void OnAddFile();
			void OnAddFolder();
			void OnAddMultipleFolders();
			void OnReplaceFolderContent(const KxDataViewItem& item, PackageProject::FolderItem& folderEntry);
			void OnRemoveElement(const KxDataViewItem& item);
			void OnClearList();
			bool OnInsertItem(KxDataViewItem& currentItem, KxDataViewItem& droppedItem) override
			{
				OnInsertItemHelperUniquePtr(*GetDataVector(), currentItem, droppedItem);
				return true;
			}
	
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
			PackageProject::FileItem* GetDataEntry(size_t index) const
			{
				if (index < GetItemCount())
				{
					return GetDataVector()->at(index).get();
				}
				return nullptr;
			}
	
			void SetProject(ModPackageProject& projectData);
			void SetContentModel(ContentModel* content)
			{
				m_ContentModel = content;
			}
	};
}
