#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/Resources/ImageResourceID.h"
#include "Utility/LabeledValue.h"
#include "Utility/WithBitmap.h"
#include <kxf::UI::Framework/kxf::Version.h>

namespace Kortex
{
	class ModPackageProject;
	class FileTreeNode;
	class ModTagStore;
	class ModSourceStore;
}

namespace Kortex
{
	class IGameMod: public KxRTTI::Interface<IGameMod>
	{
		KxRTTI_DeclareIID(IGameMod, {0xaa4f8b27, 0xdb94, 0x4e34, {0x92, 0x3d, 0xe9, 0x4c, 0x82, 0xf8, 0xf3, 0xf5}});

		friend class IModManager;
		friend class IModDispatcher;

		public:
			using Vector = std::vector<std::unique_ptr<IGameMod>>;
			using RefVector = std::vector<IGameMod*>;
			using CRefVector = std::vector<const IGameMod*>;

		private:
			intptr_t m_Priority = -1;

		protected:
			void SetPriority(intptr_t value)
			{
				m_Priority = value;
			}
			void ModPriority(intptr_t value)
			{
				m_Priority += value;
			}

		public:
			static kxf::String GetSignatureFromID(const kxf::String& id);

		public:
			virtual bool IsOK() const = 0;
			virtual bool Save() = 0;

			virtual bool LoadUsingSignature(const kxf::String& signature) = 0;
			virtual bool LoadUsingID(const kxf::String& id);
			virtual bool CreateFromProject(const ModPackageProject& config);
			
			intptr_t GetPriority() const
			{
				return m_Priority;
			}
			virtual intptr_t GetDisplayOrder() const
			{
				return m_Priority;
			}

			virtual kxf::String GetSignature() const = 0;
			virtual kxf::String GetID() const = 0;
			virtual void SetID(const kxf::String& id) = 0;
			
			virtual kxf::String GetName() const = 0;
			virtual void SetName(const kxf::String& value) = 0;
			kxf::String GetSafeName() const;
			
			virtual kxf::String GetAuthor() const = 0;
			virtual void SetAuthor(const kxf::String& value) = 0;

			virtual kxf::Version GetVersion() const = 0;
			virtual void SetVersion(const kxf::Version& value) = 0;

			virtual kxf::String GetDescription() const = 0;
			virtual void SetDescription(const kxf::String& value) = 0;

			virtual wxDateTime GetInstallTime() const = 0;
			virtual void SetInstallTime(const wxDateTime& date) = 0;

			virtual wxDateTime GetUninstallTime() const = 0;
			virtual void SetUninstallTime(const wxDateTime& date) = 0;

			virtual const ModSourceStore& GetModSourceStore() const = 0;
			virtual ModSourceStore& GetModSourceStore() = 0;

			virtual const ModTagStore& GetTagStore() const = 0;
			virtual ModTagStore& GetTagStore() = 0;

			virtual kxf::String GetPackageFile() const = 0;
			virtual void SetPackageFile(const kxf::String& value) = 0;
			bool IsPackageFileExist() const;
			
			virtual const FileTreeNode& GetFileTree() const = 0;
			virtual void ClearFileTree() = 0;
			virtual void UpdateFileTree() = 0;

			virtual bool IsActive() const = 0;
			virtual void SetActive(bool value) = 0;
			virtual bool IsInstalled() const = 0;

			virtual bool HasColor() const = 0;
			virtual KxColor GetColor() const = 0;
			virtual void SetColor(const KxColor& color) = 0;
			virtual kxf::ResourceID GetIcon() const = 0;

			virtual bool IsLinkedMod() const = 0;
			virtual void UnlinkLocation() = 0;
			virtual void LinkLocation(const kxf::String& path) = 0;

			kxf::String GetRootDir() const;
			kxf::String GetDescriptionFile() const;
			kxf::String GetInfoFile() const;
			kxf::String GetImageFile() const;
			kxf::String GetDefaultModFilesDir() const;
			virtual kxf::String GetModFilesDir() const = 0;
	};
}
