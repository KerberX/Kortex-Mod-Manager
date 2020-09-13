#pragma once
#include <Kortex/Kortex.hpp>
#include <KxFramework/KxTextBoxDialog.h>

namespace Kortex::ModManager
{
	class NewModDialog: public KxTextBoxDialog
	{
		private:
			kxf::String m_Name;

		private:
			void OnOK(wxNotifyEvent& event);

		public:
			NewModDialog(wxWindow* parent);
			virtual ~NewModDialog();

		public:
			kxf::String GetFolderName() const
			{
				return m_Name;
			}
	};
}

namespace Kortex::ModManager
{
	class NewModFromFolderDialog: public NewModDialog
	{
		private:
			wxCheckBox* m_AsLinkedModCB = nullptr;

			bool m_IsLinkedMod = false;
			kxf::String m_FolderPath;

		private:
			void OnSelectFolder(wxNotifyEvent& event);
			void OnChangeMethod(wxCommandEvent& event);

			kxf::String GetMethodString(bool bLink) const;
			kxf::String GetMethodString() const
			{
				return GetMethodString(m_AsLinkedModCB->GetValue());
			}
			void UpdateLabelText();

		public:
			NewModFromFolderDialog(wxWindow* parent);
			virtual ~NewModFromFolderDialog();

		public:
			kxf::String GetFolderPath() const
			{
				return m_FolderPath;
			}
			bool ShouldCreateAsLinkedMod() const
			{
				return m_IsLinkedMod;
			}
	};
}
