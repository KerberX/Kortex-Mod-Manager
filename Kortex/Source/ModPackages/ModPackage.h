#pragma once
#include <Kortex/Kortex.hpp>
#include "ModPackages/IPackageManager.h"
#include "PackageProject/ModPackageProject.h"
#include "Archive/GenericArchive.h"
#include "Utility/LabeledValue.h"
#include <kxf::UI::Framework/KxFileStream.h>

namespace Kortex
{
	class ModPackage final
	{
		private:
			static const size_t ms_InvalidIndex = std::numeric_limits<size_t>::max();

		private:
			GenericArchive m_Archive;
			KxFileStream m_Stream;
			ModPackageProject m_Config;
			kxf::String m_PackageFilePath;
			kxf::String m_EffectiveArchiveRoot;
			PackageProject::PackageType m_PackageType = PackageProject::PackageType::Unknown;

			std::unordered_map<KxArchive::FileIndex, std::unique_ptr<wxMemoryOutputStream>> m_DocumentsBuffer;
			bool m_DocumentsLoaded = false;

		public:
			wxBitmap ReadImage(wxInputStream& stream) const;
			wxBitmap ReadImage(wxMemoryOutputStream& stream) const
			{
				wxMemoryInputStream inStream(stream);
				return ReadImage(inStream);
			}
			wxBitmap ReadImage(size_t index) const;
			kxf::String ReadString(wxInputStream& stream, bool isASCII = false) const;
			kxf::String ReadString(wxMemoryOutputStream& stream, bool isASCII = false) const
			{
				wxMemoryInputStream inStream(stream);
				return ReadString(inStream, isASCII);
			}
			kxf::String ReadString(size_t index, bool isASCII = false) const;

		private:
			void LoadConfig(ModPackageProject& project);
			void LoadConfigNative(ModPackageProject& project, size_t index);
			void LoadConfigSMI(ModPackageProject& project, size_t index);
			void LoadConfigFOMod(ModPackageProject& project, size_t infoIndex, size_t moduleConfigIndex);
		
			kxf::String DetectEffectiveArchiveRoot(const KxFileItem& item, const kxf::String& subPath = {}) const;
			void SetModIDIfNone();

			void LoadBasicResources();
			void LoadImageResources();
			void LoadDocumentResources();

		private:
			void Init(const kxf::String& archivePath);

		public:
			ModPackage();
			ModPackage(const kxf::String& archivePath);
			ModPackage(const kxf::String& archivePath, ModPackageProject& project);
			bool Create(const kxf::String& archivePath);
			bool Create(const kxf::String& archivePath, ModPackageProject& project);
			~ModPackage();

		public:
			bool IsOK() const;
			bool IsTypeSupported() const;

			const kxf::String& GetPackageFilePath() const
			{
				return m_PackageFilePath;
			}
			const GenericArchive& GetArchive() const
			{
				return m_Archive;
			}
			GenericArchive& GetArchive()
			{
				return m_Archive;
			}
			PackageProject::PackageType GetType() const
			{
				return m_PackageType;
			}
			const ModPackageProject& GetConfig() const
			{
				return m_Config;
			}
			ModPackageProject& GetConfig()
			{
				return m_Config;
			}
		
			void LoadResources();

			kxf::String GetName() const
			{
				return m_Config.GetModName();
			}
			const PackageProject::ImageItem* GetLogoImage() const
			{
				return m_Config.GetInterface().GetMainItem();
			}
			const kxf::String& GetDescription() const
			{
				return m_Config.GetInfo().GetDescription();
			}

			std::unique_ptr<wxInputStream> GetDocumentStream(const Utility::LabeledValue& item) const;
			kxf::String GetSimpleDocument(const Utility::LabeledValue& item) const
			{
				if (auto stream = GetDocumentStream(item))
				{
					return ReadString(*stream);
				}
				return {};
			}
	};
}
