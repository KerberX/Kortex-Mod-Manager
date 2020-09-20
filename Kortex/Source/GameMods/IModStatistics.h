#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex::ModStatistics
{
	enum class StatIndex
	{
		TotalMods,
		ActiveMods,
		InactiveMods,
		FilesCount,
		FoldersCount,
		FilesAndFoldersCount,
		UsedSpace,

		MAX,
		MIN = 0
	};

	class StatInfo
	{
		private:
			size_t m_Index = -1;

		public:
			StatInfo(size_t index)
				:m_Index(index)
			{
			}

		public:
			size_t GetIndex() const
			{
				return m_Index;
			}
	};
}

namespace Kortex
{
	class IModStatistics: public kxf::SingletonPtr<IModStatistics>
	{
		public:
			virtual size_t GetStatCount() const = 0;
			virtual bool HasStat(const ModStatistics::StatInfo& stat) const = 0;

			virtual kxf::String GetStatName(const ModStatistics::StatInfo& stat) const = 0;
			virtual kxf::String GetStatValue(const ModStatistics::StatInfo& stat) const = 0;
	};
}
