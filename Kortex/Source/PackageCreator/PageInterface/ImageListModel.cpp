#include "stdafx.h"
#include <Kortex/ScreenshotsGallery.hpp>
#include "ImageListModel.h"
#include "PackageProject/ModPackageProject.h"
#include "UI/TextEditDialog.h"
#include "UI/ImageViewerDialog.h"
#include "Utility/OperationWithProgress.h"
#include <Kortex/Application.hpp>
#include <KxFramework/KxFile.h>
#include <KxFramework/KxString.h>
#include <KxFramework/KxFileOperationEvent.h>
#include <KxFramework/KxDualProgressDialog.h>
#include <KxFramework/KxFileBrowseDialog.h>
#include <KxFramework/DataView/KxDataViewMainWindow.h>

namespace
{
	enum ColumnID
	{
		Bitmap,
		Visible,
		Main,
		Header,
		Path,
		Description,
	};
	enum MenuID
	{
		ImportFiles,
		AddMultipleFiles,
	};
}

namespace Kortex::PackageDesigner::PageInterfaceNS
{
	Utility::BitmapSize ImageListModel::GetThumbnailSize(const wxWindow* window)
	{
		Utility::BitmapSize size;
		size.FromHeight(64, Utility::BitmapSize::r16_9);

		if (window)
		{
			return window->FromDIP(size);
		}
		return size;
	}
	void ImageListModel::LoadBitmap(PackageProject::ImageItem* entry, const wxWindow* window)
	{
		wxImage image(entry->GetPath(), wxBITMAP_TYPE_ANY);
		if (image.IsOk())
		{
			entry->SetBitmap(GetThumbnailSize(window).ScaleMaintainRatio(image, 4, 4));
		}
		else
		{
			entry->SetCanNotHaveBitmap(true);
		}
	}
	
	void ImageListModel::OnInitControl()
	{
		Utility::BitmapSize bitmapSize = GetThumbnailSize(GetView());

		GetView()->SetUniformRowHeight(bitmapSize.GetHeight() + 4);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_ACTIVATED, &ImageListModel::OnActivateItem, this);
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &ImageListModel::OnContextMenu, this);
		GetView()->Bind(KxEVT_DATAVIEW_CACHE_HINT, &ImageListModel::OnCacheHint, this);
	
		GetView()->AppendColumn<KxDataViewBitmapRenderer>(wxEmptyString, ColumnID::Bitmap, KxDATAVIEW_CELL_INERT, bitmapSize.GetWidth() + 4, KxDV_COL_NONE);
		GetView()->AppendColumn<KxDataViewToggleRenderer>(KTr("PackageCreator.PageInterface.ImageList.Show"), ColumnID::Visible, KxDATAVIEW_CELL_ACTIVATABLE, KxCOL_WIDTH_AUTOSIZE);
	
		// Main
		{
			auto info = GetView()->AppendColumn<KxDataViewToggleRenderer>(KTr("PackageCreator.PageInterface.ImageList.Main"), ColumnID::Main, KxDATAVIEW_CELL_ACTIVATABLE, KxCOL_WIDTH_AUTOSIZE);
			info.GetRenderer()->SetDefaultToggleType(KxDataViewToggleRenderer::ToggleType::RadioBox);
		}
	
		// Header
		{
			auto info = GetView()->AppendColumn<KxDataViewToggleRenderer>(KTr("PackageCreator.PageInterface.ImageList.Header"), ColumnID::Header, KxDATAVIEW_CELL_ACTIVATABLE, KxCOL_WIDTH_AUTOSIZE);
			info.GetRenderer()->SetDefaultToggleType(KxDataViewToggleRenderer::ToggleType::RadioBox);
		}
	
		GetView()->AppendColumn<KxDataViewTextRenderer>(KTr("PackageCreator.PageInterface.ImageList.Value"), ColumnID::Path, KxDATAVIEW_CELL_INERT, KxDVC_DEFAULT_WIDTH);
		GetView()->AppendColumn<KxDataViewHTMLRenderer>(KTr("Generic.Description"), ColumnID::Description, KxDATAVIEW_CELL_INERT, KxDVC_DEFAULT_WIDTH);
	}
	
	void ImageListModel::GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const
	{
		const PackageProject::ImageItem* entry = GetDataEntry(row);
		if (entry)
		{
			auto IsChecked = [entry](const kxf::String& path)
			{
				return !path.IsEmpty() && !entry->GetPath().IsEmpty() && entry->GetPath() == path;
			};
	
			switch (column->GetID())
			{
				case ColumnID::Bitmap:
				{
					value = entry->GetBitmap();
					break;
				}
				case ColumnID::Main:
				{
					value = IsChecked(m_Interface->GetMainImage());
					break;
				}
				case ColumnID::Header:
				{
					value = IsChecked(m_Interface->GetHeaderImage());
					break;
				}
				case ColumnID::Visible:
				{
					value = entry->IsVisible();
					break;
				}
				case ColumnID::Path:
				{
					value = entry->GetPath();
					break;
				}
				case ColumnID::Description:
				{
					value = entry->GetDescriptionRaw();
					break;
				}
			};
		}
	}
	bool ImageListModel::SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column)
	{
		PackageProject::ImageItem* entry = GetDataEntry(row);
		if (entry)
		{
			auto SetChecked = [&value, entry]() -> kxf::String
			{
				return value.As<bool>() ? entry->GetPath() : wxEmptyString;
			};
	
			switch (column->GetID())
			{
				case ColumnID::Main:
				{
					m_Interface->SetMainImage(SetChecked());
					ChangeNotify();
					return true;
				}
				case ColumnID::Header:
				{
					m_Interface->SetHeaderImage(SetChecked());
					ChangeNotify();
					return true;
				}
				case ColumnID::Visible:
				{
					entry->SetVisible(value.As<bool>());
					ChangeNotify();
					return true;
				}
			};
		}
		return false;
	}
	
	bool ImageListModel::DoTrackImagePath(const kxf::String& trackedID, const kxf::String& newID, bool remove) const
	{
		PackageProject::InterfaceSection& interfaceConfig = GetProject().GetInterface();
	
		// Main image
		if (interfaceConfig.GetMainImage() == trackedID)
		{
			interfaceConfig.SetMainImage(remove ? wxEmptyString : newID);
		}
	
		// Header image
		if (interfaceConfig.GetHeaderImage() == trackedID)
		{
			interfaceConfig.SetHeaderImage(remove ? wxEmptyString : newID);
		}
	
		// Manual components
		for (auto& step: GetProject().GetComponents().GetSteps())
		{
			for (auto& group: step->GetGroups())
			{
				for (auto& entry: group->GetItems())
				{
					if (entry->GetImage() == trackedID)
					{
						entry->SetImage(remove ? wxEmptyString : newID);
					}
				}
			}
		}
	
		return true;
	}
	
	void ImageListModel::OnActivateItem(KxDataViewEvent& event)
	{
		if (event.GetColumn())
		{
			PackageProject::ImageItem* entry = GetDataEntry(GetRow(event.GetItem()));
			switch (event.GetColumn()->GetID())
			{
				case ColumnID::Bitmap:
				{
					if (entry && entry->HasBitmap())
					{
						UI::ImageViewerDialog dialog(GetView(), entry->GetPath());
						
						UI::ImageViewerEvent imageEvent;
						imageEvent.SetFilePath(entry->GetPath());
						imageEvent.SetDescription(entry->GetDescription());
						dialog.Navigate(imageEvent);
						dialog.ShowModal();
					}
					break;
				}
				case ColumnID::Path:
				{
					if (entry)
					{
						KxFileBrowseDialog dialog(GetView(), KxID_NONE, KxFBD_OPEN);
						dialog.SetFolder(entry->GetPath().BeforeLast('\\'));
						dialog.AddFilter(KxString::Join(IScreenshotsGallery::GetSupportedExtensions(), ";"), KTr("FileFilter.Images"));
						dialog.AddFilter("*", KTr("FileFilter.AllFiles"));
						if (dialog.ShowModal() == KxID_OK)
						{
							kxf::String newPath = dialog.GetResult();
							TrackChangeID(entry->GetPath(), newPath);
	
							entry->ResetBitmap();
							entry->SetPath(newPath);
							NotifyChangedItem(event.GetItem());
						}
					}
					break;
				}
				case ColumnID::Description:
				{
					if (entry)
					{
						UI::TextEditDialog dialog(GetView());
						dialog.SetText(entry->GetDescriptionRaw());
						if (dialog.ShowModal() == KxID_OK && dialog.IsModified())
						{
							entry->SetDescription(dialog.GetText());
							NotifyChangedItem(event.GetItem());
						}
					}
					break;
				}
			};
		}
	}
	void ImageListModel::OnContextMenu(KxDataViewEvent& event)
	{
		KxDataViewItem item = event.GetItem();
		const PackageProject::ImageItem* entry = GetDataEntry(GetRow(item));
	
		KxMenu menu;
		{
			KxMenu* allItems = CreateAllItemsMenu(menu);
			CreateAllItemsMenuEntry(allItems, ColumnID::Description);
			menu.AddSeparator();
		}
	
		{
			KxMenuItem* item = menu.Add(new KxMenuItem(MenuID::ImportFiles, KTr("PackageCreator.ImportFiles")));
			item->SetBitmap(ImageProvider::GetBitmap(Imagekxf::ResourceID::FolderSearchResult));
		}
		{
			KxMenuItem* item = menu.Add(new KxMenuItem(MenuID::AddMultipleFiles, KTr("PackageCreator.AddMultipleFiles")));
			item->SetBitmap(ImageProvider::GetBitmap(Imagekxf::ResourceID::DocumentsPlus));
		}
		menu.AddSeparator();
		{
			KxMenuItem* item = menu.Add(new KxMenuItem(KxID_REMOVE, KTr(KxID_REMOVE)));
			item->Enable(entry != nullptr);
		}
		{
			KxMenuItem* item = menu.Add(new KxMenuItem(KxID_CLEAR, KTr(KxID_CLEAR)));
			item->Enable(!IsEmpty());
		}
	
		switch (menu.Show(GetView()))
		{
			case MenuID::ImportFiles:
			{
				OnImportFiles();
				break;
			}
			case MenuID::AddMultipleFiles:
			{
				OnAddMultipleItems();
				break;
			}
			case KxID_REMOVE:
			{
				OnRemoveEntry(item);
				break;
			}
			case KxID_CLEAR:
			{
				OnClearList();
				break;
			}
		};
	};
	void ImageListModel::OnCacheHint(KxDataViewEvent& event)
	{
		for (size_t row = event.GetCacheHintFrom(); row <= event.GetCacheHintTo(); row++)
		{
			KxDataViewItem item = GetView()->GetMainWindow()->GetItemByRow(row);
			PackageProject::ImageItem* entry = GetDataEntry(GetRow(item));
			if (entry && !entry->HasBitmap() && !entry->CanNotHaveBitmap())
			{
				GetView()->CallAfter([this, item, entry]()
				{
					LoadBitmap(entry, GetView());
					ItemChanged(item);
				});
			}
		}
	}
	
	void ImageListModel::OnAllItemsMenuSelect(KxDataViewColumn* column)
	{
		switch (column->GetID())
		{
			case ColumnID::Description:
			{
				UI::TextEditDialog dialog(GetView());
				if (dialog.ShowModal() == KxID_OK)
				{
					for (auto& entry: *GetDataVector())
					{
						entry.SetDescription(dialog.GetText());
					}
					NotifyAllItemsChanged();
				}
				break;
			}
		};
	}
	
	void ImageListModel::OnImportFiles()
	{
		KxFileBrowseDialog dialog(GetView(), KxID_NONE, KxFBD_OPEN_FOLDER);
		if (dialog.ShowModal() == KxID_OK)
		{
			kxf::String source = dialog.GetResult();
			auto operation = new Utility::OperationWithProgressDialog<KxFileOperationEvent>(true, GetView());
			operation->OnRun([this, operation, source]()
			{
				KxEvtFile source(source);
				operation->LinkHandler(&source, KxEVT_FILEOP_SEARCH);
				KxStringVector files = source.Find(IScreenshotsGallery::GetSupportedExtensions(), KxFS_FILE, true);
	
				size_t count = files.size();
				size_t processed = 0;
				for (const kxf::String& path: files)
				{
					if (operation->CanContinue())
					{
						GetDataVector()->emplace_back(PackageProject::ImageItem(path, wxEmptyString, true));
					}
					else
					{
						break;
					}
				}
			});
			operation->OnEnd([this]()
			{
				RefreshItems();
				ChangeNotify();
			});
			operation->SetDialogCaption(KTr("Generic.FileSearchInProgress"));
			operation->Run();
		}
	}
	void ImageListModel::OnAddMultipleItems()
	{
		KxFileBrowseDialog dialog(GetView(), KxID_NONE, KxFBD_OPEN);
		dialog.SetOptionEnabled(KxFBD_ALLOW_MULTISELECT);
		dialog.AddFilter(KxString::Join(IScreenshotsGallery::GetSupportedExtensions(), ";"), KTr("FileFilter.Images"));
		dialog.AddFilter("*", KTr("FileFilter.AllFiles"));
		if (dialog.ShowModal() == KxID_OK)
		{
			for (const kxf::String& path: dialog.GetResults())
			{
				GetDataVector()->emplace_back(PackageProject::ImageItem(path, wxEmptyString, true));
			}
			RefreshItems();
			ChangeNotify();
		}
	}
	void ImageListModel::OnRemoveEntry(const KxDataViewItem& item)
	{
		if (PackageProject::ImageItem* entry = GetDataEntry(GetRow(item)))
		{
			TrackRemoveID(entry->GetPath());
			RemoveItemAndNotify(*GetDataVector(), item);
		}
	}
	void ImageListModel::OnClearList()
	{
		for (size_t i = 0; i < GetItemCount(); i++)
		{
			TrackRemoveID(GetDataEntry(i)->GetPath());
		}
		ClearItemsAndNotify(*GetDataVector());
	}
	
	void ImageListModel::SetProject(ModPackageProject& projectData)
	{
		m_Interface = &projectData.GetInterface();
	}
}
