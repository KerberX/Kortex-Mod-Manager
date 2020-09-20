#include "stdafx.h"
#include "SelectorDisplayModel.h"
#include "Application/Resources/IImageProvider.h"
#include "Network/ModNetwork/Nexus.h"
#include "Utility/Common.h"
#include <Kortex/ModTagManager.hpp>
#include <Kortex/NetworkManager.hpp>

namespace Kortex::ModTagManager
{
	void SelectorDisplayModel::OnInitControl()
	{
		GetView()->Bind(KxEVT_DATAVIEW_ITEM_ACTIVATED, &SelectorDisplayModel::OnActivate, this);
		GetView()->Bind(wxEVT_CHAR_HOOK, &SelectorDisplayModel::OnKeyDown, this);

		// Priority group
		if (IsFullFeatured())
		{
			auto info = GetView()->AppendColumn<KxDataViewToggleRenderer>(KTr("TagManager.PrimaryTag"), ColumnID::PriorityTag, KxDATAVIEW_CELL_ACTIVATABLE);
			info.GetRenderer()->SetDefaultToggleType(KxDataViewToggleRenderer::ToggleType::RadioBox);
		}
		
		// Expanded
		if (IsFullFeatured())
		{
			GetView()->AppendColumn<KxDataViewToggleRenderer>(KTr("Generic.Expanded"), ColumnID::Expanded, KxDATAVIEW_CELL_ACTIVATABLE);
		}

		// Name
		GetView()->AppendColumn<KxDataViewBitmapTextToggleRenderer, KxDataViewTextEditor>(KTr("Generic.Name"), ColumnID::Name, KxDATAVIEW_CELL_EDITABLE|KxDATAVIEW_CELL_ACTIVATABLE);

		// NexusID
		if (IsFullFeatured())
		{
			using namespace NetworkManager;
			if (const IModNetwork* nexus = NexusModNetwork::GetInstance())
			{
				auto info = GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewTextEditor>(nexus->GetName(), ColumnID::NexusID, KxDATAVIEW_CELL_EDITABLE);
				info.GetColumn()->SetBitmap(ImageProvider::GetBitmap(nexus->GetIcon()));
			}
		}

		// Color
		if (IsFullFeatured())
		{
			GetView()->AppendColumn<KxDataViewTextRenderer, KxDataViewColorEditor>(KTr("Generic.Color"), ColumnID::Color, KxDATAVIEW_CELL_EDITABLE);
		}
	}

	void SelectorDisplayModel::GetEditorValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const
	{
		const IModTag* entry = GetDataEntry(row);
		if (entry)
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					value = entry->GetName();
					break;
				}
				case ColumnID::NexusID:
				{
					const INexusModTag* nexusTag = nullptr;
					if (entry->QueryInterface(nexusTag))
					{
						value = nexusTag->GetNexusID();
					}
					break;
				}
				case ColumnID::Color:
				{
					value = entry->GetColor();
					break;
				}
			};
		}
	}
	void SelectorDisplayModel::GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const
	{
		const IModTag* tag = GetDataEntry(row);
		if (tag)
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					wxBitmap icon = !tag->IsDefaultTag() ? ImageProvider::GetBitmap(ImageResourceID::PlusSmall) : wxNullBitmap;
					value = KxDataViewBitmapTextToggleValue(m_TagStore->HasTag(*tag), tag->GetName(), icon, KxDataViewBitmapTextToggleValue::CheckBox);
					break;
				}
				case ColumnID::PriorityTag:
				{
					value = tag == m_PrimaryTag;
					break;
				}
				case ColumnID::Expanded:
				{
					value = tag->IsExpanded();
					break;
				}
				case ColumnID::NexusID:
				{
					const INexusModTag* nexusTag = nullptr;
					if (tag->QueryInterface(nexusTag))
					{
						value = nexusTag->GetNexusID();
					}
					else
					{
						value = Utility::MakeNoneLabel();
					}
					break;
				}
				case ColumnID::Color:
				{
					KxColor color = tag->GetColor();
					value = color.IsOk() ? color.ToString(KxColor::C2S::HTML) : Utility::MakeNoneLabel();
					break;
				}
			};
		}
	}
	bool SelectorDisplayModel::SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column)
	{
		auto QueueRefresh = [this]()
		{
			GetView()->CallAfter(&KxDataViewCtrl::Refresh, true, nullptr);
		};

		IModTag* tag = GetDataEntry(row);
		if (tag)
		{
			switch (column->GetID())
			{
				case ColumnID::Name:
				{
					if (value.CheckType<kxf::String>())
					{
						kxf::String name = value.As<kxf::String>();
						if (!name.IsEmpty() && tag->GetID() != name)
						{
							const bool hasTag = m_TagStore->HasTag(*tag);
							m_TagStore->ToggleTag(*tag, false);

							tag->SetID(name);
							tag->SetName(name);

							// Re-add this tag if needed
							if (hasTag)
							{
								m_TagStore->ToggleTag(*tag, true);
							}

							m_IsModified = true;
							return true;
						}
					}
					else
					{
						const bool hasTag = m_TagStore->HasTag(*tag);
						m_TagStore->ToggleTag(*tag, value.As<bool>());
						m_IsModified = true;

						QueueRefresh();
						return hasTag;
					}
					return false;
				}
				case ColumnID::Expanded:
				{
					tag->SetExpanded(value.As<bool>());
					m_IsModified = true;
					return true;
				}
				case ColumnID::PriorityTag:
				{
					bool checked = value.As<bool>();
					if (checked && m_PrimaryTag != tag)
					{
						m_PrimaryTag = tag;
						m_IsModified = true;
						QueueRefresh();
						return true;
					}
					else if (!checked && m_PrimaryTag)
					{
						m_PrimaryTag = nullptr;
						m_IsModified = true;
						QueueRefresh();
						return true;
					}
					return false;
				}
				case ColumnID::NexusID:
				{
					INexusModTag* nexusTag = nullptr;
					int id = -1;
					if (value.GetAs(&id) && id > 0 && tag->QueryInterface(nexusTag))
					{
						nexusTag->SetNexusID(id);
						m_IsModified = true;
						return true;
					}
					break;
				}
				case ColumnID::Color:
				{
					tag->SetColor(value.As<KxColor>());
					m_IsModified = true;
					return true;
				}
			};
		}
		return false;
	}
	bool SelectorDisplayModel::IsEnabledByRow(size_t row, const KxDataViewColumn* column) const
	{
		IModTag* tag = GetDataEntry(row);
		if (tag)
		{
			switch (column->GetID())
			{
				case ColumnID::PriorityTag:
				{
					return m_TagStore->HasTag(*tag);
				}
			};
			return true;
		}
		return false;
	}
	bool SelectorDisplayModel::GetItemAttributesByRow(size_t row, const KxDataViewColumn* column, KxDataViewItemAttributes& attribute, KxDataViewCellState cellState) const
	{
		const IModTag* entry = GetDataEntry(row);
		if (entry)
		{
			attribute.SetEnabled(IsEnabledByRow(row, column));

			switch (column->GetID())
			{
				case ColumnID::Color:
				{
					KxColor color = entry->GetColor();
					if (color.IsOk())
					{
						attribute.SetBackgroundColor(color);
						attribute.SetForegroundColor(color.GetContrastColor(GetView()));
					}
					break;
				}
			};
			return true;
		}
		return false;
	}

	void SelectorDisplayModel::OnActivate(KxDataViewEvent& event)
	{
		KxDataViewItem item = event.GetItem();
		KxDataViewColumn* column = event.GetColumn();
		if (item.IsOK() && column)
		{
			GetView()->EditItem(item, column);
		}
	}
	void SelectorDisplayModel::OnKeyDown(wxKeyEvent& event)
	{
		if (event.GetKeyCode() == WXK_DELETE)
		{
			KxDataViewItem item = GetView()->GetSelection();
			KxDataViewColumn* column = GetView()->GetCurrentColumn();
			if (item.IsOK() && column->GetID() == ColumnID::Color)
			{
				if (IModTag* tag = GetDataEntry(GetRow(item)))
				{
					tag->ResetColor();
					ItemChanged(item);
				}
			}
		}
		event.Skip();
	}

	SelectorDisplayModel::SelectorDisplayModel(bool isFullFeatured)
		:m_FullFeatured(isFullFeatured)
	{
		SetDataViewFlags(GetDataViewFlags()|KxDV_NO_TIMEOUT_EDIT);
	}

	void SelectorDisplayModel::SetDataVector(ModTagStore& tagStore, IGameMod& mod)
	{
		m_Mod = &mod;
		m_TagStore = &tagStore;
		m_PrimaryTag = mod.GetTagStore().GetPrimaryTag();

		RefreshItems();
	}
	void SelectorDisplayModel::SetDataVector(ModTagStore& tagStore)
	{
		m_Mod = nullptr;
		m_TagStore = &tagStore;
		m_PrimaryTag = tagStore.GetPrimaryTag();

		RefreshItems();
	}

	size_t SelectorDisplayModel::GetItemCount() const
	{
		return m_TagStore ? IModTagManager::GetInstance()->GetTagsCount() : 0;
	}
	IModTag* SelectorDisplayModel::GetDataEntry(size_t index) const
	{
		IModTag::Vector& tags = IModTagManager::GetInstance()->GetTags();
		if (m_TagStore && index < tags.size())
		{
			return tags[index].get();
		}
		return nullptr;
	}

	void SelectorDisplayModel::ApplyChanges()
	{
		if (m_TagStore)
		{
			if (m_PrimaryTag)
			{
				m_TagStore->SetPrimaryTag(*m_PrimaryTag);
			}
			else
			{
				m_TagStore->ClearPrimaryTag();
			}
		}
	}
}
