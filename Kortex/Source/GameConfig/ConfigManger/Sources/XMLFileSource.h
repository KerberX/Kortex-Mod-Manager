#pragma once
#include <Kortex/Kortex.hpp>
#include "XMLSource.h"
#include "GameConfig/ConfigManger/IFileSource.h"

namespace Kortex::GameConfig
{
	class XMLFileSource: public KxRTTI::ImplementInterface<XMLFileSource, XMLSource, IFileSource>
	{
		private:
			kxf::String m_FilePath;
			kxf::String m_FileName;

		public:
			XMLFileSource(const kxf::String& filePath)
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
				return m_FileName;
			}

			bool Open() override;
			bool Save() override;
			void Close() override;
	};
}
