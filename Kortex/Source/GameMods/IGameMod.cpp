#include "stdafx.h"
#include "IGameMod.h"
#include "IModManager.h"
#include <Kortex/GameInstance.hpp>
#include "Utility/Common.h"
#include <kxf::UI::Framework/KxFile.h>
#include <kxf::UI::Framework/KxCrypto.h>

namespace
{
	kxf::String GetRootPath(const kxf::String& signature)
	{
		using namespace Kortex;

		if (const IGameInstance* instance = IGameInstance::GetActive())
		{
			return instance->GetModsDir() + wxS('\\') + signature;
		}
		return wxEmptyString;
	}
	kxf::String GetRootRelativePath(const kxf::String& signature, const kxf::String& name)
	{
		return GetRootPath(signature) + wxS('\\') + name;
	}
	kxf::String GetRootRelativePath(const kxf::String& signature, const wxChar* name)
	{
		return GetRootPath(signature) + wxS('\\') + name;
	}
}

namespace Kortex
{
	kxf::String IGameMod::GetSignatureFromID(const kxf::String& id)
	{
		auto utf8 = id.ToUTF8();
		wxMemoryInputStream stream(utf8.data(), utf8.length());
		return KxCrypto::MD5(stream);
	}
	
	bool IGameMod::LoadUsingID(const kxf::String& id)
	{
		SetID(id);
		return LoadUsingSignature(GetSignature());
	}
	bool IGameMod::CreateFromProject(const ModPackageProject& config)
	{
		return false;
	}
	
	kxf::String IGameMod::GetSafeName() const
	{
		return Utility::MakeSafeFileName(GetName());
	}
	bool IGameMod::IsPackageFileExist() const
	{
		return KxFile(GetPackageFile()).IsFileExist();
	}

	kxf::String IGameMod::GetRootDir() const
	{
		return GetRootPath(GetSignature());
	}
	kxf::String IGameMod::GetDescriptionFile() const
	{
		return GetRootRelativePath(GetSignature(), wxS("Description.txt"));
	}
	kxf::String IGameMod::GetInfoFile() const
	{
		return GetRootRelativePath(GetSignature(), wxS("Info.xml"));
	}
	kxf::String IGameMod::GetImageFile() const
	{
		return GetRootRelativePath(GetSignature(), wxS("Image.img"));
	}
	kxf::String IGameMod::GetDefaultModFilesDir() const
	{
		return GetRootRelativePath(GetSignature(), wxS("ModFiles"));
	}
	kxf::String IGameMod::GetModFilesDir() const
	{
		return GetDefaultModFilesDir();
	}
}
