#include "stdafx.h"
#include "Window.h"
#include <Kortex/Application.hpp>
#include <Kortex/ModManager.hpp>
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/KxButton.h>

namespace Kortex::Application::Settings
{
	void Window::OnCloseWindow(wxCloseEvent& event)
	{
		if (m_Manager.HasUnsavedChanges())
		{
			KxTaskDialog dialog(this, wxID_NONE, KTrf("Settings.SaveMessage"), {}, KxBTN_OK, kxf::StdIcon::Information);
			dialog.ShowModal();

			m_Manager.SaveChanges();
		}
		event.Skip();
	}
	void Window::OnPrepareUninstall(wxCommandEvent& event)
	{
		KxTaskDialog askDialog(this, wxID_NONE, KTrf("Settings.PrepareUninstall.Caption", IApplication::GetInstance()->GetName()), KTr("Settings.PrepareUninstall.Message"), KxBTN_YES|KxBTN_NO, KxICON_WARNING);
		if (askDialog.ShowModal() == wxID_YES)
		{
			if (IApplication::GetInstance()->Uninstall())
			{
				KxTaskDialog dialog(this, wxID_NONE, KTr("Settings.PrepareUninstall.Success"), wxEmptyString, KxBTN_NONE, kxf::StdIcon::Information);
				dialog.AddButton(wxID_OK, KTr("Settings.PrepareUninstall.RebootNow"));
				dialog.AddButton(wxID_CANCEL, KTr("Settings.PrepareUninstall.RebootLater"));
				if (dialog.ShowModal() == wxID_OK)
				{
					if (IMainWindow* mainWindow = IMainWindow::GetInstance())
					{
						mainWindow->GetFrame().Close(true);
					}
					wxShutdown(wxSHUTDOWN_REBOOT);
				}
			}
			else
			{
				KxTaskDialog(this, wxID_NONE, KTr("Settings.PrepareUninstall.Error"), wxEmptyString, KxBTN_OK, KxICON_ERROR).ShowModal();
			}
		}
	}

	Window::Window(wxWindow* parent)
		:m_DisplayModel(m_Manager)
	{
		m_Manager.OnInit();
		m_Manager.Load();

		parent = parent ? parent : &IMainWindow::GetInstance()->GetFrame();
		if (Create(parent, wxID_NONE, KTr("Settings.Caption"), wxDefaultPosition, FromDIP(kxf::Size(800, 600)), KxBTN_OK|KxBTN_CANCEL))
		{
			Bind(wxEVT_CLOSE_WINDOW, &Window::OnCloseWindow, this);

			wxWindow* removeButton = AddButton(wxID_REMOVE, KTr("Settings.PrepareUninstall.Button"), true).GetControl();
			removeButton->Bind(wxEVT_BUTTON, &Window::OnPrepareUninstall, this);

			SetMainIcon(kxf::StdIcon::None);
			PostCreate();
			IThemeManager::GetActive().Apply(GetContentWindow());

			// Create display
			m_DisplayModel.CreateView(GetContentWindow(), GetContentWindowMainSizer());
			
			KxDataView2::View* view = m_DisplayModel.GetView();
			view->GetColumnByID(GameConfig::ColumnID::Type)->SetVisible(false);
			view->GetColumnByID(GameConfig::ColumnID::Path)->SetVisible(false);
			view->SetExpanderColumn(view->GetColumnByID(GameConfig::ColumnID::Name));

			m_DisplayModel.ExpandBranches();
			m_DisplayModel.DisableColumnsMenu();
			m_DisplayModel.LoadView();
		}
	}
	Window::~Window()
	{
		m_Manager.OnExit();
	}
}
