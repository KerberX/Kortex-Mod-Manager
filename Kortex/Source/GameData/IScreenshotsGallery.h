#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IManager.h"
#include <KxFramework/KxSingleton.h>

namespace Kortex
{
	namespace ScreenshotsGallery
	{
		using SupportedTypesVector = std::vector<wxBitmapType>;
		class Config;
	}
	namespace ScreenshotsGallery::Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	}

	class IScreenshotsGallery:
		public ManagerWithTypeInfo<IManager, ScreenshotsGallery::Internal::TypeInfo>,
		public KxSingletonPtr<IScreenshotsGallery>
	{
		public:
			static const KxStringVector& GetSupportedExtensions();
			static const ScreenshotsGallery::SupportedTypesVector& GetSupportedFormats();
			static bool IsAnimationFile(const kxf::String& filePath);

		public:
			IScreenshotsGallery();

		public:
			virtual const ScreenshotsGallery::Config& GetConfig() const = 0;
	};
}
