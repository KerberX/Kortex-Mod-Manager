#pragma once
#include <Kortex/Kortex.hpp>
#include "GameMods/IModTag.h"

namespace Kortex::ModTagManager
{
	class DefaultTag: public KxRTTI::ExtendInterface<DefaultTag, IModTag, INexusModTag>
	{
		KxRTTI_DeclareIID(DefaultTag, {0x4903903, 0x7cb8, 0x423a, {0x9b, 0x6d, 0x92, 0x5, 0xb6, 0x99, 0xe7, 0x9e}});

		private:
			kxf::String m_ID;
			kxf::String m_Name;
			KxColor m_Color;
			int m_NexusID = -1;
			bool m_IsExpanded = false;

		public:
			DefaultTag() = default;
			DefaultTag(const kxf::String& id, const kxf::String& name = wxEmptyString)
				:m_ID(id), m_Name(name)
			{
			}

		public:
			bool IsOK() const override
			{
				return !m_ID.IsEmpty();
			}
			std::unique_ptr<IModTag> Clone() const override
			{
				return std::make_unique<DefaultTag>(*this);
			}
			
			bool IsExpanded() const override
			{
				return m_IsExpanded;
			}
			void SetExpanded(bool isExpanded) override
			{
				m_IsExpanded = isExpanded;
			}

			kxf::String GetID() const override
			{
				return m_ID;
			}
			void SetID(const kxf::String& id) override
			{
				m_ID = id;
			}

			kxf::String GetName() const override;
			void SetName(const kxf::String& name) override;

			KxColor GetColor() const override
			{
				return m_Color;
			}
			void SetColor(const KxColor& color) override
			{
				m_Color = color;
			}

			int GetNexusID() const override
			{
				return m_NexusID;
			}
			void SetNexusID(int value) override
			{
				m_NexusID = value;
			}
	};
}
