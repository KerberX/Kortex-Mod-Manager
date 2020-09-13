#pragma once
#include <Kortex/Kortex.hpp>
#include "Common.h"
#include "DataType.h"
#include "ItemValue.h"
#include "ItemOptions.h"
#include "ItemSamples.h"
#include "IViewItem.h"
#include "ISource.h"
#include "IAction.h"
#include "GameConfig/IConfigManager.h"
#include <KxFramework/DataView2/Node.h>
#include <KxFramework/DataView2/DataView2Fwd.h>

namespace Kortex
{
	class IConfigManager;
}

namespace Kortex::GameConfig
{
	class Item;

	class HashStore
	{
		private:
			mutable std::optional<size_t> m_Hash;

		public:
			bool HasHash() const
			{
				return m_Hash.has_value();
			}
			size_t Get(const Item& item, const kxf::String& value = {}) const;
	};
}

namespace Kortex::GameConfig
{
	class ItemGroup;
	class Definition;

	class Item: public KxRTTI::ExtendInterface<Item, IViewItem, KxDataView2::Node>
	{
		KxRTTI_DeclareIID(Item, {0xb2b4382c, 0x2bf1, 0x4d4a, {0x91, 0xf6, 0x26, 0xd0, 0x83, 0xf4, 0xfc, 0x30}});

		friend class ItemGroup;

		private:
			ItemGroup& m_Group;
			kxf::String m_Category;
			kxf::String m_Path;
			kxf::String m_Name;
			kxf::String m_Label;
			ItemSamples m_Samples;
			ItemOptions m_Options;
			HashStore m_HashStore;
			TypeID m_TypeID;

			kxf::String m_ActionName;
			IntrinsicActionValue m_IntrinsicAction;

			bool m_HasChanges = false;
			mutable std::optional<kxf::String> m_DisplayPath;

		protected:
			virtual void Clear() = 0;
			virtual void Read(const ISource& source) = 0;
			virtual void Write(ISource& source) const = 0;
			virtual void ChangeNotify();

			void RegisterAsKnown();
			void UnregisterAsKnown();

		public:
			Item(ItemGroup& group, const kxf::XMLNode& itemNode = {});
			virtual ~Item();

		public:
			virtual bool Create(const kxf::XMLNode& itemNode = {}) = 0;
			virtual bool IsOK() const;
			virtual bool IsUnknown() const = 0;
			virtual kxf::String GetFullPath() const;
			size_t GetHash() const
			{
				return m_HashStore.Get(*this);
			}
			
			bool HasChanges() const
			{
				return m_HasChanges;
			}
			void SaveChanges();
			void DiscardChanges();
			void DeleteValue();

			IConfigManager& GetManager() const;
			Definition& GetDefinition() const;
			ItemGroup& GetGroup() const
			{
				return m_Group;
			}
			wxWindow* GetInvokingWindow() const;
			wxWindow* GetInvokingTopLevelWindow() const;

			kxf::String GetCategory() const
			{
				return m_Category;
			}
			void SetCategory(const kxf::String& category)
			{
				m_Category = category;
			}
			
			virtual kxf::String GetPath() const
			{
				return m_Path;
			}
			void SetPath(const kxf::String& path)
			{
				m_Path = path;
			}

			kxf::String GetName() const
			{
				return m_Name;
			}
			void SetName(const kxf::String& name)
			{
				m_Name = name;
			}
			
			kxf::String GetLabel() const
			{
				return m_Label.IsEmpty() ? m_Name : m_Label;
			}
			void SetLabel(const kxf::String& label)
			{
				m_Label = label;
			}

			bool HasSamples() const
			{
				return !m_Samples.IsEmpty();
			}
			const ItemSamples& GetSamples() const
			{
				return m_Samples;
			}
			ItemSamples& GetSamples()
			{
				return m_Samples;
			}

			const ItemOptions& GetOptions() const
			{
				return m_Options;
			}
			ItemOptions& GetOptions()
			{
				return m_Options;
			}

			TypeID GetTypeID() const
			{
				return m_TypeID;
			}
			void SetTypeID(TypeID id)
			{
				m_TypeID = id;
			}
			DataType GetDataType() const;

			bool HasAction() const
			{
				return !m_IntrinsicAction.IsDefault() || !m_ActionName.IsEmpty();
			}
			kxf::String GetActionName() const
			{
				return m_ActionName;
			}
			IntrinsicActionValue GetIntrinsicAction() const
			{
				return m_IntrinsicAction;
			}

			virtual bool IsEditable() const;

		public:
			void OnActivate(KxDataView2::Column& column) override;
			kxf::String GetViewString(ColumnID id) const override;

			wxAny GetValue(const KxDataView2::Column& column) const override;
			bool Compare(const KxDataView2::Node& node, const KxDataView2::Column& column) const override;
	};
}
