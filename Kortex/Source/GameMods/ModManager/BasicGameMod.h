#pragma once
#include <Kortex/Kortex.hpp>
#include "GameMods/IGameMod.h"
#include "GameMods/FileTreeNode.h"
#include "GameMods/ModTagStore.h"
#include "Network/Common.h"
#include "Network/ModSourceStore.h"
#include <KxFramework/kxf::Version.h>

namespace Kortex
{
	class IModManager;
	class IModDispatcher;
}

namespace Kortex::ModManager
{
	class FixedGameMod;
	class PriorityGroup;

	class BasicGameMod: public KxRTTI::ExtendInterface<BasicGameMod, IGameMod>
	{
		KxRTTI_DeclareIID(BasicGameMod, {0x29e82e8c, 0xb63b, 0x4955, {0xba, 0xd9, 0xa7, 0x69, 0xc5, 0xae, 0x74, 0xbb}});

		private:
			kxf::String m_Signature;
			kxf::String m_ID;
			kxf::String m_Name;
			kxf::String m_Author;
			kxf::Version m_Version;
			intptr_t m_Priority;

			bool m_IsDescriptionChanged = false;
			mutable kxf::String m_Description;

			ModSourceStore m_ModSourceStore;
			ModTagStore m_TagStore;

			wxDateTime m_TimeInstall;
			wxDateTime m_TimeUninstall;
			kxf::String m_PackageFile;
			kxf::String m_LinkLocation;

			FileTreeNode m_FileTree;

			KxColor m_Color;
			bool m_IsActive = false;

		protected:
			bool IsInstalledReal() const;

		public:
			bool IsOK() const override;

			void CreateAllFolders();
			bool Save() override;

			bool LoadUsingSignature(const kxf::String& signature) override;
			bool LoadUsingID(const kxf::String& id) override;
			bool CreateFromProject(const ModPackageProject& config) override;

			kxf::String GetSignature() const override
			{
				return m_Signature;
			}
			kxf::String GetID() const override
			{
				return m_ID;
			}
			void SetID(const kxf::String& id) override
			{
				m_ID = id;
				m_Signature = GetSignatureFromID(id);
			}
			
			kxf::String GetName() const override
			{
				if (!m_Name.IsEmpty())
				{
					return m_Name;
				}
				return m_ID;
			}
			void SetName(const kxf::String& value) override
			{
				m_Name = value;
			}
			
			kxf::String GetAuthor() const override
			{
				return m_Author;
			}
			void SetAuthor(const kxf::String& value) override
			{
				m_Author = value;
			}

			kxf::Version GetVersion() const override
			{
				return m_Version;
			}
			void SetVersion(const kxf::Version& value) override
			{
				m_Version = value;
			}
			
			bool IsDescriptionChanged() const
			{
				return m_IsDescriptionChanged;
			}
			kxf::String GetDescription() const override;
			void SetDescription(const kxf::String& value) override;

			wxDateTime GetInstallTime() const override
			{
				return m_TimeInstall;
			}
			void SetInstallTime(const wxDateTime& date) override
			{
				m_TimeInstall = date;
			}

			wxDateTime GetUninstallTime() const override
			{
				return m_TimeUninstall;
			}
			void SetUninstallTime(const wxDateTime& date) override
			{
				m_TimeUninstall = date;
			}
			
			const ModSourceStore& GetModSourceStore() const override
			{
				return m_ModSourceStore;
			}
			ModSourceStore& GetModSourceStore() override
			{
				return m_ModSourceStore;
			}

			const ModTagStore& GetTagStore() const override
			{
				return m_TagStore;
			}
			ModTagStore& GetTagStore() override
			{
				return m_TagStore;
			}

			kxf::String GetPackageFile() const override
			{
				return m_PackageFile;
			}
			void SetPackageFile(const kxf::String& value) override
			{
				m_PackageFile = value;
			}
			
			const FileTreeNode& GetFileTree() const override;
			void ClearFileTree() override;
			void UpdateFileTree() override;

			bool IsActive() const override
			{
				return m_IsActive && IsInstalled();
			}
			void SetActive(bool value) override
			{
				m_IsActive = value;
			}
			bool IsInstalled() const override
			{
				return m_FileTree.HasChildren();
			}

			bool HasColor() const override
			{
				return m_Color.IsOk();
			}
			KxColor GetColor() const override
			{
				return m_Color;
			}
			void SetColor(const KxColor& color)override 
			{
				m_Color = color;
			}
			kxf::ResourceID BasicGameMod::GetIcon() const override
			{
				return {};
			}

			bool IsLinkedMod() const override
			{
				return !m_LinkLocation.IsEmpty();
			}
			void LinkLocation(const kxf::String& path) override
			{
				m_LinkLocation = path;
			}
			void UnlinkLocation() override
			{
				m_LinkLocation.clear();
			}

			kxf::String GetModFilesDir() const override;
	};
}
