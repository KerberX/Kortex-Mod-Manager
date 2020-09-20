#include "stdafx.h"
#include "MainWindow.h"
#include <Kortex/Application.hpp>
#include <Kortex/Notification.hpp>
#include <Kortex/ModManager.hpp>
#include <Kortex/NetworkManager.hpp>
#include <Kortex/ProgramManager.hpp>
#include <Kortex/DownloadManager.hpp>
#include <Kortex/GameInstance.hpp>
#include "GameMods/ModManager/Workspace.h"
#include "Application/About/Dialog.h"
#include "Application/Settings/Window.h"
#include "VirtualFileSystem/VirtualFSEvent.h"
#include "Utility/Log.h"
#include "Utility/UI.h"
#include <kxf/UI/Dialogs/TaskDialog.h>
#include <kxf/System/SystemProcess.h>
#include <kxf/System/ShellOperations.h>
#include <wx/wupdlock.h>

namespace Kortex::Application
{
	void MainWindowWorkspaceContainer::Create()
	{
		m_BookCtrl = new kxf::UI::Simplebook(&m_MainWindow, wxID_NONE);
		m_BookCtrl->SetBackgroundColour(m_MainWindow.GetBackgroundColour());
		m_BookCtrl->SetImageList(const_cast<kxf::ImageList*>(&ImageProvider::GetImageList()));
	}

	void MainWindowWorkspaceContainer::ShowWorkspace(IWorkspace& workspace)
	{
		// Update tool and status bars
		m_MainWindow.m_QuickToolBar_QuickSettingsMenu->SetEnabled(false);
		m_MainWindow.m_QuickToolBar_Help->SetEnabled(false);

		wxWindowUpdateLocker lock(m_MainWindow.m_StatusBar);
		for (int i = 0; i < m_MainWindow.m_StatusBar->GetFieldsCount() - 1; i++)
		{
			m_MainWindow.ClearStatus(i);
		}
		BookWorkspaceContainer::ShowWorkspace(workspace);
	}
	void MainWindowWorkspaceContainer::HideWorkspace(IWorkspace& workspace)
	{
		BookWorkspaceContainer::HideWorkspace(workspace);
	}
}

namespace Kortex::Application
{
	const void* MainWindow::GetUniqueID()
	{
		// Should not be zero
		return reinterpret_cast<const void*>(0xFF);
	}

	void MainWindow::CreateToolBar()
	{
		m_ToolBarSizer = new wxBoxSizer(wxHORIZONTAL);
		m_MainSizer->Add(m_ToolBarSizer, 0, wxEXPAND);

		auto NewToolBar = [this](int proportion = 1, bool hasText = false, int flags = 0)
		{
			flags |= kxf::UI::AuiToolBar::DefaultStyle|wxAUI_TB_PLAIN_BACKGROUND|wxAUI_TB_HORIZONTAL|wxAUI_TB_HORZ_LAYOUT;
			if (hasText)
			{
				flags |= wxAUI_TB_TEXT;
			}

			kxf::UI::AuiToolBar* toolBar = new kxf::UI::AuiToolBar(this, wxID_NONE, flags);
			toolBar->SetToolBorderPadding(FromDIP(LayoutConstants::HorizontalSpacingSmall));
			toolBar->SetMargins(FromDIP(LayoutConstants::HorizontalSpacing), FromDIP(LayoutConstants::HorizontalSpacing), FromDIP(LayoutConstants::VerticalSpacing), FromDIP(LayoutConstants::VerticalSpacing + 1));
			IThemeManager::GetActive().Apply(toolBar);

			m_ToolBarSizer->Add(toolBar, proportion, wxEXPAND);
			return toolBar;
		};

		// Main ToolBar
		m_ToolBar = NewToolBar(1, true);
		{
			m_ToolBar_MainMenu = m_ToolBar->AddTool(kxf::String::Format("%8s%s%8s", "", "Kortex", ""), wxNullBitmap, wxITEM_DROPDOWN);
			m_ToolBar_MainMenu->AddOption(kxf::UI::AuiToolBarItemOption::MenuOnLeftClick);
			m_ToolBar_MainMenu->Bind(kxf::UI::AuiToolBarEvent::EvtItemClick, [this](kxf::UI::AuiToolBarEvent& evnt)
			{
				kxf::UI::Menu menu;
				CreateMainMenu(menu);

				kxf::FlagSet<kxf::Alignment> alignment;
				kxf::Point pos = m_ToolBar_MainMenu->GetDropdownMenuPosition(&alignment);
				menu.Show(m_ToolBar, pos, alignment);
			});

			wxImage icon = ImageProvider::GetImage(wxS("kortex-logo"));
			kxf::Size size = Utility::BitmapSize().FromSystemSmallIcon();
			size.Scale(1.15, 1.15);
			m_ToolBar_MainMenu->SetBitmap(icon.Rescale(size.GetWidth(), size.GetHeight(), wxIMAGE_QUALITY_HIGH));

			m_ToolBar->AddSeparator();
		}
		m_ToolBar->Realize();

		// Quick ToolBar
		m_QuickToolBar = NewToolBar(0, true);
		{
			INetworkManager::GetInstance()->AddToolbarButton(*m_QuickToolBar, ImageResourceID::KortexLogoSmall);
			m_QuickToolBar->AddSeparator();
			INotificationCenter::GetInstance()->AddToolbarButton(*m_QuickToolBar, ImageResourceID::Bell);

			m_QuickToolBar_QuickSettingsMenu = m_QuickToolBar->AddTool(wxEmptyString, ImageProvider::GetBitmap(ImageResourceID::Gear), wxITEM_NORMAL);
			m_QuickToolBar_QuickSettingsMenu->Bind(kxf::UI::AuiToolBarEvent::EvtItemClick, &MainWindow::OnQSMButton, this);

			m_QuickToolBar_Help = m_QuickToolBar->AddTool(wxEmptyString, ImageProvider::GetBitmap(ImageResourceID::QuestionFrame), wxITEM_NORMAL);
		}
		m_QuickToolBar->Realize();
	}
	void MainWindow::CreateStatusBar()
	{
		const int iconWidth = Utility::BitmapSize().FromSystemSmallIcon().GetWidth();

		m_StatusBar = new kxf::UI::StatusBarEx(this, wxID_NONE, 5);
		m_StatusBar->SetImageList(&ImageProvider::GetImageList());
		m_StatusBar->SetStatusWidths({iconWidth, -3, -3, -1, FromDIP(50)});
		SetStatusBar(m_StatusBar);
	}
	void MainWindow::CreateBaseLayout()
	{
		m_WorkspaceContainer.Create();
		m_MainSizer->Add(&m_WorkspaceContainer.GetWindow(), 1, wxEXPAND);
	}
	WXLRESULT MainWindow::MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if (msg == WM_COPYDATA)
		{
			const COPYDATASTRUCT* data = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
			const kxf::String uri(reinterpret_cast<const wchar_t*>(data->lpData), data->dwData);

			IDownloadManager::GetInstance()->QueueUnknownDownload(uri);
		}
		return kxf::UI::Frame::MSWWindowProc(msg, wParam, lParam);
	}

	void MainWindow::CreateWorkspaces()
	{
		m_ToolBar_InsertionIndex = m_ToolBar->AddSeparator()->GetIndex();

		// Add workspaces menu
		kxf::UI::AuiToolBarItem* toolBarButton = Utility::UI::CreateToolBarButton(m_ToolBar, ITranslator::GetVariable(Variables::KVAR_GAME_NAME));
		toolBarButton->SetBitmap(Utility::BitmapSize(m_ToolBar->GetToolBitmapSize()).ScaleMaintainRatio(IGameInstance::GetActive()->GetIcon()));
		toolBarButton->AddOption(kxf::UI::AuiToolBarItemOption::MenuOnLeftClick);
		toolBarButton->SetDropdownMenu(&m_WorkspacesMenu);

		// Create workspaces
		InitializeWorkspaces();

		m_ToolBar->UpdateUI();
		m_ToolBar->SetOverflowVisible(!m_ToolBar->IsItemsFits());
	}
	void MainWindow::CreateMainMenu(kxf::UI::Menu& mainMenu)
	{
		{
			kxf::UI::MenuItem* item = mainMenu.Add(new kxf::UI::MenuItem(KTr("MainMenu.Settings")));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::ApplicationTask));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				Settings::Window(this).ShowModal();
			});
		}
		mainMenu.AddSeparator();
		{
			kxf::UI::MenuItem* item = mainMenu.Add(new kxf::UI::MenuItem(KTr("MainMenu.ChangeInstance")));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, &MainWindow::OnChangeInstance, this);
			item->Enable(!IModManager::GetInstance()->GetFileSystem().IsEnabled());
		}
		mainMenu.AddSeparator();

		// TODO: rework menu items injection
		// Add programs
		size_t count = mainMenu.GetMenuItemCount();
		//IProgramManager::GetInstance()->OnAddMainMenuItems(mainMenu);
		if (count != mainMenu.GetMenuItemCount())
		{
			mainMenu.AddSeparator();
		}

		// Add locations
		AddLocationsMenu(mainMenu);

		// Add about
		{
			kxf::UI::MenuItem* item = mainMenu.Add(new kxf::UI::MenuItem(KTr("MainMenu.About")));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::InformationFrame));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				AboutDialog(this).ShowModal();
			});
		}
	}
	void MainWindow::AddLocationsMenu(kxf::UI::Menu& mainMenu)
	{
		// TODO: Rewrite that shit and move it out of here
		// Set predefined locations
		if (m_Locations.empty())
		{
			using Variables::WrapAsInline;

			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_APP_SETTINGS_DIR), KTr("OpenLocation.AppSettings"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_ACTUAL_GAME_DIR), KTr("OpenLocation.GameRoot"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_VIRTUAL_GAME_DIR), KTr("OpenLocation.VirtualGameRoot"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_ACTUAL_CONFIG_DIR), KTr("OpenLocation.ConfigRootTarget"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_CONFIG_DIR), KTr("OpenLocation.VirtualConfigRoot"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_OVERWRITES_DIR), KTr("OpenLocation.WriteTargetRoot"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_INSTANCE_DIR), KTr("OpenLocation.CurrentProfileRoot"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_INSTANCES_DIR), KTr("OpenLocation.ProfilesRoot"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_MODS_DIR), KTr("OpenLocation.ModsRoot"));
			m_Locations.emplace_back(WrapAsInline(Variables::KVAR_SAVES_DIR), KTr("OpenLocation.Saves"));

			// TODO: make main window a manager to allow it load instance config
			#if 0
			if (node.HasChildren())
			{
				// This will allow to insert a separator in locations menu
				m_Locations.emplace_back(KLabeledValue(wxEmptyString, wxEmptyString));

				// Load profile locations
				for (kxf::XMLNode entryNode = node.GetFirstChildElement("Entry"); entryNode.IsOK(); entryNode = entryNode.GetNextSiblingElement())
				{
					KLabeledValue& value = m_Locations.emplace_back(KLabeledValue(KVarExp(entryNode.GetValue()), KVarExp(entryNode.GetAttribute("Label"))));
					if (value.GetValue().IsEmpty())
					{
						m_Locations.pop_back();
					}
				}
		}
			#endif
	}

		kxf::UI::Menu* locationsMenu = new kxf::UI::Menu();
		for (const Utility::LabeledValue& entry: m_Locations)
		{
			if (!entry.HasLabel() && !entry.HasValue())
			{
				locationsMenu->AddSeparator();
			}
			else
			{
				kxf::UI::MenuItem* item = locationsMenu->Add(new kxf::UI::MenuItem(KVarExp(entry.GetLabel())));
				item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Folder));
				item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, &entry](kxf::UI::MenuEvent& event)
				{
					// Create the folder, shouldn't be harmful.
					KxFile folder(KVarExp(entry.GetValue()));
					folder.CreateFolder();

					return kxf::Shell::Execute(this, folder.GetFullPath(), wxS("open"));
				});
			}
		}
		mainMenu.Add(locationsMenu, KTr("MainMenu.OpenLocation"))->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::FolderOpen));
		mainMenu.AddSeparator();
}

	void MainWindow::OnQSMButton(kxf::UI::AuiToolBarEvent& event)
	{
		// TODO: Decide what to do with 'Quick Settings Menu' button. It's currently unused.
	}
	void MainWindow::OnWindowClose(wxCloseEvent& event)
	{
		bool skip = false;

		if (event.CanVeto())
		{
			if (IModManager::GetInstance()->GetFileSystem().IsEnabled())
			{
				KxTaskDialog dialog(this, wxID_NONE, KTr("VFS.AskUnmountOnExit"), wxEmptyString, KxBTN_YES|KxBTN_NO, KxICON_QUESTION);
				if (dialog.ShowModal() != wxID_YES)
				{
					event.Veto();
					return;
				}
			}

			bool veto = false;
			auto AskForSave = [&event, &veto](IWorkspace& workspace)
			{
				// TODO: reimplement 'WorkspaceController' functions somewhere
				/*
				if (workspace.IsCreated())
				{
					IWorkspaceDocument* controller = workspace->GetWorkspaceController();
					if (controller && controller->AskForSave(event.CanVeto()) != wxID_OK)
					{
						veto = true;
						return false;
					}
				}
				*/
				return true;
			};

			IWorkspace* current = m_WorkspaceContainer.GetCurrentWorkspace();
			if (AskForSave(*current))
			{
				for (IWorkspace* workspace: m_WorkspaceContainer.EnumWorkspaces())
				{
					if (workspace != current && !AskForSave(*workspace))
					{
						break;
					}
				}
			}

			if (veto)
			{
				event.Veto();
			}
			else
			{
			skip = true;
			}
		}
		else
		{
		skip = true;
		}

		if (skip)
		{
			event.Skip();
			GetAInstanceOption().SaveWindowGeometry(this);
		}
	}
	void MainWindow::OnChangeInstance(kxf::UI::MenuEvent& event)
	{
		// TODO: reimplement 'WorkspaceController' functions somewhere [2]
		/*
		IWorkspaceDocument* controller = GetCurrentWorkspace()->GetWorkspaceController();
		if (controller && controller->AskForSave() == wxID_CANCEL)
		{
			return;
		}
		*/

		if (IApplication::GetInstance()->OpenInstanceSelectionDialog())
		{
			Close(true);
		}
	}

	void MainWindow::OnMainFSToggled(bool isActive)
	{
		if (isActive)
		{
			m_StatusBar->SetStatusText(KTr("VFS.Status.Active"));
			m_StatusBar->SetStatusImage((int)ImageResourceID::TickCircleFrame, 0);
		}
		else
		{
			m_StatusBar->SetStatusText(KTr("VFS.Status.Inactive"));
			m_StatusBar->SetStatusImage((int)ImageResourceID::InformationFrameEmpty, 0);
		}
		IThemeManager::GetActive().Apply(m_StatusBar, isActive);
	}
	void MainWindow::OnMainFSToggled(VirtualFSEvent& event)
	{
		OnMainFSToggled(event.IsActivated());
	}

	bool MainWindow::Create(wxWindow* parent)
	{
		const kxf::Size minSize(850, 600);
		if (kxf::UI::Frame::Create(parent, wxID_NONE, IApplication::GetInstance()->GetName(), wxDefaultPosition, minSize, MainWindow::DefaultStyle))
		{
			IThemeManager::GetActive().Apply(static_cast<IMainWindow*>(this));
			SetWindowUserData(GetUniqueID());
			SetDefaultBackgroundColor();
			SetIcons(wxICON(IDS_ICON_APP));
			SetMinSize(minSize);

			m_MainSizer = new wxBoxSizer(wxVERTICAL);
			SetSizer(m_MainSizer);

			CreateToolBar();
			CreateStatusBar();
			CreateBaseLayout();
			CreateWorkspaces();

			Bind(wxEVT_CLOSE_WINDOW, &MainWindow::OnWindowClose, this);
			m_BroadcastReciever.Bind(VirtualFSEvent::EvtMainToggled, &MainWindow::OnMainFSToggled, this);

			GetAInstanceOption().LoadWindowGeometry(this);
			OnMainFSToggled(false);
			return true;
		}
		return false;
	}

	void MainWindow::ClearStatus(int index)
	{
		if (m_StatusBar)
		{
			m_StatusBar->SetStatusText(wxEmptyString, index + 1);
			m_StatusBar->SetStatusImage(-1, index + 1);
		}
	}
	void MainWindow::SetStatus(const kxf::String& label, int index, const kxf::ResourceID& image)
	{
		if (m_StatusBar)
		{
			m_StatusBar->SetStatusText(label, index + 1);
			m_StatusBar->SetStatusImage(image.GetInt(), index + 1);
		}
	}
	void MainWindow::SetStatusProgress(int current)
	{
		if (m_StatusBar)
		{
			m_StatusBar->SetValue(current);
		}
	}
	void MainWindow::SetStatusProgress(int64_t current, int64_t total)
	{
		if (m_StatusBar)
		{
			m_StatusBar->SetValue(current, total);
		}
	}

	kxf::UI::AuiToolBarItem* MainWindow::AddToolBarItem(IWorkspace& workspace)
	{
		kxf::UI::AuiToolBarItem* item = m_ToolBar->AddTool(workspace.GetName(), ImageProvider::GetBitmap(workspace.GetIcon()), wxITEM_NORMAL, workspace.GetName());
		item->SetIndex(m_ToolBar_InsertionIndex);
		item->Bind(kxf::UI::AuiToolBarEvent::EvtItemClick, [&workspace](wxEvent& event)
		{
			workspace.SwitchHere();
		});

		return item;
	}
	kxf::UI::MenuItem* MainWindow::AddToolBarMenuItem(IWorkspace& workspace)
	{
		kxf::UI::MenuItem* item = m_WorkspacesMenu.AddItem(workspace.GetName());
		item->SetBitmap(ImageProvider::GetBitmap(workspace.GetIcon()));
		item->Bind(kxf::UI::MenuEvent::EvtSelect, [&workspace](wxEvent& event)
		{
			workspace.SwitchHere();
		});

		return item;
	}
}
