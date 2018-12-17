#pragma once
#include "stdafx.h"
#include "Network/IDownloadEntry.h"
#include "Network/IDownloadManager.h"

namespace Kortex::DownloadManager
{
	class DefaultDownloadManager: public RTTI::IImplementation<Kortex::IDownloadManager, IDownloadManagerNXM>
	{
		private:
			IDownloadEntry::Vector m_Downloads;
			bool m_IsAssociatedWithNXM = false;
			bool m_IsReady = false;

		private:
			KWorkspace* CreateWorkspace(KMainWindow* mainWindow) override;
			void SetReady(bool value = true)
			{
				m_IsReady = true;
			}

			void OnChangeEntry(const IDownloadEntry& entry, bool noSave = false) const override;
			void OnAddEntry(const IDownloadEntry& entry) const override;
			void OnRemoveEntry(const IDownloadEntry& entry) const override;

			void OnDownloadComplete(IDownloadEntry& entry) override;
			void OnDownloadPaused(IDownloadEntry& entry) override;
			void OnDownloadStopped(IDownloadEntry& entry) override;
			void OnDownloadResumed(IDownloadEntry& entry) override;
			void OnDownloadFailed(IDownloadEntry& entry) override;

		protected:
			virtual void OnInit() override;
			virtual void OnExit() override;
			virtual void OnLoadInstance(IGameInstance& instance, const KxXMLNode& managerNode);

		public:
			void LoadDownloads() override;
			void SaveDownloads() override;

			bool ShouldShowHiddenDownloads() const override
			{
				//return m_Options.GetAttributeBool("ShowHiddenDownloads");
				return false;
			}
			void ShowHiddenDownloads(bool show = true) override
			{
				//m_Options.SetAttribute("ShowHiddenDownloads", show);
			}

			wxString GetDownloadsLocation() const override;
			void SetDownloadsLocation(const wxString& location) override;

			const IDownloadEntry::Vector& GetDownloads() const override
			{
				return m_Downloads;
			}
			IDownloadEntry::Vector& GetDownloads() override
			{
				return m_Downloads;
			}

			IDownloadEntry& NewDownload() override;
			bool RemoveDownload(IDownloadEntry& download) override;
			bool QueueDownload(const Network::DownloadInfo& downloadInfo,
							   const Network::FileInfo& fileInfo,
							   const INetworkProvider* provider,
							   const GameID& id = GameIDs::NullGameID
			) override;
			bool QueueFromOutside(const wxString& link) override;

		// IDownloadManagerNXM
		private:
			bool CheckIsAssociatedWithNXM() const;

		public:
			bool IsAssociatedWithNXM() const override;
			void AssociateWithNXM() override;

			GameID TranslateGameID(const wxString& nexusID) const override;
			bool QueueNXM(const wxString& link) override;
	};
}