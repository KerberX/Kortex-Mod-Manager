#include "stdafx.h"
#include "IConfigManager.h"
#include "ConfigManger/Item.h"
#include "ConfigManger/ItemGroup.h"
#include "ConfigManger/Definition.h"
#include <Kortex/Common/GameConfig.hpp>
#include <Kortex/Application.hpp>

namespace Kortex
{
	namespace ConfigManager::Internal
	{
		const SimpleManagerInfo TypeInfo("ConfigManager", "ConfigManager.Name");
	}

	kxf::String IConfigManager::GetDefinitionFileByID(const kxf::String& id)
	{
		return IApplication::GetInstance()->GetDataFolder() + wxS("\\ConfigDefinitions\\") + id + wxS(".xml");
	}

	void IConfigManager::OnInit()
	{
	}
	void IConfigManager::OnExit()
	{
	}
	void IConfigManager::OnLoadInstance(IGameInstance& instance, const kxf::XMLNode& managerNode)
	{
		LoadDefaultTranslation();
	}

	bool IConfigManager::LoadTranslation(KxTranslation& translation, const kxf::String& component)
	{
		const IApplication* app = IApplication::GetInstance();
		const KxTranslation& appTranslation = app->GetTranslation();
		const kxf::String locale = appTranslation.GetLocale();

		LoadTranslationStatus status = app->TryLoadTranslation(translation, app->GetAvailableTranslations(), component, locale);
		return status == LoadTranslationStatus::Success;
	}
	bool IConfigManager::LoadDefaultTranslation()
	{
		RefStackTranslator& translator = GetTranslatorStack();
		translator.Push(IApplication::GetInstance()->GetTranslation());

		if (LoadTranslation(m_Translation, "ConfigManager"))
		{
			translator.Push(m_Translation);
			return true;
		}
		return false;
	}

	IConfigManager::IConfigManager()
	{
		AssignModule(*GameConfigModule::GetInstance());
	}

	kxf::String IConfigManager::TranslateItemLabel(const kxf::String& name, const kxf::String& perfix) const
	{
		if (!name.IsEmpty())
		{
			auto value = GetTranslator().TryGetString(perfix + wxS('.') + name);
			if (value)
			{
				return *value;
			}
			return name;
		}
		return {};
	}
	kxf::String IConfigManager::TranslateItemLabel(const kxf::XMLNode& itemNode, const kxf::String& name, const kxf::String& perfix, bool isAttribute) const
	{
		auto [text, isTranslated] = TranslateItemElement(itemNode, isAttribute, wxS("Label"));
		if (!isTranslated)
		{
			text = TranslateItemLabel(text, perfix);
			if (!text.IsEmpty())
			{
				return text;
			}
			return TranslateItemLabel(name, perfix);
		}
		return text;
	}
	std::pair<kxf::String, bool> IConfigManager::TranslateItemElement(const kxf::XMLNode& itemNode, bool isAttribute, const kxf::String& attributeName) const
	{
		kxf::String text = isAttribute ? itemNode.GetAttribute(attributeName) : itemNode.GetValue();
		if (!text.IsEmpty())
		{
			// Strip translation variable
			if (text.StartsWith(wxS("$T(")) && text.EndsWith(wxS(")")))
			{
				text.Remove(0, 3);
				text.RemoveLast(1);
			}

			auto value = GetTranslator().TryGetString(text);
			if (value)
			{
				return {*value, true};
			}
		}
		return {text, false};
	}
}
