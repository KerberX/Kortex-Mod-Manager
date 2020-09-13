#include "stdafx.h"
#include "ITranslator.h"
#include <Kortex/Application.hpp>
#include <Kortex/GameInstance.hpp>

namespace Kortex
{
	const ITranslator& ITranslator::GetAppTranslator()
	{
		return IApplication::GetInstance()->GetTranslator();
	}

	kxf::String ITranslator::GetVariable(const kxf::String& variable, const kxf::String& variableNamespace)
	{
		return IApplication::GetInstance()->ExpandVariables(Variables::WrapAsInline(variable, variableNamespace));
	}
	kxf::String ITranslator::GetVariable(const IGameInstance& instance, const kxf::String& variable, const kxf::String& variableNamespace)
	{
		return instance.ExpandVariables(Variables::WrapAsInline(variable, variableNamespace));
	}
	kxf::String ITranslator::ExpandVariables(const kxf::String& variables)
	{
		return IApplication::GetInstance()->ExpandVariables(variables);
	}
	kxf::String ITranslator::ExpandVariables(const IGameInstance& instance, const kxf::String& source)
	{
		return instance.ExpandVariables(source);
	}
}
