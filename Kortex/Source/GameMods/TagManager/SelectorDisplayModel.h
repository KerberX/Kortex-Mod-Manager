#pragma once
#include <Kortex/Kortex.hpp>
#include "KxFramework/KxDataViewListModelEx.h"

namespace Kortex
{
	class IModTag;
	class ModTagStore;
	class IGameMod;
}

namespace Kortex::ModTagManager
{
	class SelectorDisplayModel: public KxDataViewListModelEx
	{
		protected:
			enum ColumnID
			{
				PriorityTag,
				Expanded,
				Name,
				NexusID,
				Color,
			};

		private:
			const bool m_FullFeatured = false;

		protected:
			IGameMod* m_Mod = nullptr;
			ModTagStore* m_TagStore = nullptr;
			const IModTag* m_PrimaryTag = nullptr;
			bool m_IsModified = false;

		protected:
			void OnInitControl() override;

			void GetEditorValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) override;
			bool IsEnabledByRow(size_t row, const KxDataViewColumn* column) const override;
			bool GetItemAttributesByRow(size_t row, const KxDataViewColumn* column, KxDataViewItemAttributes& attribute, KxDataViewCellState cellState) const override;

			void OnActivate(KxDataViewEvent& event);
			void OnKeyDown(wxKeyEvent& event);

		public:
			SelectorDisplayModel(bool isFullFeatured = false);

		public:
			void SetDataVector(ModTagStore& tagStore, IGameMod& mod);
			void SetDataVector(ModTagStore& tagStore);
			size_t GetItemCount() const override;
			IModTag* GetDataEntry(size_t index) const;

			bool IsFullFeatured() const
			{
				return m_FullFeatured;
			}
			bool IsModified() const
			{
				return m_IsModified;
			}
			
			bool HasPriorityGroupTag() const
			{
				return m_PrimaryTag != nullptr;
			}
			void ApplyChanges();
	};
}
