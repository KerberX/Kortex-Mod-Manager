#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageCreator/VectorModel.h"
#include "PackageProject/InterfaceSection.h"
#include "PackageCreator/IDTracker.h"
#include "Utility/BitmapSize.h"

namespace Kortex::PackageDesigner::PageInterfaceNS
{
	class ImageListModel: public VectorModel<PackageProject::ImageItem::Vector>, public IDTracker
	{
		public:
			static Utility::BitmapSize GetThumbnailSize(const wxWindow* window = nullptr);
			static void LoadBitmap(PackageProject::ImageItem* entry, const wxWindow* window = nullptr);
	
		private:
			PackageProject::InterfaceSection* m_Interface = nullptr;
	
		private:
			void OnInitControl() override;
	
			void GetValueByRow(wxAny& data, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& data, size_t row, const KxDataViewColumn* column) override;
	
			void OnActivateItem(KxDataViewEvent& event);
			void OnContextMenu(KxDataViewEvent& event);
			void OnCacheHint(KxDataViewEvent& event);
			void OnAllItemsMenuSelect(KxDataViewColumn* column) override;
	
			void OnImportFiles();
			void OnAddMultipleItems();
			void OnRemoveEntry(const KxDataViewItem& item);
			void OnClearList();
			bool OnInsertItem(KxDataViewItem& currentItem, KxDataViewItem& droppedItem) override
			{
				OnInsertItemHelperPrimitive(*GetDataVector(), currentItem, droppedItem);
				return true;
			}
	
			bool DoTrackImagePath(const kxf::String& trackedID, const kxf::String& newID, bool remove) const;
	
		protected:
			IDTracker* GetTracker() override
			{
				return this;
			}
			bool TrackChangeID(const kxf::String& trackedID, const kxf::String& newID) override
			{
				return DoTrackImagePath(trackedID, newID, false);
			}
			bool TrackRemoveID(const kxf::String& trackedID) override
			{
				return DoTrackImagePath(trackedID, wxEmptyString, true);
			}
	
		public:
			const PackageProject::ImageItem* GetDataEntry(size_t index) const
			{
				if (index < GetItemCount())
				{
					return &GetDataVector()->at(index);
				}
				return nullptr;
			}
			PackageProject::ImageItem* GetDataEntry(size_t index)
			{
				if (index < GetItemCount())
				{
					return &GetDataVector()->at(index);
				}
				return nullptr;
			}
	
			const PackageProject::ImageItem* GetDataEntry(const KxDataViewItem& item) const
			{
				return GetDataEntry(GetRow(item));
			}
	
			void SetProject(ModPackageProject& projectData);
	};
}
