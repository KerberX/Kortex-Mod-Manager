#include "stdafx.h"
#include "DisplayModel.h"
#include <Kortex/Application.hpp>
#include <Kortex/Notification.hpp>
#include <Kortex/GameInstance.hpp>
#include <Kortex/InstallWizard.hpp>
#include <Kortex/NetworkManager.hpp>
#include "Network/ModNetwork/Nexus.h"
#include "Utility/MenuSeparator.h"
#include "Utility/Common.h"
#include <kxf::UI::Framework/kxf::UI::Menu.h>
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxShell.h>
#include <kxf::UI::Framework/KxTaskDialog.h>
#include <kxf::UI::Framework/KxTextBoxDialog.h>
#include <kxf::UI::Framework/KxComparator.h>

namespace Kortex::DownloadManager
{
	KxDataView2::ToolTip DisplayModel::GetToolTip(const KxDataView2::Node& node, const KxDataView2::Column& column) const
	{
		using ColumnID = DisplayModelNode::ColumnID;
		if (column.GetID<ColumnID>() == ColumnID::Name)
		{
			const DisplayModelNode& displayNode = static_cast<const DisplayModelNode&>(node);

			kxf::String message;
			for (size_t i = 0; i < GetView()->GetColumnCount(); i++)
			{
				KxDataView2::Column* column = GetView()->GetColumn(i);
				if (column)
				{
					kxf::String text = column->GetRenderer().GetTextValue(displayNode.GetValue(*column));
					if (!text.IsEmpty())
					{
						if (!message.IsEmpty())
						{
							message += wxS("\r\n");
						}
						message += kxf::String::Format(wxS("%1:\t%2"), column->GetTitle(), text);
					}
				}
			}
			return message;
		}
		return KxDataView2::Model::GetToolTip(node, column);
	}

	void DisplayModel::OnActivate(KxDataView2::Event& event)
	{
		DownloadItem* download = GetItem(event.GetNode());
		if (download && download->IsCompleted())
		{
			KxShell::Execute(GetView(), download->GetLocalPath());
		}
	}
	void DisplayModel::OnContextMenu(KxDataView2::Event& event)
	{
		DownloadItem* download = GetItem(event.GetNode());
		const IModNetwork* modNetwork = download ? download->GetModNetwork() : nullptr;

		const bool isRunning = download && download->IsRunning();
		const bool isPaused = download && download->IsPaused();
		const bool isFailed = download && download->IsFailed();
		const bool isCompleted = download && download->IsCompleted();
		const bool isListEmpty = m_Nodes.empty();

		kxf::UI::Menu contextMenu;
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.Install"));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Box));
			item->Enable(isCompleted);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, download](kxf::UI::MenuEvent& event)
			{
				Install(*download);
			});
		}
		contextMenu.AddSeparator();

		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.AddFromURL"));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::PlusSmall));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				KxTextBoxDialog dialog(GetView(), wxID_NONE, KTr("DownloadManager.Menu.AddFromURL.Message"), wxDefaultPosition, wxDefaultSize, KxBTN_OK|KxBTN_CANCEL);
				if (dialog.ShowModal() == wxID_OK)
				{
					m_DownloadManager.QueueUnknownDownload(dialog.GetValue());
				}
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.Pause"));
			item->Enable(isRunning);
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::ControlPause));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [download](kxf::UI::MenuEvent& event)
			{
				download->Pause();
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.Abort"));
			item->Enable(isRunning || isPaused);
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::ControlStopSquare));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [download](kxf::UI::MenuEvent& event)
			{
				download->Stop();
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(isPaused ? KTr("DownloadManager.Menu.Resume") : KTr("DownloadManager.Menu.Start"));
			item->Enable(download && (download->CanStart() || download->CanResume()));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::ControlRight));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [download, isPaused](kxf::UI::MenuEvent& event)
			{
				if (isPaused)
				{
					download->Resume();
				}
				else
				{
					download->Start();
				}
			});
		}

		contextMenu.AddSeparator();
		{
			kxf::UI::Menu* providerMenu = new kxf::UI::Menu();
			if (download && !isRunning)
			{
				for (ModNetworkRepository* repository: INetworkManager::GetInstance()->GetModRepositories())
				{
					const ModNetworkRepository* modRepository = download->GetModRepository();

					kxf::UI::MenuItem* item = providerMenu->AddItem(repository->GetContainer().GetName(), wxEmptyString, wxITEM_CHECK);
					item->Check(repository == modRepository);
					item->SetBitmap(ImageProvider::GetBitmap(repository->GetContainer().GetIcon()));
					item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, download, repository](kxf::UI::MenuEvent& event)
					{
						NetworkModInfo networkModInfo = download->GetNetworkModInfo();

						KxTextBoxDialog dialog(GetView(), wxID_NONE, KTr("DownloadManager.Menu.SetSource.Message"), wxDefaultPosition, wxDefaultSize, KxBTN_OK|KxBTN_CANCEL);
						dialog.SetIcon(ImageProvider::GetIcon(repository->GetContainer().GetIcon()));
						dialog.SetValue(networkModInfo.ToString());
						dialog.GetTextBox()->SetValidator(NetworkModInfo::CreateValidator());
						if (dialog.ShowModal() == wxID_OK)
						{
							networkModInfo.FromString(dialog.GetValue());
							download->SetModRepository(*repository, std::move(networkModInfo));
							download->Save();
						}
					});
				}
			}

			kxf::UI::MenuItem* item = contextMenu.Add(providerMenu, KTr("DownloadManager.Menu.SetSource"));
			item->Enable(providerMenu->GetMenuItemCount() != 0);
		}
		{
			kxf::UI::Menu* targetGameMenu = new kxf::UI::Menu();
			if (download && !isRunning)
			{
				for (const auto& gameTemplate: IGameInstance::GetTemplates())
				{
					const GameID gameID = download->GetTargetGame();

					kxf::UI::MenuItem* item = targetGameMenu->AddItem(gameTemplate->GetGameName(), wxEmptyString, wxITEM_CHECK);
					item->Check(gameID == gameTemplate->GetGameID());
					item->Enable(!gameID || !item->IsChecked());
					item->SetBitmap(gameTemplate->GetIcon(ImageProvider::GetImageList().GetSize()));
					item->Bind(kxf::UI::MenuEvent::EvtSelect, [download, &gameTemplate = *gameTemplate](kxf::UI::MenuEvent& event)
					{
						download->SetTargetGame(gameTemplate.GetGameID());
						download->Save();
					});
				}
			}

			kxf::UI::MenuItem* item = contextMenu.Add(targetGameMenu, KTr("DownloadManager.Menu.SetTargetGame"));
			item->Enable(targetGameMenu->GetMenuItemCount() != 0);
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.QueryInfo"));
			item->Enable(download && download->CanQueryInfo());
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [download](kxf::UI::MenuEvent& event)
			{
				if (download->QueryInfo())
				{
					download->Save();
				}
				else
				{
					kxf::String message = KTrf("DownloadManager.Notification.QueryDownloadInfoFailed", download->GetName());
					INotificationCenter::NotifyUsing<IDownloadManager>(message, KxICON_WARNING);
				}
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.ShowChangeLog"));
			item->Enable(download && download->HasChangeLog());
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::EditList));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, download](kxf::UI::MenuEvent& event)
			{
				KxTaskDialog dialog(GetView(), wxID_NONE, download->GetDisplayName(), wxEmptyString, KxBTN_OK, kxf::StdIcon::None);
				dialog.SetMessage(kxf::String::Format("%1 %2", KTr("Generic.Version"), download->GetVersion()));
				dialog.SetExMessage(download->GetChangeLog());
				dialog.SetMainIcon(KxShell::GetFileIcon(download->GetLocalPath()));
				dialog.SetOptionEnabled(KxTD_EXMESSAGE_EXPANDED);
				dialog.ShowModal();
			});
		}
		contextMenu.AddSeparator();

		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.Remove"));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::Bin));
			item->Enable(download && !isListEmpty && !isRunning);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, download](kxf::UI::MenuEvent& event)
			{
				if (m_DownloadManager.RemoveDownload(*download))
				{
					BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtRefreshItems);
				}
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.RemoveAll"));
			item->Enable(!isListEmpty);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				RemoveAll();
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.RemoveInstalled"));
			item->Enable(!isListEmpty);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				RemoveAll(true);
			});
		}
		contextMenu.AddSeparator();

		if (download)
		{
			const bool isVisible = download->IsVisible();

			kxf::UI::MenuItem* item = contextMenu.AddItem(isVisible ? KTr("DownloadManager.Menu.Hide") : KTr("DownloadManager.Menu.Unhide"));
			item->Enable(!isRunning);
			item->SetBitmap(ImageProvider::GetBitmap(isVisible ? ImageResourceID::MagnifierMinus : ImageResourceID::MagnifierPlus));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, download, isVisible](kxf::UI::MenuEvent& event)
			{
				download->Show(!isVisible);
				download->Save();
				BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtRefreshItems);
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.HideAll"));
			item->Enable(!isListEmpty);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				SetAllHidden(true);
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.HideInstalled"));
			item->Enable(!isListEmpty);
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				SetAllHidden(true, true);
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.UnhideAll"));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				SetAllHidden(false);
			});
		}
		contextMenu.AddSeparator();

		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.AlwaysShowHidden"), wxEmptyString, wxITEM_CHECK);
			item->Check(m_DownloadManager.ShouldShowHiddenDownloads());
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				m_DownloadManager.ShowHiddenDownloads(!m_DownloadManager.ShouldShowHiddenDownloads());
				BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtRefreshItems);
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.ShowArchivesOnly"), wxEmptyString, wxITEM_CHECK);
			item->Check(m_DownloadManager.ShouldShowArchivesOnly());
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				if (AskRefresh())
				{
					m_DownloadManager.SetShowArchivesOnly(!m_DownloadManager.ShouldShowArchivesOnly());
					m_DownloadManager.LoadDownloads();

					BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtRefreshItems);
				}
			});
		}
		contextMenu.AddSeparator();

		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("MainMenu.OpenLocation"));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::FolderOpen));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, download](kxf::UI::MenuEvent& event)
			{
				if (download)
				{
					KxShell::OpenFolderAndSelectItem(download->GetLocalPath());
				}
				else
				{
					KxShell::Execute(GetView(), m_DownloadManager.GetDownloadsLocation(), "open");
				}
			});
		}
		if (modNetwork)
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTrf("DownloadManager.Menu.VisitOnWebSite", modNetwork->GetName()));
			item->Enable(download && download->CanVisitSource());
			item->SetBitmap(ImageProvider::GetBitmap(modNetwork->GetIcon()));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this, download](kxf::UI::MenuEvent& event)
			{
				KxShell::OpenURI(GetView(), download->GetModNetwork()->GetModPageURI(*download));
			});
		}
		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr(wxID_REFRESH));
			item->SetBitmap(ImageProvider::GetBitmap(ImageResourceID::ArrowCircleDouble));
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [this](kxf::UI::MenuEvent& event)
			{
				if (AskRefresh())
				{
					m_DownloadManager.LoadDownloads();
					BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtRefreshItems);
				}
			});
		}
		contextMenu.AddSeparator();

		{
			kxf::UI::MenuItem* item = contextMenu.AddItem(KTr("DownloadManager.Menu.CopyLink"));
			item->Enable(download && download->IsOK());
			item->Bind(kxf::UI::MenuEvent::EvtSelect, [download](kxf::UI::MenuEvent& event)
			{
				Utility::CopyTextToClipboard(download->GetURI().BuildUnescapedURI());
			});
		}

		// Add any options from mod repositories
		for (ModNetworkRepository* repository: INetworkManager::GetInstance()->GetModRepositories())
		{
			repository->OnDownloadMenu(contextMenu, download);
		}

		contextMenu.Show(GetView());
	}

	void DisplayModel::OnDownloadAdded(DownloadEvent& event)
	{
		DownloadItem& item = event.GetDownload();
		if (AddNode(item) && !item.IsHidden())
		{
			ItemsChanged();
		}
	}
	void DisplayModel::OnDownloadRemoved(DownloadEvent& event)
	{
		DownloadItem& item = event.GetDownload();
		if (RemoveNode(item) && !item.IsHidden())
		{
			ItemsChanged();
		}
	}
	void DisplayModel::OnDownloadProgress(DownloadEvent& event)
	{
		DownloadItem& item = event.GetDownload();
		if (!item.IsHidden() && GetView()->IsShownOnScreen())
		{
			if (auto node = GetNode(item); node != m_Nodes.end())
			{
				node->Refresh();
			}
		}
	}

	void DisplayModel::OnDownloadStarted(DownloadEvent& event)
	{
		DownloadItem& item = event.GetDownload();
		item.Save();

		GetView()->GetRootNode().SortChildren();
		INotificationCenter::Notify(KTr("DownloadManager.Notification.DownloadStarted"),
									KTrf("DownloadManager.Notification.DownloadStartedEx", item.GetName()),
									kxf::StdIcon::Information
		);
		OnDownloadProgress(event);
	}
	void DisplayModel::OnDownloadCompleted(DownloadEvent& event)
	{
		DownloadItem& item = event.GetDownload();
		item.Save();

		INotificationCenter::Notify(KTr("DownloadManager.Notification.DownloadCompleted"),
									KTrf("DownloadManager.Notification.DownloadCompletedEx", item.GetName()),
									kxf::StdIcon::Information
		);
		OnDownloadProgress(event);
	}
	void DisplayModel::OnDownloadFailed(DownloadEvent& event)
	{
		DownloadItem& item = event.GetDownload();
		item.Save();

		INotificationCenter::Notify(KTr("DownloadManager.Notification.DownloadFailed"),
									KTrf("DownloadManager.Notification.DownloadFailedEx", item.GetName()),
									KxICON_WARNING
		);
		OnDownloadProgress(event);
	}

	void DisplayModel::OnRefreshItems(DownloadEvent& event)
	{
		RefreshItems();
	}

	DisplayModelNode* DisplayModel::AddNode(DownloadItem& item)
	{
		if (!item.IsHidden() || m_DownloadManager.ShouldShowHiddenDownloads())
		{
			DisplayModelNode& node = m_Nodes.emplace_back(item);
			GetView()->GetRootNode().AttachChild(node);
			node.OnAttachNode();

			return &node;
		}
		return nullptr;
	}
	bool DisplayModel::RemoveNode(DownloadItem& item)
	{
		if (auto node = GetNode(item); node != m_Nodes.end())
		{
			GetView()->GetRootNode().DetachChild(*node);
			m_Nodes.erase(node);
			return true;
		}
		return false;
	}

	bool DisplayModel::AskRefresh()
	{
		if (m_DownloadManager.GetActiveDownloadsCount() != 0)
		{
			KxTaskDialog dialog(GetView(), wxID_NONE, KTr("DownloadManager.RefreshItems.Caption"), KTr("DownloadManager.RefreshItems.Message"), KxBTN_NONE, KxICON_WARNING);
			dialog.AddButton(wxID_REFRESH);
			dialog.AddButton(wxID_NO);
			dialog.SetDefaultButton(wxID_NO);
			
			return dialog.ShowModal() == wxID_REFRESH;
		}
		return true;
	}
	void DisplayModel::RemoveAll(bool installedOnly)
	{
		KxTaskDialog dialog(GetView(), wxID_NONE, KTr("DownloadManager.RemoveDownloadsCaption"), wxEmptyString, KxBTN_YES|KxBTN_NO, KxICON_WARNING);
		dialog.SetMessage(installedOnly ? KTr("DownloadManager.RemoveInstalledDownloadsMessage") : KTr("DownloadManager.RemoveDownloadsMessage"));

		if (dialog.ShowModal() == wxID_YES)
		{
			DownloadItem::RefVector items = IDownloadManager::GetInstance()->GetInactiveDownloads(installedOnly);
			if (!items.empty())
			{
				wxWindowUpdateLocker lock(GetView());
				for (DownloadItem* entry: items)
				{
					IDownloadManager::GetInstance()->RemoveDownload(*entry);
				}
				BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtRefreshItems);
			}
		}
	}
	void DisplayModel::SetAllHidden(bool isHidden, bool installedOnly)
	{
		DownloadItem::RefVector items = IDownloadManager::GetInstance()->GetInactiveDownloads(installedOnly);
		if (!items.empty())
		{
			wxWindowUpdateLocker lock(GetView());
			for (DownloadItem* item: items)
			{
				item->Hide(isHidden);
				item->Save();
			}
			BroadcastProcessor::Get().ProcessEvent(DownloadEvent::EvtRefreshItems);
		}
	}
	void DisplayModel::Install(DownloadItem& item)
	{
		item.Save();
		new InstallWizard::WizardDialog(GetView(), item.GetLocalPath());
	}

	DisplayModel::DisplayModel()
		:m_DownloadManager(*IDownloadManager::GetInstance())
	{
		m_BroadcastReciever.Bind(DownloadEvent::EvtAdded, &DisplayModel::OnDownloadAdded, this);
		m_BroadcastReciever.Bind(DownloadEvent::EvtRemoved, &DisplayModel::OnDownloadRemoved, this);
		m_BroadcastReciever.Bind(DownloadEvent::EvtProgress, &DisplayModel::OnDownloadProgress, this);

		m_BroadcastReciever.Bind(DownloadEvent::EvtStarted, &DisplayModel::OnDownloadStarted, this);
		m_BroadcastReciever.Bind(DownloadEvent::EvtCompleted, &DisplayModel::OnDownloadCompleted, this);
		m_BroadcastReciever.Bind(DownloadEvent::EvtFailed, &DisplayModel::OnDownloadFailed, this);

		m_BroadcastReciever.Bind(DownloadEvent::EvtRefreshItems, &DisplayModel::OnRefreshItems, this);
	}

	void DisplayModel::CreateView(wxWindow* parent)
	{
		using namespace KxDataView2;
		using ColumnID = DisplayModelNode::ColumnID;

		View* view = new View(parent, wxID_NONE, CtrlStyle::VerticalRules|CtrlStyle::CellFocus|CtrlStyle::FitLastColumn);
		view->AssignModel(this);
		view->SetUniformRowHeight(view->GetDefaultRowHeight(UniformHeight::Explorer));

		// Events
		view->Bind(KxDataView2::EvtITEM_ACTIVATED, &DisplayModel::OnActivate, this);
		view->Bind(KxDataView2::EvtITEM_CONTEXT_MENU, &DisplayModel::OnContextMenu, this);
		view->Bind(KxDataView2::EvtCOLUMN_HEADER_RCLICK, [this](Event& event)
		{
			kxf::UI::Menu menu;
			if (GetView()->CreateColumnSelectionMenu(menu))
			{
				GetView()->OnColumnSelectionMenu(menu);
			}
		});

		// Columns
		const ColumnStyle columnStyle = ColumnStyle::Sort|ColumnStyle::Move|ColumnStyle::Size;
		view->AppendColumn<BitmapTextRenderer>(KTr("Generic.Name"), ColumnID::Name, {}, columnStyle);
		view->AppendColumn<TextRenderer>(KTr("Generic.Version"), ColumnID::Version, {}, columnStyle);
		{
			auto [column, renderer] = view->AppendColumn<TextRenderer>(KTr("Generic.Size"), ColumnID::Size, {}, columnStyle);
			renderer.SetAlignment(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
		}
		view->AppendColumn<TextRenderer>(KTr("Generic.Game"), ColumnID::Game, {}, columnStyle);
		view->AppendColumn<TextRenderer>(KTr("Generic.Source"), ColumnID::Source, {}, columnStyle);
		{
			auto [column, renderer] = view->AppendColumn<TextRenderer>(KTr("Generic.Date"), ColumnID::Date, {}, columnStyle);
			column.SortDescending();
		}
		{
			auto [column, renderer] = view->AppendColumn<ProgressRenderer>(KTr("Generic.Status"), ColumnID::Status, {}, columnStyle);
			renderer.SetAlignment(wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL);
		}

		// Add items
		RefreshItems();
	}
	void DisplayModel::RefreshItems()
	{
		KxDataView2::View* view = GetView();
		if (m_DownloadManager.GetDownloadsLocation().IsEmpty())
		{
			view->SetEmptyControlLabel(KTr("DownloadManager.DownloadLocation.Invalid"));
		}
		else
		{
			view->SetEmptyControlLabel(KTr("Generic.NoItemsToDisplay"));
		}

		view->GetRootNode().DetachAllChildren();
		m_Nodes.clear();

		for (DownloadItem* item: m_DownloadManager.GetDownloads())
		{
			AddNode(*item);
		}
		ItemsChanged();
	}
}
