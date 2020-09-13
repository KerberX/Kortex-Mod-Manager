#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/BroadcastProcessor.h"

namespace Kortex
{
	class IGameSave;
}

namespace Kortex
{
	class SaveEvent: public BroadcastEvent
	{
		public:
			using RefVector = std::vector<IGameSave*>;

		public:
			KxEVENT_MEMBER(SaveEvent, Removing);
			KxEVENT_MEMBER(SaveEvent, Removed);
			KxEVENT_MEMBER(SaveEvent, Changed);
			KxEVENT_MEMBER(SaveEvent, FiltersChanged);

		private:
			RefVector m_SavesArray;
			IGameSave* m_Save = nullptr;

		public:
			SaveEvent() = default;
			SaveEvent(IGameSave& save)
				:m_Save(&save)
			{
			}
			SaveEvent(RefVector saves)
				:m_SavesArray(saves)
			{
			}
			SaveEvent(const kxf::String& name)
			{
				SetString(name);
			}

			SaveEvent* Clone() const override
			{
				return new SaveEvent(*this);
			}

		public:
			bool HasSave() const
			{
				return m_Save != nullptr;
			}
			IGameSave* GetSave() const
			{
				return m_Save;
			}
			kxf::String GetSaveName() const;

			bool HasSavesArray() const
			{
				return !m_SavesArray.empty();
			}
			const RefVector& GetSavesArray() const
			{
				return m_SavesArray;
			}
	};
}
