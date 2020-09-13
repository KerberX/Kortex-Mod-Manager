#pragma once
#include <Kortex/Kortex.hpp>
#include "ProjectSection.h"

namespace Kortex::PackageProject
{
	class ConfigSection: public ProjectSection
	{
		public:
			static bool IsCompressionMethodSupported(const kxf::String& value);
	
		public:
			static const int ms_MinCompressionLevel = 0;
			static const int ms_MaxCompressionLevel = 9;
			static const int ms_DefaultCompressionLevel = 5;
	
			static const int ms_MinDictionarySize = 0;
			static const int ms_MaxDictionarySize = 10;
			static const int ms_DefaultDictionarySize = 5;
			static const kxf::String ms_DefaultCompressionMethod;
	
		private:
			kxf::String m_InstallPackageFile;
	
			kxf::String m_CompressionMethod = ms_DefaultCompressionMethod;
			int m_CompressionLevel = ms_DefaultCompressionLevel;
			int m_CompressionDictionarySize = ms_DefaultDictionarySize;
			bool m_CompressionUseMultithreading = false;
			bool m_CompressionSolidArchive = false;
	
		public:
			ConfigSection(ModPackageProject& project);
			~ConfigSection();
	
		public:
			const kxf::String& GetInstallPackageFile() const
			{
				return m_InstallPackageFile;
			}
			void SetInstallPackageFile(const kxf::String& value)
			{
				m_InstallPackageFile = value;
			}
	
			const kxf::String& GetCompressionMethod() const
			{
				return m_CompressionMethod;
			}
			void SetCompressionMethod(const kxf::String& value);
			
			int GetCompressionLevel() const
			{
				return m_CompressionLevel;
			}
			void SetCompressionLevel(int value);
			
			int GetCompressionDictionarySize() const
			{
				return m_CompressionDictionarySize;
			}
			void SetCompressionDictionarySize(int value);
			
			bool IsMultithreadingUsed() const
			{
				return m_CompressionUseMultithreading;
			}
			void SetUseMultithreading(bool value)
			{
				m_CompressionUseMultithreading = value;
			}
			
			bool IsSolidArchive() const
			{
				return m_CompressionSolidArchive;
			}
			void SetSolidArchive(bool value)
			{
				m_CompressionSolidArchive = value;
			}
	};
}
