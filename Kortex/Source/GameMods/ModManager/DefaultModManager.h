#pragma once
#include <Kortex/Kortex.hpp>
#include "GameMods/IModManager.h"
#include "GameMods/IGameMod.h"
#include "GameMods/ModEvent.h"
#include "Application/IWorkspace.h"
#include "FixedGameMod.h"
#include "MainFileSystem.h"
#include "MandatoryModEntry.h"
#include <Kx/Threading.hpp>

namespace Kortex
{
	class ProfileEvent;
	class IGameProfile;
}

namespace Kortex::ModManager
{
	class MirroredLocation
	{
		friend class Config;

		public:
			using Vector = std::vector<MirroredLocation>;

		private:
			KxStringVector m_Sources;
			kxf::String m_Target;

		private:
			bool IsOK() const
			{
				return !m_Sources.empty() && !m_Target.IsEmpty();
			}

		public:
			MirroredLocation(const kxf::XMLNode& parentNode);

		public:
			bool ShouldUseMultiMirror() const
			{
				return m_Sources.size() > 1;
			}

			KxStringVector GetSources() const;
			kxf::String GetSource() const;
			kxf::String GetTarget() const;
	};
	class MandatoryLocation
	{
		public:
			using Vector = std::vector<MandatoryLocation>;

		private:
			kxf::String m_Source;
			kxf::String m_Name;

		public:
			MandatoryLocation(const kxf::XMLNode& parentNode);

		public:
			bool IsOK() const
			{
				return !m_Source.IsEmpty();
			}

			kxf::String GetSource() const;
			kxf::String GetName() const;
	};

	class Config
	{
		private:
			MirroredLocation::Vector m_MirroredLocations;
			MandatoryLocation::Vector m_MandatoryLocations;

		public:
			void OnLoadInstance(IGameInstance& profile, const kxf::XMLNode& node);

		public:
			const MirroredLocation::Vector& GetMirroredLocations() const
			{
				return m_MirroredLocations;
			}
			const MandatoryLocation::Vector& GetMandatoryLocations() const
			{
				return m_MandatoryLocations;
			}
	};
}

namespace Kortex::ModManager
{
	class DefaultModManager: public IModManager
	{
		friend class MainFileSystem;

		private:
			BroadcastReciever m_BroadcastReciever;
			Config m_Config;
			MainFileSystem m_FileSystem;

			FixedGameMod m_BaseGame;
			FixedGameMod m_WriteTarget;
			std::vector<KMandatoryModEntry> m_MandatoryMods;

			std::unique_ptr<KxThread> m_ModsLoaderThread;
			IGameProfile* m_SortingProfile = nullptr;
			bool m_InitialLoadMods = false;

		private:
			IGameMod* DoCreateMod(const kxf::String& signature);
			void ProcessInstallMod(IGameMod& mod);
			
			void DoUninstallMod(IGameMod& mod, const bool erase);
			void ProcessEraseMod(IGameMod& mod);
			void ProcessUninstallMod(IGameMod& mod);

			void OnUpdateModLayoutNeeded(ModEvent& event);
			void OnModLayoutSaveNeeded(ModEvent& event);
			void OnProfileSelected(ProfileEvent& event);

		protected:
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;
			void OnInit() override;
			void OnExit() override;
			void CreateWorkspaces() override;

		public:
			DefaultModManager();

		public:
			IWorkspace::RefVector EnumWorkspaces() const override;
			void Load() override;
			void Save() const override;

			const ModManager::Config& GetOptions() const override
			{
				return m_Config;
			}

			std::unique_ptr<IGameMod> NewMod() override;
			IGameMod::RefVector GetMods(GetModsFlags flags = GetModsFlags::None) override;
			size_t GetModsCount(ModManager::GetModsFlags flags = ModManager::GetModsFlags::None) override;
			IGameMod::RefVector GetMandatoryMods() override;

			IGameMod& GetBaseGame() override
			{
				return m_BaseGame;
			}
			IGameMod& GetWriteTarget() override
			{
				return m_WriteTarget;
			}

			IGameMod* FindModByID(const kxf::String& modID) const override;
			IGameMod* FindModByName(const kxf::String& modName) const override;
			IGameMod* FindModBySignature(const kxf::String& signature) const override;
			IGameMod* FindModByModNetwork(const kxf::String& modNetworkName, NetworkModInfo modInfo) const override;
			IGameMod* FindModByModNetwork(const IModNetwork& modNetwork, NetworkModInfo modInfo) const override;
			
			bool IsModActive(const kxf::String& modID) const override;
			bool ChangeModID(IGameMod& mod, const kxf::String& newID) override;
			void ExportModList(const kxf::String& outputFilePath) const override;

			void InstallEmptyMod(const kxf::String& name) override;
			void InstallModFromFolder(const kxf::String& sourcePath, const kxf::String& name, bool linkLocation = false) override;
			void InstallModFromPackage(const kxf::String& packagePath) override;
			void UninstallMod(IGameMod& mod) override
			{
				DoUninstallMod(mod, false);
			}
			void EraseMod(IGameMod& mod) override
			{
				DoUninstallMod(mod, true);
			}
			
		public:
			IVirtualFileSystem& GetFileSystem() override
			{
				return m_FileSystem;
			}
	};
}
