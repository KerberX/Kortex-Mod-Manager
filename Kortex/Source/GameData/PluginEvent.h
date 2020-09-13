#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/BroadcastProcessor.h"

namespace Kortex
{
	class IGamePlugin;
}

namespace Kortex
{
	class PluginEvent: public BroadcastEvent
	{
		public:
			using RefVector = std::vector<IGamePlugin*>;

		public:
			KxEVENT_MEMBER(PluginEvent, Toggled);
			KxEVENT_MEMBER(PluginEvent, Changed);
			KxEVENT_MEMBER(PluginEvent, Reordered);

		private:
			RefVector m_PluginsVector;
			IGamePlugin* m_Plugin = nullptr;

		public:
			PluginEvent() = default;
			PluginEvent(IGamePlugin& plugin)
				:m_Plugin(&plugin)
			{
			}
			PluginEvent(RefVector plugins)
				:m_PluginsVector(plugins)
			{
			}
			PluginEvent(const kxf::String& name)
			{
				SetString(name);
			}

			PluginEvent* Clone() const override
			{
				return new PluginEvent(*this);
			}

		public:
			bool HasPlugin() const
			{
				return m_Plugin != nullptr;
			}
			IGamePlugin* GetPlugin() const
			{
				return m_Plugin;
			}
			kxf::String GetPluginName() const;

			bool HasPluginsArray() const
			{
				return !m_PluginsVector.empty();
			}
			const RefVector& GetPluginsArray() const
			{
				return m_PluginsVector;
			}

	};
}
