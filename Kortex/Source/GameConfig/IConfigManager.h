#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/IManager.h"
#include "Application/RefStackTranslator.h"
#include "ConfigManger/ItemValue.h"
#include "ConfigManger/IAction.h"
#include "ConfigManger/ISamplingFunction.h"
#include <KxFramework/KxTranslation.h>
class kxf::XMLNode;

namespace Kortex
{
	namespace GameConfig
	{
		class DisplayModel;
		class Definition;
		class ItemGroup;
		class Item;
	}
	namespace ConfigManager::Internal
	{
		extern const SimpleManagerInfo TypeInfo;
	};

	class IConfigManager:
		public KxRTTI::ExtendInterface<IConfigManager, ManagerWithTypeInfo<IManager, ConfigManager::Internal::TypeInfo>>
	{
		KxRTTI_DeclareIID(IConfigManager, {0x7a3622ac, 0xf11, 0x4999, {0x9c, 0xe5, 0xad, 0x8d, 0xd4, 0xf, 0xf9, 0xa5}});

		friend class GameConfig::Item;
		friend class GameConfig::DisplayModel;

		public:
			using DefinitionFunc = std::function<void(const GameConfig::Definition& definition)>;

		public:
			static kxf::String GetDefinitionFileByID(const kxf::String& id);

		private:
			KxTranslation m_Translation;

		protected:
			void OnInit() override;
			void OnExit() override;
			void OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode) override;
			
			virtual void OnCreateDisplayModel(GameConfig::DisplayModel& displayModel)
			{
			}
			virtual void OnDestroyDisplayModel(GameConfig::DisplayModel& displayModel)
			{
			}

			virtual RefStackTranslator& GetTranslatorStack() = 0;
			bool LoadTranslation(KxTranslation& translation, const kxf::String& component);
			bool LoadDefaultTranslation();

			virtual void OnItemChanged(GameConfig::Item& item) = 0;
			virtual void OnItemChangeDiscarded(GameConfig::Item& item) = 0;

		public:
			IConfigManager();

		public:
			virtual const ITranslator& GetTranslator() const = 0;
			virtual GameConfig::DisplayModel* GetDisplayModel() const = 0;

			virtual void ForEachDefinition(const DefinitionFunc& func) = 0;
			template<class TFunctor> void ForEachGroup(TFunctor&& func)
			{
				ForEachDefinition([&func](auto&& definition)
				{
					definition.ForEachGroup(func);
				});
			}
			template<class TFunctor> void ForEachItem(TFunctor&& func)
			{
				ForEachGroup([&func](auto&& group)
				{
					group.ForEachItem(func);
				});
			}

			virtual void Load() = 0;
			virtual void SaveChanges() = 0;
			virtual void DiscardChanges() = 0;
			virtual bool HasUnsavedChanges() const = 0;

			kxf::String TranslateItemLabel(const kxf::String& name, const kxf::String& perfix) const;
			kxf::String TranslateItemLabel(const kxf::XMLNode& itemNode, const kxf::String& name, const kxf::String& perfix, bool isAttribute = false) const;
			std::pair<kxf::String, bool> TranslateItemElement(const kxf::XMLNode& itemNode, bool isAttribute = false, const kxf::String& attributeName = {}) const;

			virtual std::unique_ptr<GameConfig::IAction> QueryAction(const kxf::String& name)
			{
				return nullptr;
			}
			virtual std::unique_ptr<GameConfig::ISamplingFunction> QuerySamplingFunction(const kxf::String& name, GameConfig::SampleValue::Vector& samples)
			{
				return nullptr;
			}
	};
}
