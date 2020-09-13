#pragma once
#include <Kortex/Kortex.hpp>
#include <KxFramework/KxStdDialog.h>
#include "WebView.h"
class KxPanel;
class KxHTMLWindow;
class KxBitmapComboBox;
class KxStyledTextBox;
class KxAuiToolBar;
class KxAuiToolBarItem;
class KxAuiToolBarEvent;

namespace Kortex::UI
{
	class TextEditDialog: public KxStdDialog
	{
		private:
			KxAuiToolBar* m_ToolBar = nullptr;
			KxAuiToolBarItem* m_ToolBar_SwitchMode = nullptr;
			KxAuiToolBarItem* m_ToolBar_Save = nullptr;
			KxAuiToolBarItem* m_ToolBar_Open = nullptr;
			KxBitmapComboBox* m_HeadingList = nullptr;

			KxPanel* m_View = nullptr;
			wxSimplebook* m_Container = nullptr;

			KxStyledTextBox* m_Editor = nullptr;
			WebView m_Preview;
			
			kxf::String m_Text;
			bool m_IsTextModified = false;
			bool m_EditMode = true;
			bool m_Editable = true;

		private:
			bool Create(wxWindow* parent);
			KxHTMLWindow* GetHTMLBackend();

		private:
			int GetViewSizerProportion() const override
			{
				return 1;
			}
			wxOrientation GetViewSizerOrientation() const override
			{
				return wxVERTICAL;
			}
			wxOrientation GetViewLabelSizerOrientation() const override
			{
				return wxHORIZONTAL;
			}
			bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* id = nullptr) const override
			{
				return true;
			}
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_View;
			}

			void OnNewTextSet();
			void OnPrepareSaveText();
			void ClearUndoHistory();
			void OnSwitchMode(KxAuiToolBarEvent& event);
			void OnKey(wxKeyEvent& event);
			void OnOK(wxNotifyEvent& event);
			void OnSaveLoadFile(KxAuiToolBarEvent& event);

			void ToggleTag(const kxf::String& tagStart, const kxf::String& tagEnd);
			void ToggleTag(const kxf::String& tagName, const kxf::String& attributeName, const kxf::String& attributeValue)
			{
				ToggleTag(kxf::String::Format(" %s=\"%s\"", attributeName, attributeValue), tagName);
			}
			void ToggleTag(const kxf::String& tagName)
			{
				ToggleTag(tagName, tagName);
			}
			void LoadFromFile(const kxf::String& filePath);
			void SaveToFile(const kxf::String& filePath) const;
			void DoShowPreview(bool show);

		public:
			TextEditDialog(wxWindow* parent);
			virtual ~TextEditDialog();

		public:
			virtual int ShowModal() override;

		public:
			const kxf::String& GetText() const;
			void SetText(const kxf::String& text);
			bool IsModified() const
			{
				return m_IsTextModified;
			}
			void ShowPreview(bool show)
			{
				m_EditMode = !show;
			}
			bool IsEditable() const
			{
				return m_Editable;
			}
			void SetEditable(bool value)
			{
				m_Editable = value;
			}
	};
}
