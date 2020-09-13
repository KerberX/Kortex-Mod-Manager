#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/BroadcastProcessor.h"

namespace Kortex
{
	class IGameMod;
}

namespace Kortex
{
	class ModEvent: public BroadcastEvent
	{
		public:
			using RefVector = std::vector<IGameMod*>;

		public:
			KxEVENT_MEMBER(ModEvent, Installing);
			KxEVENT_MEMBER(ModEvent, Installed);

			KxEVENT_MEMBER(ModEvent, Unsinstalling);
			KxEVENT_MEMBER(ModEvent, Uninstalled);

			KxEVENT_MEMBER(ModEvent, Toggled);
			KxEVENT_MEMBER(ModEvent, Changed);
			KxEVENT_MEMBER(ModEvent, FilesChanged);

			KxEVENT_MEMBER(ModEvent, Reordered);
			KxEVENT_MEMBER(ModEvent, VirtualTreeInvalidated);

			KxEVENT_MEMBER(ModEvent, BeginReload);
			KxEVENT_MEMBER(ModEvent, EndReload);

		private:
			RefVector m_ModVector;
			IGameMod* m_Mod = nullptr;

		public:
			ModEvent() = default;
			ModEvent(IGameMod& mod)
				:m_Mod(&mod)
			{
			}
			ModEvent(RefVector mods)
				:m_ModVector(mods)
			{
			}
			ModEvent(const kxf::String& id)
			{
				wxNotifyEvent::SetString(id);
			}

		public:
			ModEvent* Clone() const override
			{
				return new ModEvent(*this);
			}

			bool HasMod() const
			{
				return m_Mod != nullptr;
			}
			IGameMod* GetMod() const
			{
				return m_Mod;
			}
			kxf::String GetModID() const;

			bool HasModArray() const
			{
				return !m_ModVector.empty();
			}
			const RefVector& GetModsArray() const
			{
				return m_ModVector;
			}
	};
}
