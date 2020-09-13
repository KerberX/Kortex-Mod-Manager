#pragma once
#include <Kortex/Kortex.hpp>
#include "GameData/ISaveManager.h"
#include "Application/IWorkspace.h"
#include "Utility/LabeledValue.h"
#include "Utility/BitmapSize.h"

namespace Kortex::SaveManager
{
	class Config
	{
		private:
			Utility::LabeledValue::Vector m_FileFilters;
			kxf::String m_SaveImplementation;
			kxf::String m_Location;
			Utility::BitmapSize m_BitmapSize;

			kxf::String m_PrimarySaveExt;
			kxf::String m_SecondarySaveExt;

		public:
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& node);

		public:
			kxf::String GetSaveImplementation() const;
			kxf::String GetLocation() const;
			Utility::BitmapSize GetBitmapSize() const
			{
				return m_BitmapSize;
			}

			bool HasFileFilter() const
			{
				return !m_FileFilters.empty();
			}
			const Utility::LabeledValue::Vector& GetFileFilters() const
			{
				return m_FileFilters;
			}

			bool HasMultiFileSaveConfig() const
			{
				return !m_SecondarySaveExt.IsEmpty();
			}
			const kxf::String& GetPrimarySaveExtension() const
			{
				return m_PrimarySaveExt;
			}
			const kxf::String& GetSecondarySaveExtension() const
			{
				return m_SecondarySaveExt;
			}
	};
}

namespace Kortex::SaveManager
{
	namespace Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	}

	class DefaultSaveManager: public ISaveManager
	{
		private:
			BroadcastReciever m_BroadcastReciever;
			Config m_Config;
			KxStringVector m_ActiveFilters;

		private:
			void CreateWorkspaces() override;
			void OnSavesLocationChanged(BroadcastEvent& event);

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
			std::unique_ptr<IGameSave> NewSave() const override;

			void UpdateActiveFilters(const KxStringVector& filters) override;
	};
}
