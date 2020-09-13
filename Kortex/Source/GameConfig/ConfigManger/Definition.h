#pragma once
#include <Kortex/Kortex.hpp>
#include "DataType.h"
#include "ITypeDetector.h"
#include "ItemOptions.h"
#include "ItemGroup.h"

namespace Kortex
{
	class IConfigManager;
}

namespace Kortex::GameConfig
{
	class Definition
	{
		private:
			IConfigManager& m_Manager;

			kxf::String m_ID;
			kxf::String m_FilePath;
			std::unordered_map<TypeID, DataType, TypeID::Hash> m_DataTypes;
			std::vector<std::unique_ptr<ITypeDetector>> m_TypeDetectors;

			std::unordered_map<kxf::String, std::unique_ptr<ItemGroup>> m_Groups;
			ItemOptions m_Options;
			bool m_IsLoaded = false;

		private:
			void LoadGroups(const kxf::XMLNode& groupsNode);
			template<class TItems, class TFunctor> static void VectorForEach(TItems&& items, TFunctor&& func)
			{
				for (auto& item: items)
				{
					func(*item);
				}
			}
			template<class TItems, class TFunctor> static void MapForEach(TItems&& items, TFunctor&& func)
			{
				for (auto& [id, item]: items)
				{
					func(*item);
				}
			}

		public:
			Definition(IConfigManager& manager, const kxf::String& id, const kxf::String& filePath)
				:m_Manager(manager), m_ID(id), m_FilePath(filePath)
			{
			}
			virtual ~Definition();

		public:
			IConfigManager& GetManager() const
			{
				return m_Manager;
			}
			kxf::String GetID() const
			{
				return m_ID;
			}
			kxf::String GetFilePath() const
			{
				return m_FilePath;
			}
			const ItemOptions& GetOptions() const
			{
				return m_Options;
			}
			DataType GetDataType(TypeID id) const;

			bool Load();
			void RemoveInvalidGroups();

			ItemGroup* GetGroupByID(const kxf::String& id)
			{
				auto it = m_Groups.find(id);
				return it != m_Groups.end() ? it->second.get() : nullptr;
			}
			template<class TFunctor> void ForEachGroup(TFunctor&& func) const
			{
				MapForEach(m_Groups, func);
			}
			template<class TFunctor> void ForEachGroup(TFunctor&& func)
			{
				MapForEach(m_Groups, func);
			}
			template<class TFunctor> void ForEachTypeDetector(TFunctor&& func) const
			{
				VectorForEach(m_TypeDetectors, func);
			}
	};
}
