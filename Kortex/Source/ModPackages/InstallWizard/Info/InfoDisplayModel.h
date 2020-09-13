#pragma once
#include <Kortex/Kortex.hpp>
#include "InfoPage.h"
#include <KxFramework/DataView2/DataView2.h>

namespace Kortex::InstallWizard::InfoPageNS
{
	enum class InfoKind
	{
		None = 0,
		ID,
		Name,
		ModSource,
		Tags,
	};

	class Item
	{
		friend class InfoDisplayModel;

		private:
			Utility::LabeledValue Value;
			kxf::ResourceID IconID;
			InfoKind Type = InfoKind::None;

		public:
			Item(const Utility::LabeledValue& value)
				:Value(value)
			{
			}
	};
}

namespace Kortex::InstallWizard::InfoPageNS
{
	class InfoDisplayModel: public KxDataView2::VirtualListModel, public KxDataView2::TypeAliases
	{
		private:
			InfoPage& m_Page;
			std::vector<Item> m_Items;

		private:
			wxAny GetValue(const Node& node, const Column& column) const override;
			wxAny GetEditorValue(const Node& node, const Column& column) const override;
			bool SetValue(Node& node, Column& column, const wxAny& value) override;
			bool IsEnabled(const Node& node, const Column& column) const override;
			bool GetAttributes(const Node& node, const Column& column, const CellState& cellState, CellAttributes& attributes) const override;

			bool CheckModID(const kxf::String& id);
			void OnActivateItem(Event& event);

		public:
			InfoDisplayModel(InfoPage& page, size_t initialCount = 0)
				:m_Page(page)
			{
				m_Items.reserve(initialCount);
			}

		public:
			void CreateView(wxWindow* parent);
			void AddItem(const Utility::LabeledValue& value, const kxf::ResourceID& image = {}, InfoKind type = InfoKind::None);
	};
}
