#pragma once
#include <Kortex/Kortex.hpp>
#include "SelectorDisplayModel.h"
class KxDataViewComboBox;

namespace Kortex::ModTagManager
{
	class SelectorDisplayModelCB: public SelectorDisplayModel
	{
		private:
			KxDataViewComboBox* m_ComboView = nullptr;

		protected:
			virtual KxDataViewCtrl* OnCreateDataView(wxWindow* window) override;
			virtual wxWindow* OnGetDataViewWindow() override;
			virtual bool SetValueByRow(const wxAny& data, size_t row, const KxDataViewColumn* column) override;
			virtual bool IsEditorEnabledByRow(size_t row, const KxDataViewColumn* column) const override;

		private:
			kxf::String DoGetStingValue() const;
			void SetStringValue(const kxf::String& value);
			void OnGetStringValue(KxDataViewEvent& event);

		public:
			void SetDataVector(ModTagStore& tagStore)
			{
				SelectorDisplayModel::SetDataVector(tagStore);
				SetStringValue(m_TagStore ? DoGetStingValue() : wxEmptyString);
			}
			void SetDataVector(ModTagStore& tagStore, IGameMod& mod)
			{
				SelectorDisplayModel::SetDataVector(tagStore, mod);
			}
	};
}
