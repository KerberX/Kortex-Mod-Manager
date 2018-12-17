#pragma once
#include "stdafx.h"
#include "KDataViewListModel.h"

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
				Name,
				PriorityGroup,
			};

		private:
			const bool m_FullFeatured = false;

		protected:
			ModTagStore* m_Data = nullptr;
			IGameMod* m_GameMod = nullptr;
			const IModTag* m_PriorityGroupTag = nullptr;
			bool m_IsModified = false;
			bool m_AllowSave = false;

		protected:
			virtual void OnInitControl() override;

			virtual void GetEditorValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			virtual void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const override;
			virtual bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) override;
			virtual bool IsEnabledByRow(size_t row, const KxDataViewColumn* column) const override;

			void OnActivate(KxDataViewEvent& event);

		protected:
			const IModTag* FindStdTag(const wxString& tagID) const;
			bool HasPriorityGroupTag() const
			{
				return m_PriorityGroupTag != nullptr;
			}

		public:
			SelectorDisplayModel(bool isFullFeatured = false);

		public:
			virtual void SetDataVector(ModTagStore* tagStore = nullptr, IGameMod* mod = nullptr);
			virtual size_t GetItemCount() const override;
		
			IModTag* GetDataEntry(size_t index) const;

			bool IsFullFeatured() const
			{
				return m_FullFeatured;
			}
			bool IsModified() const
			{
				return m_IsModified;
			}
			void ApplyChanges();
			void SetAllowSave(bool allow = true)
			{
				m_AllowSave = allow;
			}
			wxWindow* GetWindow()
			{
				return OnGetDataViewWindow();
			}
	};
}