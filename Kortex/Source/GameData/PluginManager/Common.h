#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IManager.h"

namespace Kortex::PluginManager
{
	class StdContentItem final
	{
		public:
			using Vector = std::vector<StdContentItem>;

		private:
			kxf::String m_ID;
			kxf::String m_Name;
			kxf::String m_Logo;

		public:
			StdContentItem(const kxf::XMLNode& node);

		public:
			kxf::String GetID() const;
			kxf::String GetName() const;
			kxf::String GetLogo() const;
			kxf::String GetLogoFullPath() const;
	};
	class SortingToolItem final
	{
		public:
			using Vector = std::vector<SortingToolItem>;

		private:
			kxf::String m_ID;
			kxf::String m_Name;
			kxf::String m_Command;
			mutable kxf::String m_Executable;

		public:
			SortingToolItem(const kxf::XMLNode& node);

		public:
			kxf::String GetID() const;
			kxf::String GetName() const;
			
			kxf::String GetExecutable() const;
			void SetExecutable(const kxf::String& path) const;
			
			kxf::String GetArguments() const;
	};
	class Config final
	{
		private:
			kxf::String m_Implementation;
			kxf::String m_PluginImplementation;

			int m_PluginLimit = -1;
			kxf::String m_StdandardContent_MainID;
			StdContentItem::Vector m_StandardContent;
			SortingToolItem::Vector m_SortingTools;

		public:
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node);

		public:
			const kxf::String& GetManagerImplementation() const
			{
				return m_Implementation;
			}
			const kxf::String& GetPluginImplementation() const
			{
				return m_PluginImplementation;
			}

			bool HasPluginLimit() const
			{
				return m_PluginLimit > 0;
			}
			int GetPluginLimit() const
			{
				return m_PluginLimit;
			}

			bool HasMainStdContentID() const;
			kxf::String GetMainStdContentID() const;

			const StdContentItem* GetStandardContent(const kxf::String& id) const;
			bool IsStandardContent(const kxf::String& id) const
			{
				return GetStandardContent(id) != nullptr;
			}

			bool HasSortingTools() const
			{
				return !m_SortingTools.empty();
			}
			const SortingToolItem::Vector& GetSortingTools() const
			{
				return m_SortingTools;
			}
	};
}
