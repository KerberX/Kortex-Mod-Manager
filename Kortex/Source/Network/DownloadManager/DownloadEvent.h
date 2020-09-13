#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/BroadcastProcessor.h"

namespace Kortex
{
	class DownloadItem;
}

namespace Kortex
{
	class DownloadEvent: public BroadcastEvent
	{
		public:
			KxEVENT_MEMBER(DownloadEvent, Added);
			KxEVENT_MEMBER(DownloadEvent, Removed);
			KxEVENT_MEMBER(DownloadEvent, Progress);

			KxEVENT_MEMBER(DownloadEvent, Started);
			KxEVENT_MEMBER(DownloadEvent, Stopped);
			KxEVENT_MEMBER(DownloadEvent, Paused);
			KxEVENT_MEMBER(DownloadEvent, Resumed);
			KxEVENT_MEMBER(DownloadEvent, Completed);
			KxEVENT_MEMBER(DownloadEvent, Failed);

			KxEVENT_MEMBER(DownloadEvent, RefreshItems);
			KxEVENT_MEMBER(DownloadEvent, ConcurrentDownloadsCountChanged);

		private:
			DownloadItem* m_Download = nullptr;

		public:
			DownloadEvent() = default;
			DownloadEvent(DownloadItem& item)
				:m_Download(&item)
			{
			}

		public:
			DownloadEvent* Clone() const override
			{
				return new DownloadEvent(*this);
			}
			
			DownloadItem& GetDownload() const
			{
				return *m_Download;
			}
			bool ShouldShowHidden() const;
			int GetMaxConcurrentDownloads() const;
	};
}
