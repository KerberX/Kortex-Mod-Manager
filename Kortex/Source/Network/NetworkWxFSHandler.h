#pragma once
#include <Kortex/Kortex.hpp>
#include <KxFramework/KxFile.h>

namespace Kortex
{
	class INetworkManager;
}

namespace Kortex::NetworkManager
{
	class NetworkWxFSHandler: public wxInternetFSHandler
	{
		private:
			INetworkManager& m_NetworkManager;

		private:
			kxf::String GetCacheDirectory() const;
			kxf::String ExtractFileName(const kxf::String& location) const;
			kxf::String ConstructFullPath(const kxf::String& location) const;
			wxFSFile* DoOpenFile(const kxf::String& location) const;

			KxFile GetCachedCopyFile(const kxf::String& location) const;
			bool IsNewerThan(const wxDateTime& fileDate, const wxTimeSpan& span) const;

		public:
			NetworkWxFSHandler(INetworkManager& networkManager);
			virtual ~NetworkWxFSHandler();

		public:
			bool CanOpen(const kxf::String& location) override;
			kxf::String FindFirst(const kxf::String& wildcard, int flags) override;
			kxf::String FindNext() override;
			wxFSFile* OpenFile(wxFileSystem& fs, const kxf::String& location) override;
	};
}