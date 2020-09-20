#include "stdafx.h"
#include "DisplayModel.h"
#include "Utility/Common.h"
#include <Kortex/GameInstance.hpp>
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/kxf::UI::Menu.h>

namespace Kortex::NetworkManager::NXMHandler
{
	void DisplayModel::OnActivate(KxDataView2::Event& event)
	{
		if (event.GetNode() && event.GetColumn())
		{
			GetView()->EditItem(*event.GetNode(), *event.GetColumn());
		}
	}

	DisplayModel::DisplayModel(OptionStore& options)
		:m_Options(options)
	{
	}

	void DisplayModel::CreateView(wxWindow* parent)
	{
		using namespace KxDataView2;

		// View
		View* view = new View(parent, wxID_NONE, CtrlStyle::VerticalRules|CtrlStyle::CellFocus|CtrlStyle::FitLastColumn);
		view->AssignModel(this);
		view->SetUniformRowHeight(view->GetDefaultRowHeight(UniformHeight::Explorer));

		// Columns
		ColumnStyle columnStyle = ColumnStyle::Move|ColumnStyle::Size|ColumnStyle::Sort;
		view->AppendColumn<TextRenderer>(KTr("NetworkManager.NXMHandler.NexusID"), ColumnID::NexusID, {}, columnStyle);
		{
			auto [column, r] = view->AppendColumn<TextRenderer>(KTr("NetworkManager.NXMHandler.Game"), ColumnID::Game, {}, columnStyle);
			column.SetVisible(false);
		}
		{
			auto [c, r, editor] = view->AppendColumn<TextRenderer, ComboBoxEditor>(KTr("NetworkManager.NXMHandler.Target"), ColumnID::Target, {}, columnStyle);
			editor.AlwaysUseStringSelection(false);
			editor.EndEditOnCloseup(true);
			editor.SetEditable(false);
			editor.AutoPopup();

			editor.AddItem(Utility::MakeNoneLabel());
			editor.AddItem(Utility::MakeBracketedLabel(KTr("NetworkManager.NXMHandler.ExternalProgram")));
			for (const auto& instance: IGameInstance::GetShallowInstances())
			{
				editor.AddItem(instance->GetInstanceID());
			}
		}

		// Events
		view->Bind(EvtITEM_ACTIVATED, &DisplayModel::OnActivate, this);
		view->Bind(EvtCOLUMN_HEADER_RCLICK, [view](Event& event)
		{
			kxf::UI::Menu menu;
			if (view->CreateColumnSelectionMenu(menu))
			{
				view->OnColumnSelectionMenu(menu);
			}
		});
	}
	void DisplayModel::RefreshItems()
	{
		std::unordered_set<kxf::String> hash;
		for (const auto& instance: IGameInstance::GetTemplates())
		{
			if (hash.insert(instance->GetVariables().GetVariable("NexusDomainName").AsString()).second)
			{
				m_Nodes.emplace_back(m_Options, *instance);
			}
		}

		GetView()->GetRootNode().AttachChildren(m_Nodes);
		ItemsChanged();
	}
}
