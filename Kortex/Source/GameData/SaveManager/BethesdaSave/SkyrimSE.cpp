#include "stdafx.h"
#include "SkyrimSE.h"
#include <kxf::UI::Framework/KxFileStream.h>
#include <kxf::UI::Framework/KxLZ4Stream.h>
#include <kxf::UI::Framework/KxMemoryStream.h>

namespace
{
	template<class TCounter, class TStream> void ReadPluginList(TStream& stream, KxStringVector& plugins)
	{
		size_t pluginCount = stream.ReadObject<TCounter>();
		for (size_t i = 0; i < pluginCount; i++)
		{
			const uint16_t length = stream.ReadObject<uint16_t>();
			if (length != 0)
			{
				plugins.emplace_back(stream.ReadStringUTF8(length));
			}
			else
			{
				break;
			}
		}
	}
}

namespace Kortex::SaveManager::BethesdaSave
{
	bool SkyrimSE::OnRead(const KxFileItem& fileItem)
	{
		KxFileStream stream(fileItem.GetFullPath(), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
		if (stream.IsOk())
		{
			if (stream.ReadStringASCII(13) == wxS("TESV_SAVEGAME"))
			{
				// Read 'headerSize'
				const uint32_t headerSize = stream.ReadObject<uint32_t>();

				// Read version
				m_SaveVersion = stream.ReadObject<uint32_t>();

				// Read basic info
				m_BasicInfo.emplace_back(std::to_string(stream.ReadObject<uint32_t>()), KTr("SaveManager.Info.SaveIndex")).Order(0).Display().DisplayLabel();
				m_BasicInfo.emplace_back(stream.ReadStringUTF8(stream.ReadObject<uint16_t>()), KTr("SaveManager.Info.Name")).Order(1).Display();
				m_BasicInfo.emplace_back(std::to_string(stream.ReadObject<uint32_t>()), KTr("SaveManager.Info.Level")).Order(2).Display().DisplayLabel();
				m_BasicInfo.emplace_back(stream.ReadStringUTF8(stream.ReadObject<uint16_t>()), KTr("SaveManager.Info.Location")).Order(3).Display();
				m_BasicInfo.emplace_back(stream.ReadStringUTF8(stream.ReadObject<uint16_t>()), KTr("SaveManager.Info.TimeInGame"));
				m_BasicInfo.emplace_back(stream.ReadStringUTF8(stream.ReadObject<uint16_t>()), KTr("SaveManager.Info.Race"));

				// Player sex
				uint16_t playerSex = stream.ReadObject<uint16_t>();
				m_BasicInfo.emplace_back(playerSex == 0 ? KTr("SaveManager.Info.SexMale") : KTr("SaveManager.Info.SexFemale"), KTr("SaveManager.Info.Sex"));

				// Skip 'playerCurExp', 'playerLvlUpExp' and 'filetime' fields
				stream.Skip<float32_t, float32_t, FILETIME>();

				// Read image
				uint32_t width = stream.ReadObject<uint32_t>();
				uint32_t height = stream.ReadObject<uint32_t>();

				// Skip unknown 2 bytes
				stream.Skip<uint16_t>();
				m_Bitmap = ReadBitmapRGBA(stream.ReadVector<uint8_t>(width * height * 4), width, height);

				// It seems that only save files with header size == 84 have esl block, otherwise and following code breaks.
				if (m_SaveVersion >= 12 && headerSize == 84)
				{
					uint32_t uncompressedSize = stream.ReadObject<uint32_t>();
					uint32_t compressedSize = stream.ReadObject<uint32_t>();

					// Read and decompress.
					// Not very efficient, but LZ4 docs is too much awful to write decompressing stream.
					KxUInt8Vector compressedData = stream.ReadVector<uint8_t>(compressedSize);
					KxUInt8Vector uncompressedData(uncompressedSize, 0);
					KxLZ4::Decompress(compressedData.data(), compressedData.size(), uncompressedData.data(), uncompressedData.size());
					KxIOStreamWrapper<KxMemoryInputStream> memoryStream(uncompressedData.data(), uncompressedData.size());

					// Skip unknown 5 bytes
					memoryStream.Skip(5);

					// ESM + ESP
					ReadPluginList<uint8_t>(memoryStream, m_PluginsList);

					// ESL
					ReadPluginList<uint16_t>(memoryStream, m_PluginsList);
				}
				else
				{
					// Skip 'formVersion' field, unknown 10 bytes and 'pluginInfoSize' field
					stream.Skip(1 + 10 + 4);

					// Read plugins list
					ReadPluginList<uint8_t>(stream, m_PluginsList);
				}

				SortBasicInfo();
				return true;
			}
		}
		return false;
	}
}
