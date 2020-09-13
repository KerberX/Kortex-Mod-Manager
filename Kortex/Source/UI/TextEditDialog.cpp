#include "stdafx.h"
#include "TextEditDialog.h"
#include <Kortex/Application.hpp>
#include "Application/Resources/Imagekxf::ResourceID.h"
#include "Utility/UI.h"
#include <KxFramework/KxPanel.h>
#include <KxFramework/KxAuiToolBar.h>
#include <KxFramework/KxStyledTextBox.h>
#include <KxFramework/KxFileBrowseDialog.h>
#include <KxFramework/KxBitmapComboBox.h>
#include <KxFramework/KxComboBox.h>
#include <KxFramework/KxFileStream.h>
#include <KxFramework/KxTextFile.h>
#include <KxFramework/KxString.h>
#include <KxFramework/KxUtility.h>
#include <memory>

namespace Kortex::UI
{
	bool TextEditDialog::Create(wxWindow* parent)
	{
		if (KxStdDialog::Create(parent, KxID_NONE, KTr("TextEditor.Caption"), wxDefaultPosition, wxDefaultSize, KxBTN_OK|KxBTN_CANCEL))
		{
			SetMainIcon(KxICON_NONE);
			SetWindowResizeSide(wxBOTH);
			SetInitialSize(parent->GetSize().Scale(0.85f, 0.85f));

			/* View */
			wxBoxSizer* viewSizer = new wxBoxSizer(wxVERTICAL);
			m_View = new KxPanel(m_ContentPanel, KxID_NONE);
			m_View->SetSizer(viewSizer);
			IThemeManager::GetActive().Apply(m_View);

			/* ToolBar */
			m_ToolBar = new KxAuiToolBar(m_View, KxID_NONE, KxAuiToolBar::DefaultStyle|wxAUI_TB_PLAIN_BACKGROUND);
			m_ToolBar->SetBackgroundColour(m_View->GetBackgroundColour());
			viewSizer->Add(m_ToolBar, 0, wxEXPAND);

			// Edit mode
			m_ToolBar_SwitchMode = Utility::UI::CreateToolBarButton(m_ToolBar, KTr("TextEditor.ToolBar.Mode"), Imagekxf::ResourceID::Edit);
			m_ToolBar_SwitchMode->Bind(KxEVT_AUI_TOOLBAR_CLICK, &TextEditDialog::OnSwitchMode, this);
			m_ToolBar->AddSeparator();

			// Save/Load
			m_ToolBar_Save = Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_SAVE), Imagekxf::ResourceID::Disk);
			m_ToolBar_Save->Bind(KxEVT_AUI_TOOLBAR_CLICK, &TextEditDialog::OnSaveLoadFile, this);

			m_ToolBar_Open = Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_OPEN), Imagekxf::ResourceID::FolderOpen);
			m_ToolBar_Open->Bind(KxEVT_AUI_TOOLBAR_CLICK, &TextEditDialog::OnSaveLoadFile, this);
			m_ToolBar->AddSeparator();

			// Undo/Redo
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_UNDO), Imagekxf::ResourceID::ArrowCurve180Left)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				m_Editor->Undo();
			});
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_REDO), Imagekxf::ResourceID::ArrowCircle135Left)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				m_Editor->Redo();
			});
			m_ToolBar->AddSeparator();

			// Styles
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_BOLD), Imagekxf::ResourceID::EditBold)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				ToggleTag("b");
			});
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_ITALIC), Imagekxf::ResourceID::EditItalic)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				ToggleTag("i");
			});
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_UNDERLINE), Imagekxf::ResourceID::EditUnderline)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				ToggleTag("u");
			});
			m_ToolBar->AddSeparator();

			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_JUSTIFY_LEFT), Imagekxf::ResourceID::EditAlignmentLeft)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				ToggleTag("div", "align", "left");
			});
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_JUSTIFY_CENTER), Imagekxf::ResourceID::EditAlignmentCenter)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				ToggleTag("div", "align", "center");
			});
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_JUSTIFY_RIGHT), Imagekxf::ResourceID::EditAlignmentRight)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				ToggleTag("div", "align", "right");
			});
			Utility::UI::CreateToolBarButton(m_ToolBar, KTr(KxID_JUSTIFY_FILL), Imagekxf::ResourceID::EditAlignmentJustify)->Bind(KxEVT_AUI_TOOLBAR_CLICK, [this](KxAuiToolBarEvent& event)
			{
				ToggleTag("div", "align", "justify");
			});

			// Heading
			m_HeadingList = new KxBitmapComboBox(m_ToolBar, KxID_ANY);
			m_HeadingList->SetImageList(const_cast<kxf::ImageList*>(&ImageProvider::GetImageList()));
			m_ToolBar->AddControl(m_HeadingList, KTr("TextEditor.ToolBar.Heading"));

			const int maxHeading = 6;
			for (int i = 1; i <= maxHeading; i++)
			{
				m_HeadingList->AddItem(kxf::String::Format("%1 %2", KTr("TextEditor.ToolBar.Heading"), i), (int)Imagekxf::ResourceID::EditHeading + i);
			}
			m_HeadingList->SetSelection(0);
			m_HeadingList->Bind(wxEVT_COMBOBOX, [this](wxCommandEvent& event)
			{
				ToggleTag(kxf::String::Format("h%1", event.GetInt() + 1));
				m_Editor->SetFocus();
			});

			m_ToolBar->AddStretchSpacer();

			// Background color
			wxColourPickerCtrl* backgroundColorPicker = new wxColourPickerCtrl(m_ToolBar, KxID_NONE);
			backgroundColorPicker->Bind(wxEVT_COLOURPICKER_CHANGED, [this](wxColourPickerEvent& event)
			{
				if (KxHTMLWindow* htmlWindow = GetHTMLBackend())
				{
					htmlWindow->SetHTMLBackgroundColour(event.GetColour());
					htmlWindow->SetBackgroundColour(event.GetColour());
				}
				else if (UI::IWebView* backend = m_Preview.GetBackend())
				{
					backend->GetWindow()->SetBackgroundColour(event.GetColour());
				}

				if (m_Editor)
				{
					// It's probably not a good idea to change background color of the editor
					//m_Editor->SetBackgroundColour(event.GetColour());
				}
			});
			m_ToolBar->AddControl(backgroundColorPicker);

			// Rendering mode
			{
				KxComboBox* rendererList = new KxComboBox(m_ToolBar, KxID_NONE);
				rendererList->Bind(wxEVT_COMBOBOX, [this, rendererList](wxCommandEvent& event)
				{
					wxGraphicsRenderer* renderer = static_cast<wxGraphicsRenderer*>(rendererList->GetClientData(event.GetSelection()));
					if (KxHTMLWindow* htmlWindow = GetHTMLBackend())
					{
						htmlWindow->SetRenderer(renderer);
					}
					if (m_Editor)
					{
						// Changing rendering backend is fine, especially that Scintilla doesn't have HiDPI text scaling bug unlike wxWidgets own Direct2D renderer.
						m_Editor->SetTechnology(renderer == wxGraphicsRenderer::GetDirect2DRenderer() ? wxSTC_TECHNOLOGY_DIRECTWRITE : wxSTC_TECHNOLOGY_DEFAULT);
						m_Editor->Refresh();
					}
				});
				m_ToolBar->AddControl(rendererList);

				auto AddRenderer = [&](wxGraphicsRenderer* renderer, const kxf::String& name, bool allowNull = false)
				{
					if (renderer || allowNull)
					{
						int index = rendererList->AddItem(name);
						rendererList->SetClientData(index, renderer);
					}
				};

				AddRenderer(nullptr, wxS("GDI"), true);
				AddRenderer(wxGraphicsRenderer::GetGDIPlusRenderer(), wxS("GDI+"));
				AddRenderer(wxGraphicsRenderer::GetDirect2DRenderer(), wxS("Direct2D"));
				AddRenderer(wxGraphicsRenderer::GetCairoRenderer(), wxS("Cairo"));
				rendererList->SetSelection(0);
			}

			m_ToolBar->Realize();

			// Tabs
			m_Container = new wxSimplebook(m_View, KxID_NONE);
			viewSizer->Add(m_Container, 1, wxEXPAND|wxTOP, LayoutConstants::VerticalSpacing);
			IThemeManager::GetActive().Apply(m_Container);

			PostCreate(wxDefaultPosition);

			/* Editor page */
			m_Editor = new KxStyledTextBox(m_Container, KxID_NONE);
			m_Container->AddPage(m_Editor, wxEmptyString, true);
			backgroundColorPicker->SetColour(m_Editor->GetBackgroundColour());

			/* Preview page */
			m_Preview.Create(m_Container, WebView::Backend::Sciter);
			m_Container->AddPage(m_Preview, wxEmptyString);

			/* Complete creation */
			AddUserWindow(m_ToolBar);
			AddUserWindow(m_Editor);
			AddUserWindow(m_Preview);
			AdjustWindow(wxDefaultPosition);
			m_Editor->SetFocus();
			return true;
		}
		return false;
	}
	KxHTMLWindow* TextEditDialog::GetHTMLBackend()
	{
		UI::IWebView* backend = m_Preview.GetBackend();
		return backend ? dynamic_cast<KxHTMLWindow*>(backend->GetWindow()) : nullptr;
	}

	void TextEditDialog::OnNewTextSet()
	{
		m_Editor->SetValue(m_Text);
		ClearUndoHistory();
	}
	void TextEditDialog::OnPrepareSaveText()
	{
		m_Text = m_Editor->GetValue();
	}
	void TextEditDialog::ClearUndoHistory()
	{
		m_Editor->SetModified(false);
		m_Editor->EmptyUndoBuffer();
	}
	void TextEditDialog::OnSwitchMode(KxAuiToolBarEvent& event)
	{
		DoShowPreview(m_EditMode);
	}
	void TextEditDialog::OnKey(wxKeyEvent& event)
	{
		if (event.ControlDown() && event.GetKeyCode() == WXK_TAB)
		{
			DoShowPreview(m_EditMode);
		}
		else
		{
			event.Skip();
		}
	}
	void TextEditDialog::OnOK(wxNotifyEvent& event)
	{
		if (event.GetId() == KxID_OK)
		{
			OnPrepareSaveText();
			m_IsTextModified = m_Editor->IsModified();
		}
		else
		{
			m_IsTextModified = false;
		}
		event.Skip();
	}
	void TextEditDialog::OnSaveLoadFile(KxAuiToolBarEvent& event)
	{
		bool save = event.GetEventObject() == m_ToolBar_Save;

		KxFileBrowseDialog dialog(this, KxID_NONE, save ? KxFBD_SAVE : KxFBD_OPEN);
		dialog.AddFilter("*.txt", KTr("FileFilter.Text"));
		dialog.AddFilter("*", KTr("FileFilter.AllFiles"));
		dialog.SetDefaultExtension("txt");

		if (dialog.ShowModal() == KxID_OK)
		{
			if (save)
			{
				OnPrepareSaveText();
				SaveToFile(dialog.GetResult());
			}
			else
			{
				LoadFromFile(dialog.GetResult());
			}
		}
	}

	void TextEditDialog::ToggleTag(const kxf::String& tagStart, const kxf::String& tagEnd)
	{
		int selStart = m_Editor->GetSelectionStart();
		kxf::String text = m_Editor->GetSelectedText();
		if (text.StartsWith('<' + tagStart) && (text.EndsWith('/' + tagEnd + '>') || text.EndsWith("/>")))
		{
			kxf::String innerText = text.AfterFirst('>').BeforeLast('<');
			m_Editor->ReplaceSelection(innerText);
		}
		else
		{
			kxf::String string = kxf::String::Format("<%s>%s</%s>", tagEnd, text, tagEnd);
			m_Editor->ReplaceSelection(string);
		}
	}
	void TextEditDialog::LoadFromFile(const kxf::String& filePath)
	{
		m_Text = KxTextFile::ReadToString(filePath);
		OnNewTextSet();
	}
	void TextEditDialog::SaveToFile(const kxf::String& filePath) const
	{
		KxTextFile::WriteToFile(filePath, m_Text);
	}
	void TextEditDialog::DoShowPreview(bool show)
	{
		if (show)
		{
			m_Preview.LoadText(m_Editor->GetValue());
			m_Container->ChangeSelection(1);
			m_ToolBar_SwitchMode->SetBitmap(ImageProvider::GetBitmap(Imagekxf::ResourceID::EditCodeDivision));
			m_Preview.GetWindow()->SetFocus();

			m_EditMode = false;
		}
		else
		{
			m_Container->ChangeSelection(0);
			m_ToolBar_SwitchMode->SetBitmap(ImageProvider::GetBitmap(Imagekxf::ResourceID::Edit));
			m_Editor->SetEditable(IsEditable());
			m_Editor->SetFocus();

			m_EditMode = true;
		}

		for (size_t i = 0; i < m_ToolBar->GetToolCount(); i++)
		{
			KxAuiToolBarItem* item = m_ToolBar->FindToolByIndex(i);
			if (item)
			{
				item->SetEnabled(m_EditMode);
			}
		}
		m_ToolBar_SwitchMode->SetEnabled(true);
		m_ToolBar_Save->SetEnabled(true);
		m_HeadingList->Enable(m_EditMode);
	}

	TextEditDialog::TextEditDialog(wxWindow* parent)
	{
		if (Create(parent))
		{
			SetSize(IMainWindow::GetDialogBestSize(this));
			CenterOnScreen();

			Bind(wxEVT_CHAR_HOOK, &TextEditDialog::OnKey, this);
			Bind(KxEVT_STDDIALOG_BUTTON, &TextEditDialog::OnOK, this);
		}
	}
	TextEditDialog::~TextEditDialog()
	{
	}

	int TextEditDialog::ShowModal()
	{
		DoShowPreview(!m_EditMode);
		return KxStdDialog::ShowModal();
	}

	const kxf::String& TextEditDialog::GetText() const
	{
		return m_Text;
	}
	void TextEditDialog::SetText(const kxf::String& text)
	{
		m_Text = text;
		OnNewTextSet();
	}
}
