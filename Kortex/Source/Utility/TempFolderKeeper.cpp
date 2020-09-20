#include "stdafx.h"
#include "TempFolderKeeper.h"
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxMath.h>

namespace
{
	static kxf::String g_GlobalTemp;
}

namespace Kortex::Utility
{
	ScopedTempFile::~ScopedTempFile()
	{
		KxFile(m_FilePath).RemoveFile();
	}
}

namespace Kortex::Utility
{
	void TempFolderKeeper::InitGlobalTemp()
	{
		if (g_GlobalTemp.IsEmpty())
		{
			g_GlobalTemp = wxFileName::GetTempDir() + wxS('\\') + IApplication::GetInstance()->GetName() + '\\';
			KxFile(g_GlobalTemp).CreateFolder();
		}
	}
	const kxf::String& TempFolderKeeper::GetGlobalTemp()
	{
		InitGlobalTemp();
		return g_GlobalTemp;
	}
	kxf::String TempFolderKeeper::DoCreateTempFile(const kxf::String& folder)
	{
		return wxFileName::CreateTempFileName(folder);
	}
	kxf::String TempFolderKeeper::DoCreateTempFile(const kxf::String& folder, const kxf::String& suffix)
	{
		if (suffix.IsEmpty())
		{
			return DoCreateTempFile(folder);
		}
		else
		{
			kxf::String tempFile = DoCreateTempFile(folder);
			kxf::String newTempFile = tempFile + suffix;
			KxFile(tempFile).Rename(newTempFile, true);
			return newTempFile;
		}
	}

	kxf::String TempFolderKeeper::CreateGlobalTempFile(const kxf::String& suffix)
	{
		return DoCreateTempFile(g_GlobalTemp, suffix);
	}
	ScopedTempFile TempFolderKeeper::CreateScopedGlobalTempFile(const kxf::String& suffix)
	{
		return ScopedTempFile(DoCreateTempFile(g_GlobalTemp, suffix));
	}

	kxf::String TempFolderKeeper::InitTempFolder() const
	{
		const void* pInstance = this + KxMath::RandomInt(std::numeric_limits<size_t>::max());

		// Global temp folder already has path separator at its end.
		return kxf::String::Format("%s0x%p\\", GetGlobalTemp(), pInstance);
	}

	TempFolderKeeper::TempFolderKeeper()
		:m_TempFolder(InitTempFolder())
	{
		KxFile(m_TempFolder).CreateFolder();
	}
	TempFolderKeeper::~TempFolderKeeper()
	{
		KxFile(m_TempFolder).RemoveFolderTree(true);
	}

	kxf::String TempFolderKeeper::CreateTempFile(const kxf::String& suffix) const
	{
		return DoCreateTempFile(m_TempFolder, suffix);
	}
	ScopedTempFile TempFolderKeeper::CreateScopedTempFile(const kxf::String& suffix)
	{
		return CreateTempFile(suffix);
	}
}
