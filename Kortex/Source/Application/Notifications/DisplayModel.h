#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf/UI/Controls/DataView.h>
#include "BaseNotification.h"
#include "Utility/BitmapSize.h"

namespace Kortex::Notifications
{
	class DisplayModel: public kxf::UI::DataView::VirtualListModel
	{
		private:
			using Node = kxf::UI::DataView::Node;
			using Column = kxf::UI::DataView::Column;
			using CellState = kxf::UI::DataView::CellState;
			using CellAttribute = kxf::UI::DataView::CellAttribute;

			using ToolTip = kxf::UI::DataView::ToolTip;
			using ItemEvent = kxf::UI::DataView::ItemEvent;

		private:
			Utility::BitmapSize m_BitmapSize;
			INotification::Vector& m_Notifications;

		private:
			wxAny GetValue(const Node& node, const Column& column) const override;
			ToolTip GetToolTip(const Node& node, const Column& column) const override;
			bool GetAttributes(const Node& node, const Column& column, const CellState& cellState, CellAttribute& attributes) const override;

			void OnSelectItem(ItemEvent& event);
			void OnActivateItem(ItemEvent& event);

			const INotification& GetItem(const Node& node) const
			{
				return *m_Notifications[node.GetRow()];
			}
			INotification& GetItem(Node& node)
			{
				return *m_Notifications[node.GetRow()];
			}
			kxf::String FormatText(const INotification& notification) const;

		public:
			DisplayModel();

		public:
			void CreateView(wxWindow* parent);
			void OnShowWindow();
			void RefreshItems();
	};
}
