#pragma once
#include <Kortex/Kortex.hpp>
#include "Programs/IProgramItem.h"
#include "Programs/IProgramManager.h"
#include <kxf::UI::Framework/KxStdDialog.h>
#include <kxf::UI::Framework/KxTextBox.h>

namespace Kortex::ProgramManager
{
	class ProgramEditorDialog: public KxStdDialog
	{
		private:
			IProgramItem* m_Program = nullptr;

			wxWindow* m_ContentPanel = nullptr;
			KxTextBox* m_NameInput = nullptr;
			KxTextBox* m_ExecutableInput = nullptr;
			KxTextBox* m_WorkingFolderInput = nullptr;
			KxTextBox* m_ArgumentsInput = nullptr;

		private:
			wxOrientation GetViewLabelSizerOrientation() const override
			{
				return wxHORIZONTAL;
			}
			wxOrientation GetWindowResizeSide() const override
			{
				return wxHORIZONTAL;
			}
			
			void CreateUI(wxWindow* parent);
			void SetupValues();
			kxf::String BrowseForLocation(const kxf::String& path, bool isDirectory);

		public:
			ProgramEditorDialog(wxWindow* parent)
			{
				CreateUI(parent);
			}
			ProgramEditorDialog(wxWindow* parent, IProgramItem& program)
				:m_Program(&program)
			{
				CreateUI(parent);
			}

		public:
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_ContentPanel;
			}
			IProgramItem& Accept();
	};
}
