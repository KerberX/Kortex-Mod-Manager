#include "stdafx.h"
#include <Kortex/Application.hpp>
#include <Kortex/ScreenshotsGallery.hpp>
#include "Workspace.h"
#include "GameMods/ModManager/Workspace.h"
#include "GameInstance/IGameInstance.h"
#include "UI/ImageViewerDialog.h"
#include <KxFramework/KxLabel.h>
#include <KxFramework/KxTextBox.h>
#include <KxFramework/KxThumbView.h>
#include <KxFramework/KxShellMenu.h>
#include <KxFramework/KxFile.h>

namespace Kortex::ScreenshotsGallery
{
	bool Workspace::OnCreateWorkspace()
	{
		m_MainSizer = new wxBoxSizer(wxVERTICAL);
		SetSizer(m_MainSizer);

		m_ViewPane = new KxThumbView(this, KxID_NONE);
		m_ViewPane->SetSpacing(wxSize(LayoutConstants::VerticalSpacing, LayoutConstants::HorizontalSpacing));
		m_MainSizer->Add(m_ViewPane, 1, wxEXPAND);
		IThemeManager::GetActive().Apply(m_ViewPane);

		m_ViewPane->Bind(KxEVT_THUMBVIEW_SELECTED, &Workspace::OnSelectItem, this);
		m_ViewPane->Bind(KxEVT_THUMBVIEW_ACTIVATED, &Workspace::OnActivateItem, this);
		m_ViewPane->Bind(KxEVT_THUMBVIEW_CONTEXT_MENU, &Workspace::OnItemMenu, this);

		OnReloadWorkspace();
		return true;
	}
	bool Workspace::OnOpenWorkspace()
	{
		m_ViewPane->SetFocus();
		return true;
	}
	bool Workspace::OnCloseWorkspace()
	{
		return true;
	}
	void Workspace::OnReloadWorkspace()
	{
		LoadData();
	}

	Workspace::Workspace()
	{
		IMainWindow::GetInstance()->AddToolBarMenuItem(*this);
	}
	Workspace::~Workspace()
	{
	}

	void Workspace::LoadData()
	{
		m_ViewPane->ClearThumbs();
		m_LoadedImages.clear();

		IScreenshotsGallery* manager = IScreenshotsGallery::GetInstance();
		for (const kxf::String& folderPath: manager->GetConfig().GetLocations())
		{
			KxStringVector files = KxFile(KVarExp(folderPath)).Find(manager->GetSupportedExtensions(), KxFS_FILE, false);
			for (const kxf::String& path: files)
			{
				m_LoadedImages.emplace_back(path);
				m_ViewPane->AddThumb(path);
			}
		}
	}
	void Workspace::OnSelectItem(wxCommandEvent& event)
	{
		if (event.GetInt() != wxNOT_FOUND)
		{
			DisplayInfo(m_LoadedImages[event.GetInt()]);
		}
		else
		{
			ClearControls();
		}
	}
	void Workspace::OnActivateItem(wxCommandEvent& event)
	{
		if (event.GetInt() != wxNOT_FOUND)
		{
			m_CurrentImageIndex = event.GetInt();
			UI::ImageViewerDialog dialog(this);
			dialog.Bind(UI::ImageViewerEvent::EvtPrevious, &Workspace::OnDialogNavigate, this);
			dialog.Bind(UI::ImageViewerEvent::EvtNext, &Workspace::OnDialogNavigate, this);

			UI::ImageViewerEvent evt;
			SetNavigationInfo(evt);
			evt.SetFilePath(m_LoadedImages[m_CurrentImageIndex]);
			dialog.Navigate(evt);

			dialog.ShowModal();
			m_CurrentImageIndex = -1;
		}
	}
	void Workspace::OnItemMenu(wxContextMenuEvent& event)
	{
		if (event.GetInt() != wxNOT_FOUND)
		{
			const kxf::String path = m_LoadedImages[event.GetInt()];
			KxShellMenu menu(path);
			if (menu.IsOK())
			{
				menu.Bind(KxEVT_MENU_HOVER, [](KxMenuEvent& event)
				{
					IMainWindow::GetInstance()->SetStatus(event.GetHelpString());
				});
				menu.Show(this, event.GetPosition());
			}
		}
	}
	void Workspace::OnDialogNavigate(UI::ImageViewerEvent& event)
	{
		int oldIndex = m_CurrentImageIndex;
		if (event.GetEventType() == UI::ImageViewerEvent::EvtNext)
		{
			m_CurrentImageIndex++;
		}
		else
		{
			m_CurrentImageIndex--;
		}

		SetNavigationInfo(event);
		if (m_CurrentImageIndex >= 0 && (size_t)m_CurrentImageIndex < m_LoadedImages.size())
		{
			event.SetFilePath(m_LoadedImages[m_CurrentImageIndex]);
		}
		else
		{
			m_CurrentImageIndex = oldIndex;
			event.Veto();
		}
	}
	void Workspace::SetNavigationInfo(UI::ImageViewerEvent& event)
	{
		event.SetHasPrevNext(m_CurrentImageIndex > 0, (size_t)(m_CurrentImageIndex + 1) < m_LoadedImages.size());
	}

	kxf::String Workspace::GetID() const
	{
		return "KScreenshotsGalleryWorkspace";
	}
	kxf::String Workspace::GetName() const
	{
		return KTr("ScreenshotsGallery.NameShort");
	}
	IWorkspaceContainer* Workspace::GetPreferredContainer() const
	{
		return &IMainWindow::GetInstance()->GetWorkspaceContainer();
	}

	void Workspace::DisplayInfo(const kxf::String& filePath)
	{
		ClearControls();
	}
	void Workspace::ClearControls()
	{
	}
}
