#pragma once
#include <Kortex/Kortex.hpp>

namespace Kortex::Utility
{
	class ScopedTempFile final
	{
		private:
			const kxf::String m_FilePath;

		public:
			ScopedTempFile(const kxf::String& filePath)
				:m_FilePath(filePath)
			{
			}
			~ScopedTempFile();

		public:
			operator const kxf::String&() const
			{
				return m_FilePath;
			}
	};
}

namespace Kortex::Utility
{
	class TempFolderKeeper
	{
		private:
			static void InitGlobalTemp();
			static kxf::String DoCreateTempFile(const kxf::String& folder);
			static kxf::String DoCreateTempFile(const kxf::String& folder, const kxf::String& suffix);

		public:
			static const kxf::String& GetGlobalTemp();
			static kxf::String CreateGlobalTempFile(const kxf::String& suffix = wxEmptyString);
			static ScopedTempFile CreateScopedGlobalTempFile(const kxf::String& suffix = wxEmptyString);

		private:
			const kxf::String m_TempFolder;

		private:
			kxf::String InitTempFolder() const;

		public:
			TempFolderKeeper();
			virtual ~TempFolderKeeper();

		public:
			kxf::String GetFolder() const
			{
				return m_TempFolder;
			}
			
			kxf::String CreateTempFile(const kxf::String& suffix = wxEmptyString) const;
			ScopedTempFile CreateScopedTempFile(const kxf::String& suffix = wxEmptyString);
	};
}
