#pragma once
#include <Kortex/Kortex.hpp>
#include "GameData/IGamePlugin.h"
#include <KxFramework/KxFileItem.h>

namespace Kortex::PluginManager
{
	class BaseGamePlugin: public IGamePlugin
	{
		private:
			KxFileItem m_FileItem;
			mutable const StdContentItem* m_StdContent = nullptr;
			bool m_IsActive = false;

		protected:
			void Create(const kxf::String& fullPath)
			{
				m_FileItem.SetFullPath(fullPath);
				m_FileItem.UpdateInfo();
			}

		public:
			BaseGamePlugin() = default;
			BaseGamePlugin(const kxf::String& fullPath)
				:m_FileItem(fullPath)
			{
			}

		public:
			bool IsOK() const override
			{
				return m_FileItem.IsOK();
			}

			kxf::String GetName() const override
			{
				return m_FileItem.GetName();
			}
			kxf::String GetFullPath() const override
			{
				return m_FileItem.GetFullPath();
			}

			bool IsActive() const override
			{
				return m_IsActive;
			}
			void SetActive(bool isActive) override
			{
				m_IsActive = isActive;
			}
			
			const StdContentItem* GetStdContentEntry() const override;
	};
}
