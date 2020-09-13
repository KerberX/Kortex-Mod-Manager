#pragma once
#include <Kortex/Kortex.hpp>
#include "BaseGamePlugin.h"
#include "IBethesdaGamePlugin.h"
#include "IBethesdaPluginReader.h"

namespace Kortex::PluginManager
{
	class IPluginReader;

	class BethesdaPlugin: public KxRTTI::ImplementInterface<BethesdaPlugin, BaseGamePlugin, IBethesdaGamePlugin>
	{
		public:
			using HeaderFlags = BethesdaPluginData::HeaderFlags;

		protected:
			void OnRead(IPluginReader& reader) override;

			void SetLight(bool value)
			{
				KxUtility::ModFlagRef(m_Data.m_HeaderFlags, HeaderFlags::Light, value);
			}
			void SetMaster(bool value)
			{
				KxUtility::ModFlagRef(m_Data.m_HeaderFlags, HeaderFlags::Master, value);
			}

		private:
			BethesdaPluginData m_Data;

		public:
			BethesdaPlugin() = default;
			BethesdaPlugin(const kxf::String& fullPath)
			{
				BaseGamePlugin::Create(fullPath);
			}

		public:
			bool IsOK() const override
			{
				return BaseGamePlugin::IsOK();
			}
			
			bool IsLocalized() const override
			{
				ReadDataIfNeeded();
				return m_Data.m_HeaderFlags & HeaderFlags::Localized;
			}
			bool IsMaster() const override
			{
				ReadDataIfNeeded();
				return m_Data.m_HeaderFlags & HeaderFlags::Master;
			}
			bool IsLight() const override
			{
				ReadDataIfNeeded();
				return m_Data.m_HeaderFlags & HeaderFlags::Light;
			}
			uint32_t GetFormVersion() const override
			{
				ReadDataIfNeeded();
				return m_Data.m_FormVersion;
			}
			
			KxStringVector GetRequiredPlugins() const override
			{
				ReadDataIfNeeded();
				return m_Data.m_RequiredPlugins;
			}
			kxf::String GetAuthor() const override
			{
				ReadDataIfNeeded();
				return m_Data.m_Author;
			}
			kxf::String GetDescription() const override
			{
				ReadDataIfNeeded();
				return m_Data.m_Description;
			}
	};
}
