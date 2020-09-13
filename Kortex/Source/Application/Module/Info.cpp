#include "stdafx.h"
#include "ManagerInfo.h"
#include "ModuleInfo.h"
#include "Application/IApplication.h"
#include "Application/ITranslator.h"
#include <optional>

namespace
{
	std::optional<kxf::String> GetTranslation(const kxf::String& value)
	{
		using namespace Kortex;

		if (IApplication* application = IApplication::GetInstance())
		{
			return application->GetTranslator().TryGetString(value);
		}
		return std::nullopt;
	}
}

namespace Kortex
{
	kxf::String SimpleModuleInfo::GetName() const
	{
		auto value = GetTranslation(m_Name);
		return value ? *value : m_Name;
	}
}

namespace Kortex
{
	kxf::String SimpleManagerInfo::GetName() const
	{
		auto value = GetTranslation(m_Name);
		return value ? *value : m_Name;
	}
}
