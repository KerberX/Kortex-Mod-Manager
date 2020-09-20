#include "stdafx.h"
#include "NetworkWxFSHandler.h"
#include "INetworkManager.h"
#include "Utility/Log.h"
#include <kxf::UI::Framework/KxFileStream.h>
#include <kxf::UI::Framework/KxCURL.h>
#include <kxf::UI::Framework/KxINet.h>

namespace Kortex::NetworkManager
{
	kxf::String NetworkWxFSHandler::GetCacheDirectory() const
	{
		return m_NetworkManager.GetCacheDirectory();
	}
	kxf::String NetworkWxFSHandler::ExtractFileName(const kxf::String& location) const
	{
		return KxINet::SplitURL(location).FileName;
	}
	kxf::String NetworkWxFSHandler::ConstructFullPath(const kxf::String& location) const
	{
		return GetCacheDirectory() + wxS('\\') + ExtractFileName(location);
	}
	wxFSFile* NetworkWxFSHandler::DoOpenFile(const kxf::String& location) const
	{
		Utility::Log::LogMessage("[NetworkWxFSHandler] Attempt to download resource: \"%1\"", location);
		
		KxFile file = GetCachedCopyFile(location);
		if (file.IsFileExist() && IsNewerThan(file.GetFileTime(KxFILETIME_MODIFICATION), wxTimeSpan::Days(7)))
		{
			kxf::String fullPath = ConstructFullPath(location);
			Utility::Log::LogMessage("[NetworkWxFSHandler] Using cached copy: \"%1\"", fullPath);

			auto stream = std::make_unique<KxFileStream>(fullPath, KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
			return new wxFSFile(stream.release(), fullPath, wxEmptyString, GetAnchor(location), wxFileName(fullPath).GetModificationTime());
		}
		else if (KxINet::IsInternetAvailable())
		{
			Utility::Log::LogMessage("[NetworkWxFSHandler] No cached copy or cached copy is too old, downloading");

			kxf::String fullPath = ConstructFullPath(location);
			auto stream = std::make_unique<KxFileStream>(fullPath, KxFileStream::Access::RW, KxFileStream::Disposition::CreateAlways, KxFileStream::Share::Read);
			KxCURLStreamReply reply(*stream);

			auto session = m_NetworkManager.NewCURLSession(location);
			session->SetTimeout(wxTimeSpan::Milliseconds(250));
			session->SetConnectionTimeout(wxTimeSpan::Milliseconds(250));

			session->Download(reply);
			if (reply.IsOK())
			{
				stream->Rewind();

				Utility::Log::LogMessage("[NetworkWxFSHandler] Resource downloaded to: \"%1\"", fullPath);
				return new wxFSFile(stream.release(), fullPath, wxEmptyString, GetAnchor(location), wxFileName(fullPath).GetModificationTime());
			}
		}

		// File is invalid, delete it.
		file.RemoveFile();

		Utility::Log::LogMessage("[NetworkWxFSHandler] Can not download resource: \"%1\"", location);
		return nullptr;
	}
	
	KxFile NetworkWxFSHandler::GetCachedCopyFile(const kxf::String& location) const
	{
		return ConstructFullPath(location);
	}
	bool NetworkWxFSHandler::IsNewerThan(const wxDateTime& fileDate, const wxTimeSpan& span) const
	{
		wxDateTime currentDate = wxDateTime::Now();
		if (fileDate > currentDate)
		{
			// File from the future? Return false to delete this file and download a new one.
			return false;
		}
		else
		{
			return currentDate - fileDate <= span;
		}
	}

	NetworkWxFSHandler::NetworkWxFSHandler(INetworkManager& networkManager)
		:m_NetworkManager(networkManager)
	{
		KxFile(GetCacheDirectory()).CreateFolder();
	}
	NetworkWxFSHandler::~NetworkWxFSHandler()
	{
	}

	bool NetworkWxFSHandler::CanOpen(const kxf::String& location)
	{
		// Base class doesn't seems to support HTTPS protocol
		return GetProtocol(location) == wxS("https") || wxInternetFSHandler::CanOpen(location);
	}
	kxf::String NetworkWxFSHandler::FindFirst(const kxf::String& wildcard, int flags)
	{
		// This don't seems to be called at all
		Utility::Log::LogInfo("[%1] Wildcards: %2, flags: %3", __FUNCTION__, wildcard, flags);
		return wxInternetFSHandler::FindFirst(wildcard, flags);
	}
	kxf::String NetworkWxFSHandler::FindNext()
	{
		// And this too
		Utility::Log::LogInfo("[%1]", __FUNCTION__);
		return wxInternetFSHandler::FindNext();
	}
	wxFSFile* NetworkWxFSHandler::OpenFile(wxFileSystem& fs, const kxf::String& location)
	{
		return DoOpenFile(location);
	}
}
