#include "stdafx.h"
#include "Workspace.h"
#include "DisplayModel.h"
#include <Kortex/Application.hpp>
#include <Kortex/Notification.hpp>
#include <Kortex/DownloadManager.hpp>
#include "GameMods/ModManager/Workspace.h"
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/KxFileBrowseDialog.h>
#include <kxf::UI::Framework/KxComboBox.h>

namespace
{
	auto GetDisplayModelOptions()
	{
		using namespace Kortex;
		using namespace Kortex::Application;

		return GetAInstanceOptionOf<IDownloadManager>(OName::Workspace, OName::DisplayModel);
	}
}

namespace Kortex::DownloadManager
{
	void Workspace::OnSelectConcurrentDownloadsCount(wxCommandEvent& event)
	{
		IDownloadManager::GetInstance()->SetMaxConcurrentDownloads(event.GetSelection());
		BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtConcurrentDownloadsCountChanged);
	}

	bool Workspace::OnCreateWorkspace()
	{
		IDownloadManager* manager = IDownloadManager::GetInstance();
		manager->LoadDownloads();

		// Main view
		m_MainSizer = new wxBoxSizer(wxVERTICAL);
		SetSizer(m_MainSizer);

		m_DisplayModel = new DisplayModel();
		m_DisplayModel->CreateView(this);

		m_MainSizer->Add(m_DisplayModel->GetView(), 1, wxEXPAND);
		GetDisplayModelOptions().LoadDataViewLayout(m_DisplayModel->GetView());

		// Toolbar
		m_ToolBar = new kxf::UI::AuiToolBar(this, wxID_NONE, kxf::UI::AuiToolBar::DefaultStyle|wxAUI_TB_PLAIN_BACKGROUND);
		m_ToolBar->SetBackgroundColour(IThemeManager::GetActive().GetColor(Theme::ColorIndex::Window, Theme::ColorFlag::Background));
		m_ToolBar->AddStretchSpacer();

		{
			m_ToolBar->AddLabel(KTr("DownloadManager.ConcurrentDownloads.Label") + ':');

			KxComboBox* comboBox = new KxComboBox(m_ToolBar, wxID_NONE);
			comboBox->Bind(wxEVT_COMBOBOX, &Workspace::OnSelectConcurrentDownloadsCount, this);

			comboBox->AddItem(KTr("DownloadManager.ConcurrentDownloads.Unlimited"));
			for (int i = 1; i <= 10; i++)
			{
				comboBox->AddItem(kxf::String::Format(wxS("%1"), i));
			}

			comboBox->SetSelection(0);
			if (IDownloadManager::GetInstance()->HasConcurrentDownloadsLimit())
			{
				comboBox->SetSelection(IDownloadManager::GetInstance()->GetMaxConcurrentDownloads());
			}
			m_ToolBar->AddControl(comboBox);
		}

		m_ToolBar->UpdateUI();
		m_ToolBar->SetMargins(0, 0, 0, 0);
		m_MainSizer->Add(m_ToolBar, 0, wxEXPAND|wxTOP, LayoutConstants::VerticalSpacing);

		return true;
	}
	bool Workspace::OnOpenWorkspace()
	{
		return true;
	}
	bool Workspace::OnCloseWorkspace()
	{
		return true;
	}
	void Workspace::OnReloadWorkspace()
	{
		m_DisplayModel->RefreshItems();
	}

	Workspace::~Workspace()
	{
		if (IsCreated())
		{
			GetDisplayModelOptions().SaveDataViewLayout(m_DisplayModel->GetView());
		}
	}

	kxf::String Workspace::GetID() const
	{
		return "DownloadManager::Workspace";
	}
	kxf::String Workspace::GetName() const
	{
		return KTr("DownloadManager.NameShort");
	}
	IWorkspaceContainer* Workspace::GetPreferredContainer() const
	{
		IWorkspaceContainer* result = nullptr;
		IWorkspace::CallIfCreated<ModManager::Workspace>([&](ModManager::Workspace& workspace)
		{
			result = &workspace.GetWorkspaceContainer();
		});
		return result;
	}
}
