#include "stdafx.h"
#include <Kortex/SaveManager.hpp>
#include <Kortex/Application.hpp>
#include "GameMods/IModManager.h"
#include "VirtualFileSystem/VirtualFSEvent.h"
#include "UI/ImageViewerDialog.h"
#include "Utility/DateTime.h"
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxComparator.h>

namespace Kortex::SaveManager
{
	KxDataView2::ToolTip DisplayModel::GetToolTip(const KxDataView2::Node& node, const KxDataView2::Column& column) const
	{
		const IGameSave& save = GetItem(node);

		switch (column.GetID<ColumnID>())
		{
			case ColumnID::Name:
			{
				return kxf::String::Format(wxS("%1\r\n%2"), save.GetDisplayName(), save.GetFileItem().GetName());
			}
		};
		return VirtualListModel::GetToolTip(node, column);
	}
	wxAny DisplayModel::GetEditorValue(const KxDataView2::Node& node, const KxDataView2::Column& column) const
	{
		const IGameSave& save = GetItem(node);

		switch (column.GetID<ColumnID>())
		{
			case ColumnID::Name:
			{
				kxf::String name = save.GetFileItem().GetName().BeforeLast(wxS('.'));
				return !name.IsEmpty() ? name : save.GetFileItem().GetName();
			}
		};
		return {};
	}
	wxAny DisplayModel::GetValue(const KxDataView2::Node& node, const KxDataView2::Column& column) const
	{
		const IGameSave& save = GetItem(node);

		switch (column.GetID<ColumnID>())
		{
			case ColumnID::Bitmap:
			{
				return save.HasThumbBitmap() ? save.GetThumbBitmap() : ImageProvider::GetBitmap(ImageResourceID::CrossWhite);
			}
			case ColumnID::Name:
			{
				return kxf::String::Format(wxS("%1\r\n<i>%2</i>"), save.GetDisplayName(), save.GetFileItem().GetName());
			}
			case ColumnID::ModificationDate:
			{
				return Utility::DateTime::FormatDateTime(save.GetFileItem().GetModificationTime());
			}
			case ColumnID::Size:
			{
				return KxFile::FormatFileSize(save.GetFileItem().GetFileSize(), 2);
			}
		};
		return {};
	}
	bool DisplayModel::SetValue(KxDataView2::Node& node, KxDataView2::Column& column, const wxAny& value)
	{
		IGameSave& save = GetItem(node);

		switch (column.GetID<ColumnID>())
		{
			case ColumnID::Name:
			{
				const Config& savesConfig = m_Manager.GetConfig();
				KxFileItem& infoPrimary = save.GetFileItem();

				if (savesConfig.HasMultiFileSaveConfig())
				{
					KxFileItem infoSecondary(infoPrimary);
					KxFileItem newInfoPrimary(infoPrimary);
					KxFileItem newInfoSecondary(infoPrimary);

					// Set new name
					infoSecondary.SetName(infoPrimary.GetName().BeforeLast('.') + '.' + savesConfig.GetSecondarySaveExtension());
					newInfoPrimary.SetName(value.As<kxf::String>() + '.' + savesConfig.GetPrimarySaveExtension());
					newInfoSecondary.SetName(value.As<kxf::String>() + '.' + savesConfig.GetSecondarySaveExtension());

					newInfoPrimary.UpdateInfo();
					newInfoSecondary.UpdateInfo();
					if (!newInfoPrimary.IsOK() && !newInfoSecondary.IsOK())
					{
						bool b1 = KxFile(infoPrimary.GetFullPath()).Rename(newInfoPrimary.GetFullPath(), false);
						bool b2 = KxFile(infoSecondary.GetFullPath()).Rename(newInfoSecondary.GetFullPath(), false);
						if (b1 && b2)
						{
							infoPrimary = newInfoPrimary;
							infoPrimary.UpdateInfo();
							BroadcastProcessor::Get().ProcessEvent(SaveEvent::EvtChanged, save);

							m_Workspace->OnSelection(&save);
							return true;
						}
					}
				}
				else
				{
					KxFileItem newPrimaryInfo(infoPrimary);

					kxf::String ext = infoPrimary.GetName().AfterLast(wxS('.'));
					if (ext != infoPrimary.GetName())
					{
						newPrimaryInfo.SetName(value.As<kxf::String>() + wxS('.') + ext);
					}
					else
					{
						newPrimaryInfo.SetName(value.As<kxf::String>());
					}

					newPrimaryInfo.UpdateInfo();
					if (!newPrimaryInfo.IsOK())
					{
						KxFile(infoPrimary.GetFullPath()).Rename(newPrimaryInfo.GetFullPath(), false);
						infoPrimary = newPrimaryInfo;
						BroadcastProcessor::Get().ProcessEvent(SaveEvent::EvtChanged, save);

						m_Workspace->OnSelection(&save);
						return true;
					}
				}
				break;
			}
		};
		return false;
	}
	bool DisplayModel::IsEnabled(const KxDataView2::Node& node, const KxDataView2::Column& column) const
	{
		return GetItem(node).IsOK() && !IModManager::GetInstance()->GetFileSystem().IsEnabled();
	}
	bool DisplayModel::GetAttributes(const KxDataView2::Node& node,
									 const KxDataView2::Column& column,
									 const KxDataView2::CellState& cellState,
									 KxDataView2::CellAttributes& attributes
	) const
	{
		const IGameSave& save = GetItem(node);

		switch (column.GetID<ColumnID>())
		{
			case ColumnID::Name:
			case ColumnID::ModificationDate:
			case ColumnID::Size:
			{
				attributes.Options().Enable(KxDataView2::CellOption::Enabled, IsEnabled(node, column));
				return true;
			}
		};
		return false;
	}

	void DisplayModel::Resort()
	{
		KxDataView2::Column* column =  GetView()->GetSortingColumn();
		if (!column)
		{
			column = GetView()->GetColumnByID(ColumnID::ModificationDate);
		}

		std::sort(m_Saves.begin(), m_Saves.end(), [this, column](IGameSave* left, IGameSave* right)
		{
			const bool ret = Compare(*left, *right, *column);
			return column->IsSortedAscending() ? ret : !ret;
		});
	}
	bool DisplayModel::Compare(const IGameSave& left, const IGameSave& right, const KxDataView2::Column& column) const
	{
		switch (column.GetID<ColumnID>())
		{
			case ColumnID::Name:
			{
				return KxComparator::IsLess(left.GetFileItem().GetName(), right.GetFileItem().GetName());
			}
			case ColumnID::Size:
			{
				return left.GetFileItem().GetFileSize() < right.GetFileItem().GetFileSize();
			}
			case ColumnID::ModificationDate:
			{
				return left.GetFileItem().GetModificationTime() < right.GetFileItem().GetModificationTime();
			}
		};
		return false;
	}
	
	bool DisplayModel::UpdateBitmapSize(int width)
	{
		if (width < 0)
		{
			width = m_BitmapColumn->GetWidth();
		}

		if (width < m_MinBitmapSize.GetWidth())
		{
			m_BitmapColumn->SetWidth(m_MinBitmapSize.GetWidth());
			return false;
		}
		else if (width > m_MaxWidth)
		{
			m_BitmapColumn->SetWidth(m_MaxWidth);
			return false;
		}
		else
		{
			const Utility::BitmapSize oldSize = m_BitmapSize;
			m_BitmapSize.FromWidth(width, m_BitmapRatio);
			if (m_BitmapSize.GetHeight() < m_MinBitmapSize.GetHeight())
			{
				m_BitmapSize.FromHeight(m_MinBitmapSize.GetHeight(), m_BitmapRatio);
			}

			if (m_BitmapSize != oldSize)
			{
				for (IGameSave* save: m_Saves)
				{
					save->ResetThumbBitmap();
				}
				UpdateBitmapCellDimensions();
			}
			return true;
		}
	}
	void DisplayModel::UpdateBitmapCellDimensions()
	{
		m_BitmapColumn->SetWidth(m_BitmapSize.GetWidth());
		if (m_BitmapColumn->IsVisible())
		{
			GetView()->SetUniformRowHeight(m_BitmapSize.GetHeight());
		}
		else
		{
			GetView()->SetUniformRowHeight(-1);
		}
	}
	void DisplayModel::UpdateNativeBitmapSize()
	{
		Utility::BitmapSize& size = m_NativeBitmapSize.IsFullySpecified() ? m_NativeBitmapSize : m_DefaultBitmapSize;
		m_BitmapSize = size;
		m_BitmapRatio = size.GetRatio();
		m_MaxWidth = size.GetWidth() * 2;

		UpdateBitmapCellDimensions();
	}

	void DisplayModel::OnSelectItem(KxDataView2::Event& event)
	{
		if (KxDataView2::Node* node = event.GetNode())
		{
			m_Workspace->OnSelection(&GetItem(*node));
		}
		else
		{
			m_Workspace->OnSelection(nullptr);
		}
	}
	void DisplayModel::OnActivateItem(KxDataView2::Event& event)
	{
		KxDataView2::Node* node = event.GetNode();
		KxDataView2::Column* column = event.GetColumn();
		if (column && node)
		{
			IGameSave& save = GetItem(*node);

			switch (column->GetID<ColumnID>())
			{
				case ColumnID::Bitmap:
				{
					UI::ImageViewerDialog dialog(GetView(), save.GetFileItem().GetName());

					UI::ImageViewerEvent evt;
					evt.SetBitmap(save.GetBitmap());
					dialog.Navigate(evt);

					dialog.ShowModal();
					break;
				}
				case ColumnID::Name:
				{
					if (IsEnabled(*node, *column))
					{
						GetView()->EditItem(*node, *column);
					}
					break;
				}
			};
		}
	}
	void DisplayModel::OnContextMenu(KxDataView2::Event& event)
	{
		if (KxDataView2::Node* node = event.GetNode())
		{
			m_Workspace->OnContextMenu(&GetItem(*node));
		}
		else
		{
			m_Workspace->OnContextMenu(nullptr);
		}
	}
	void DisplayModel::OnCacheHint(KxDataView2::Event& event)
	{
		if (m_BitmapColumn->IsVisible())
		{
			for (KxDataView2::Row row = event.GetCacheHintFrom(); row <= event.GetCacheHintTo(); row++)
			{
				IGameSave& save = *m_Saves[row];
				if (!save.IsOK() || !save.HasThumbBitmap())
				{
					save.ReadFile();

					wxBitmap bitmap = save.GetBitmap();
					if (!m_NativeBitmapSize.IsFullySpecified() && bitmap.IsOk())
					{
						m_NativeBitmapSize = bitmap.GetSize();
						UpdateNativeBitmapSize();
					}
					save.SetThumbBitmap(m_BitmapSize.ScaleMaintainRatio(bitmap, 0, 4));
				}
			}
		}
	}
	
	void DisplayModel::OnHeaderSorted(KxDataView2::Event& event)
	{
		Resort();
	}
	void DisplayModel::OnHeaderResized(KxDataView2::Event& event)
	{
		KxDataView2::Column* column = event.GetColumn();
		if (column == m_BitmapColumn && column->IsVisible())
		{
			int width = event.GetWidth();
			if (event.GetEventType() == KxDataView2::EvtCOLUMN_HEADER_WIDTH_FIT)
			{
				width = (m_NativeBitmapSize.IsFullySpecified() ? m_NativeBitmapSize : m_DefaultBitmapSize).GetWidth();
			}

			const int maxAvailableWidth = 0.95 * GetView()->GetClientSize().GetWidth();
			if (width < maxAvailableWidth && UpdateBitmapSize(width))
			{
				width = m_BitmapSize.GetWidth();
			}
			else
			{
				event.Veto();
			}
			event.SetWidth(width);
		}
	}
	void DisplayModel::OnHeaderMenuItem(KxDataView2::Event& event)
	{
		UpdateBitmapCellDimensions();
	}

	void DisplayModel::OnFiltersChanged(BroadcastEvent& event)
	{
		RefreshItems();
	}
	void DisplayModel::OnVFSToggled(BroadcastEvent& event)
	{
		if (m_Workspace->IsActive())
		{
			GetView()->Refresh();
		}
	}

	DisplayModel::DisplayModel()
		:m_Manager(*ISaveManager::GetInstance())
	{
		m_Workspace = Workspace::GetInstance();
		m_BroadcastReciever.Bind(SaveEvent::EvtFiltersChanged, &DisplayModel::OnFiltersChanged, this);
		m_BroadcastReciever.Bind(VirtualFSEvent::EvtMainToggled, &DisplayModel::OnVFSToggled, this);
	}

	void DisplayModel::CreateView(wxWindow* parent)
	{
		using namespace KxDataView2;

		View* view = new View(parent, wxID_NONE, CtrlStyle::VerticalRules|CtrlStyle::CellFocus|CtrlStyle::FitLastColumn);
		view->AssignModel(this);

		// Events
		view->Bind(KxDataView2::EvtITEM_SELECTED, &DisplayModel::OnSelectItem, this);
		view->Bind(KxDataView2::EvtITEM_ACTIVATED, &DisplayModel::OnActivateItem, this);
		view->Bind(KxDataView2::EvtITEM_CONTEXT_MENU, &DisplayModel::OnContextMenu, this);;
		view->Bind(KxDataView2::EvtVIEW_CACHE_HINT, &DisplayModel::OnCacheHint, this);

		view->Bind(KxDataView2::EvtCOLUMN_SORTED, &DisplayModel::OnHeaderSorted, this);
		view->Bind(KxDataView2::EvtCOLUMN_RESIZE, &DisplayModel::OnHeaderResized, this);
		view->Bind(KxDataView2::EvtCOLUMN_END_RESIZE, &DisplayModel::OnHeaderResized, this);
		view->Bind(KxDataView2::EvtCOLUMN_HEADER_WIDTH_FIT, &DisplayModel::OnHeaderResized, this);
		view->Bind(KxDataView2::EvtCOLUMN_HEADER_MENU_ITEM, &DisplayModel::OnHeaderMenuItem, this);

		// Columns
		constexpr ColumnStyle columnStyle = ColumnStyle::Sort|ColumnStyle::Move|ColumnStyle::Size;
		view->AppendColumn<BitmapRenderer>(KTr("Generic.Image"), ColumnID::Bitmap, {}, ColumnStyle::Move|ColumnStyle::Size);
		view->AppendColumn<HTMLRenderer, TextEditor>(KTr("Generic.Name"), ColumnID::Name, {}, columnStyle);
		view->AppendColumn<TextRenderer>(KTr("Generic.ModificationDate"), ColumnID::ModificationDate, {}, columnStyle);
		view->AppendColumn<TextRenderer>(KTr("Generic.Size"), ColumnID::Size, {}, columnStyle);

		view->GetColumnByID(ColumnID::Size)->GetRenderer().SetAlignment(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
		view->GetColumnByID(ColumnID::ModificationDate)->SortDescending();
		
		// Bitmap setup
		m_BitmapColumn = view->GetColumnByID(ColumnID::Bitmap);
		m_BitmapColumn->SetMinWidth(view->GetUniformRowHeight());
	}
	void DisplayModel::RefreshItems()
	{
		m_DefaultBitmapSize = m_Manager.GetConfig().GetBitmapSize();
		m_MinBitmapSize.FromHeight(m_BitmapColumn->GetMinWidth(), m_DefaultBitmapSize.GetRatio());
		UpdateNativeBitmapSize();
		UpdateBitmapSize();
		UpdateBitmapCellDimensions();

		m_Saves = m_Manager.GetSaves();
		Resort();
		SetItemCount(m_Saves.size());
	}
}
