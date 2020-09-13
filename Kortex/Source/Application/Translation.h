#pragma once
#include <Kortex/Kortex.hpp>
#include "ITranslator.h"

namespace Kortex
{
	class IGameInstance;

	template<class T>
	kxf::String Translate(T&& id)
	{
		return {};
		//return ITranslator::GetAppTranslator().GetString(id);
	}

	template<class T>
	kxf::String Translate(const IGameInstance& instance, T&& id)
	{
		return {};
		//return ITranslator::GetAppTranslator().GetString(instance, id);
	}

	template<class T, class... Args>
	kxf::String FormatTranslate(T&& id, Args&&... arg)
	{
		return {};
		//return ITranslator::GetAppTranslator().FormatString(id, std::forward<Args>(arg)...);
	}

	template<class T, class... Args>
	kxf::String FormatTranslate(const IGameInstance& instance, T&& id, Args&&... arg)
	{
		return {};
		//return ITranslator::GetAppTranslator().FormatString(instance, id, std::forward<Args>(arg)...);
	}
}

#define KVarExp Kortex::ITranslator::ExpandVariables
#define KTr Kortex::Translate
#define KTrf Kortex::FormatTranslate
