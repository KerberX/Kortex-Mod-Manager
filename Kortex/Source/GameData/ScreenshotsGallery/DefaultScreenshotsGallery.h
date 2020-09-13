#pragma once
#include <Kortex/Kortex.hpp>
#include "GameData/IScreenshotsGallery.h"
#include "Application/IWorkspace.h"

namespace Kortex::ScreenshotsGallery
{
	class Config
	{
		private:
			KxStringVector m_Locations;

		public:
			void OnLoadInstance(IGameInstance& profile, const kxf::XMLNode& node);

		public:
			KxStringVector GetLocations() const;
	};
}

namespace Kortex::ScreenshotsGallery
{
	class DefaultScreenshotsGallery: public IScreenshotsGallery
	{
		private:
			Config m_Config;

		protected:
			void CreateWorkspaces() override;

		protected:
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;
			void OnInit() override;
			void OnExit() override;

		public:
			const Config& GetConfig() const override
			{
				return m_Config;
			}
			IWorkspace::RefVector EnumWorkspaces() const override;
	};
}
