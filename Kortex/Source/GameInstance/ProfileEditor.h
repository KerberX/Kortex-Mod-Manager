#pragma once
#include <Kortex/Kortex.hpp>
#include "IGameProfile.h"
#include "kxf::UI::Framework/KxDataViewListModelEx.h"
#include <kxf::UI::Framework/KxStdDialog.h>
class KxButton;
class KxCheckBox;

namespace Kortex::ProfileEditor
{
	class DisplayModel: public KxDataViewVectorListModelEx<IGameProfile::Vector, KxDataViewListModelEx>
	{
		private:
			bool m_IsModified = false;
			kxf::String m_NewCurrentProfile;

		protected:
			void OnInitControl() override;

			void GetEditorValueByRow(wxAny& data, size_t row, const KxDataViewColumn* column) const override;
			void GetValueByRow(wxAny& data, size_t row, const KxDataViewColumn* column) const override;
			bool SetValueByRow(const wxAny& data, size_t row, const KxDataViewColumn* column) override;
			bool IsEnabledByRow(size_t row, const KxDataViewColumn* column) const override;

			void OnActivate(KxDataViewEvent& event);
			void MarkModified()
			{
				m_IsModified = true;
			}
			void SetNewProfile(const kxf::String& id)
			{
				m_NewCurrentProfile = id;
			}

		public:
			DisplayModel();

		public:
			bool IsModified() const
			{
				return m_IsModified;
			}
			const kxf::String& GetNewProfile() const
			{
				return m_NewCurrentProfile;
			}

			const IGameProfile* GetDataEntry(size_t i) const
			{
				if (i < GetItemCount())
				{
					return &*GetDataVector()->at(i);
				}
				return nullptr;
			}
			IGameProfile* GetDataEntry(size_t i)
			{
				if (i < GetItemCount())
				{
					return &*GetDataVector()->at(i);
				}
				return nullptr;
			}
	};
}

namespace Kortex::ProfileEditor
{
	class Dialog: public KxStdDialog, public DisplayModel
	{
		private:
			wxWindow* m_ViewPane = nullptr;
			KxButton* m_AddButton = nullptr;
			KxButton* m_CopyButton = nullptr;
			KxButton* m_RemoveButton = nullptr;

		private:
			virtual wxWindow* GetDialogMainCtrl() const override
			{
				return m_ViewPane;
			}
		
			void OnSelectProfile(KxDataViewEvent& event);
			void OnAddProfile(wxCommandEvent& event);
			void OnCopyProfile(wxCommandEvent& event);
			void OnRemoveProfile(wxCommandEvent& event);

		public:
			Dialog(wxWindow* parent);
			virtual ~Dialog();
	};
}
