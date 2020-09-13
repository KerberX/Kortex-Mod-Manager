#pragma once
#include <Kortex/Kortex.hpp>
#include "GameInstance/GameID.h"
#include "DownloadManager/DownloadItem.h"
#include "DownloadManager/DownloadEvent.h"
#include <KxFramework/KxURI.h>
#include <KxFramework/KxSingleton.h>
#include <KxFramework/KxComponentSystem.h>

namespace Kortex
{
	class IModNetwork;
	class ModNetworkRepository;
}

namespace Kortex
{
	namespace DownloadManager::Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	}

	class IDownloadManager:
		public ManagerWithTypeInfo<IManager, DownloadManager::Internal::TypeInfo>,
		public KxSingletonPtr<IDownloadManager>,
		public KxComponentContainer
	{
		public:
			enum class LocationStatus
			{
				Success = 0,
				NotExist,
				NotSpecified,
				InsufficientVolumeSpace,
				InsufficientVolumeCapabilities,
			};

		public:
			static kxf::String RenameIncrement(const kxf::String& name);

			static bool IsAssociatedWithLink(const kxf::String& type);
			static void AssociateWithLink(const kxf::String& type);

		protected:
			DownloadItem::Vector m_Downloads;
			
		private:
			int m_MaxConcurrentDownloads = -1;
			bool m_ShowHiddenDownloads = true;
			bool m_ShowArchivesOnly = false;

		protected:
			void OnInit() override;
			void OnExit() override;
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;
			
		protected:
			LocationStatus CheckDownloadLocation(const kxf::String& directoryPath, int64_t fileSize = -1) const;
			LocationStatus OnAccessDownloadLocation(int64_t fileSize = -1) const;

		public:
			IDownloadManager();
			virtual ~IDownloadManager();

		public:
			DownloadItem::RefVector GetDownloads() const;
			DownloadItem::RefVector GetInactiveDownloads(bool installedOnly = false) const;
			size_t GetActiveDownloadsCount() const;

			DownloadItem& AddDownload(std::unique_ptr<DownloadItem> download);
			bool RemoveDownload(DownloadItem& download);

			void LoadDownloads();
			void SaveDownloads();
			void PauseAllActive();

			bool ShouldShowHiddenDownloads() const
			{
				return m_ShowHiddenDownloads;
			}
			void ShowHiddenDownloads(bool show = true);

			bool ShouldShowArchivesOnly() const
			{
				return m_ShowArchivesOnly;
			}
			void SetShowArchivesOnly(bool show = true);

			kxf::String GetDownloadsLocation() const;
			void SetDownloadsLocation(const kxf::String& location);
			
			bool HasConcurrentDownloadsLimit() const
			{
				return m_MaxConcurrentDownloads >= 0;
			}
			int GetMaxConcurrentDownloads() const
			{
				return m_MaxConcurrentDownloads;
			}
			void SetMaxConcurrentDownloads(int count);

			DownloadItem* FindDownloadByFileName(const kxf::String& name, const DownloadItem* except = nullptr) const;
			bool AutoRenameIncrement(DownloadItem& item) const;

		public:
			virtual std::unique_ptr<IDownloadExecutor> NewDownloadExecutor(DownloadItem& item,
																		   const KxURI& uri,
																		   const kxf::String& localPath
			) = 0;
			virtual bool QueueDownload(ModNetworkRepository& modRepository,
									   const ModDownloadReply& downloadInfo,
									   const ModFileReply& fileInfo,
									   const GameID& id = {}
			) = 0;
			virtual bool QueueSimpleDownload(const KxURI& uri, const kxf::String& localPath = {}) = 0;
			virtual bool QueueUnknownDownload(const kxf::String& link);
	};
}
