#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IManager.h"
#include "GameMods/IGameMod.h"
#include "VirtualFileSystem/IVirtualFileSystem.h"
#include "ModManager/Common.h"
#include "Network/Common.h"
#include "Network/NetworkModInfo.h"
#include <kxf::UI::Framework/KxSingleton.h>

namespace Kortex
{
	class IGameProfile;
	class IModNetwork;
}
namespace Kortex::ModManager
{
	class Config;
	namespace Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	}
}

namespace Kortex
{
	class IModManager:
		public KxRTTI::ExtendInterface<IModManager, ManagerWithTypeInfo<IManager, ModManager::Internal::TypeInfo>>,
		public kxf::SingletonPtr<IModManager>
	{
		KxRTTI_DeclareIID(IModManager, {0xec61c172, 0xa750, 0x4be9, {0xb0, 0x9d, 0x27, 0xe8, 0x8c, 0x49, 0x36, 0xb2}});

		public:
			using GetModsFlags = ModManager::GetModsFlags;

		protected:
			IGameMod::Vector m_Mods;

		protected:
			void RecalculatePriority(size_t startAt = 0);
			void SortByPriority();

		protected:
			IModManager();

		public:
			virtual const ModManager::Config& GetOptions() const = 0;

			virtual std::unique_ptr<IGameMod> NewMod() = 0;
			IGameMod& EmplaceMod()
			{
				return EmplaceMod(NewMod());
			}
			IGameMod& EmplaceMod(std::unique_ptr<IGameMod> mod)
			{
				mod->SetPriority(m_Mods.size());
				return *m_Mods.emplace_back(std::move(mod));
			}

			virtual void Load() = 0;
			virtual void Save() const = 0;

			virtual IGameMod::RefVector GetMods(ModManager::GetModsFlags flags = ModManager::GetModsFlags::None) = 0;
			virtual size_t GetModsCount(ModManager::GetModsFlags flags = ModManager::GetModsFlags::None) = 0;
			virtual IGameMod::RefVector GetMandatoryMods() = 0;

			virtual IGameMod& GetBaseGame() = 0;
			virtual IGameMod& GetWriteTarget() = 0;

			void ResortMods();
			void ResortMods(const IGameProfile& profile);

			bool MoveModsBefore(const IGameMod::RefVector& movedMods, const IGameMod& anchor);
			bool MoveModsAfter(const IGameMod::RefVector& movedMods, const IGameMod& anchor);
			bool ChangeModPriority(IGameMod& movedMod, intptr_t targetPriority);

			virtual IGameMod* FindModByID(const kxf::String& modID) const = 0;
			virtual IGameMod* FindModByName(const kxf::String& modName) const = 0;
			virtual IGameMod* FindModBySignature(const kxf::String& signature) const = 0;
			virtual IGameMod* FindModByModNetwork(const kxf::String& sourceName, NetworkModInfo modInfo) const = 0;
			virtual IGameMod* FindModByModNetwork(const IModNetwork& modNetwork, NetworkModInfo modInfo) const = 0;
			
			virtual bool IsModActive(const kxf::String& modID) const = 0;
			virtual bool ChangeModID(IGameMod& mod, const kxf::String& newID) = 0;
			virtual void ExportModList(const kxf::String& outputFilePath) const = 0;

			virtual void InstallEmptyMod(const kxf::String& name) = 0;
			virtual void InstallModFromFolder(const kxf::String& sourcePath, const kxf::String& name, bool linkLocation = false) = 0;
			virtual void InstallModFromPackage(const kxf::String& packagePath) = 0;
			virtual void UninstallMod(IGameMod& mod) = 0;
			virtual void EraseMod(IGameMod& mod) = 0;

		public:
			virtual IVirtualFileSystem& GetFileSystem() = 0;
	};
}
