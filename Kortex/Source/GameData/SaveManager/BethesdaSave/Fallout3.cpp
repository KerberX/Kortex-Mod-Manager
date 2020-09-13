#include "stdafx.h"
#include "Fallout3.h"
#include <KxFramework/KxFileStream.h>

namespace Kortex::SaveManager::BethesdaSave
{
	bool Fallout3::OnRead(const KxFileItem& fileItem)
	{
		// https://falloutmods.fandom.com/wiki/FOS_file_format

		KxFileStream stream(fileItem.GetFullPath(), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
		if (stream.IsOk())
		{
			if (stream.ReadStringASCII(11) == wxS("FO3SAVEGAME"))
			{
				// Skip 'headerSize' field
				stream.Skip<uint32_t>();

				// Unknown, possibly file version number, always 0x30
				m_SaveVersion = stream.ReadObject<uint32_t>();

				// Read screenshot dimensions
				stream.Skip<uint8_t>();
				uint32_t width = stream.ReadObject<uint32_t>();
				stream.Skip<uint8_t>();
				uint32_t height = stream.ReadObject<uint32_t>();

				// Read save index
				stream.Skip<uint8_t>();
				m_BasicInfo.emplace_back(std::to_wstring(stream.ReadObject<uint32_t>()), KTr("SaveManager.Info.SaveIndex")).Order(0).Display().DisplayLabel();

				auto ReadWZString = [this, &stream](const kxf::String& fieldName)
				{
					stream.Skip<uint8_t>();
					uint16_t length = stream.ReadObject<uint16_t>();
					stream.Skip<uint8_t>();
					return m_BasicInfo.emplace_back(stream.ReadStringACP(length), KTr(fieldName));
				};

				// Read name
				ReadWZString("SaveManager.Info.Name").Order(1).Display();
				ReadWZString("SaveManager.Info.Karma");

				// Read level
				stream.Skip<uint8_t>();
				m_BasicInfo.emplace_back(std::to_wstring(stream.ReadObject<uint32_t>()), KTr("SaveManager.Info.Level")).Order(2).Display().DisplayLabel();

				// Read location
				ReadWZString("SaveManager.Info.Location").Order(3).Display();

				// Read game time
				ReadWZString("SaveManager.Info.TimeInGame");

				// Skip separator
				stream.Skip<uint8_t>();

				// Read image
				m_Bitmap = ReadBitmapRGB(stream.ReadVector<uint8_t>(width * height * 3), width, height);

				// Skip 'formVersion' and 'pluginInfoSize' fields
				stream.Skip<uint8_t, uint32_t>();

				// Read plugins list
				size_t count = stream.ReadObject<uint8_t>();
				for (size_t i = 0; i < count; i++)
				{
					stream.Skip<uint8_t>();
					uint16_t length = stream.ReadObject<uint16_t>();
					stream.Skip<uint8_t>();
					m_PluginsList.emplace_back(stream.ReadStringACP(length));
				}

				SortBasicInfo();
				return true;
			}
		}
		return false;
	}
}
