#pragma once
#include <Kortex/Kortex.hpp>
#include "INISource.h"
#include "GameConfig/ConfigManger/IFileSource.h"

namespace Kortex::GameConfig
{
	class INIFileSource: public KxRTTI::ImplementInterface<INIFileSource, INISource, IFileSource>
	{
		private:
			kxf::String m_FilePath;
			kxf::String m_FileName;

		public:
			INIFileSource(const kxf::String& filePath)
				:m_FilePath(filePath), m_FileName(filePath.AfterLast(wxS('\\')))
			{
			}

		public:
			// IFileSource
			kxf::String GetFileName() const override
			{
				return m_FileName;
			}
			kxf::String GetFilePath() const override
			{
				return m_FilePath;
			}

			// ISource
			kxf::String GetPathDescription() const override
			{
				// No file name resolution is required, we can return it unchanged.
				return m_FileName;
			}

			bool Open() override;
			bool Save() override;
			void Close() override;
	};
}
