#include "stdafx.h"
#include "DisplayModelNode.h"
#include "Utility/String.h"
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxTextFile.h>

using namespace Kortex::Utility;

namespace Kortex::Application::About
{
	kxf::String INode::GetLocation(Type type) const
	{
		const IApplication* app = IApplication::GetInstance();

		switch (type)
		{
			case Type::Application:
			{
				return String::ConcatWithSeparator(wxS('\\'), app->GetDataFolder(), wxS("About"));
			}
			case Type::Software:
			{
				return String::ConcatWithSeparator(wxS('\\'), app->GetDataFolder(), wxS("About"), wxS("Software"), GetName());
			}
			case Type::Resource:
			{
				return String::ConcatWithSeparator(wxS('\\'), app->GetDataFolder(), wxS("About"), wxS("Resource"), GetName());
			}
		};
		return wxEmptyString;
	}
	kxf::String INode::ReadLicense(Type type) const
	{
		kxf::String license = KxTextFile::ReadToString(String::ConcatWithSeparator(wxS('\\'), GetLocation(type), wxS("License.txt")));
		if (!license.IsEmpty())
		{
			// Create clickable links (<a href="link">link</a>) from '<link>' or just link
			{
				wxRegEx regex("(<?)(https?:\\/\\/|www.)([\\w\\d\\$\\-\\._+!*'(),\\/]+)(>?)", wxRE_ADVANCED|wxRE_ICASE);
				regex.ReplaceAll(&license, wxS("<a href=\"\\2\\3\">\\2\\3</a>"));
			}
			
			// Convert emails
			{
				wxRegEx regex("(<?)([a-zA-Z0-9.!#$%&�*+=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*)(>?)", wxRE_ADVANCED|wxRE_ICASE);
				regex.ReplaceAll(&license, wxS("<a href=\"mailto:\\2\">\\2</a>"));
			}
		}
		return license;
	}
	const kxf::String& INode::LoadLicense(LicenseData& data, Type type) const
	{
		if (data.m_License.IsEmpty() && data.m_ShouldLoad)
		{
			data.m_License = ReadLicense(type);
			data.m_ShouldLoad = false;
		}
		return data;
	}
}

namespace Kortex::Application::About
{
	kxf::String AppNode::GetName() const
	{
		return IApplication::GetInstance()->GetName();
	}
	kxf::Version AppNode::GetVersion() const
	{
		return IApplication::GetInstance()->GetVersion();
	}
	kxf::ResourceID AppNode::GetIconID() const
	{
		return ImageResourceID::KortexLogoSmall;
	}
	KxURI AppNode::GetURI() const
	{
		return wxS("https://github.com/KerberX/Kortex-Mod-Manager");
	}
	
	bool AppNode::HasLicense() const
	{
		return !LoadLicense(m_Licence, Type::Application).IsEmpty();
	}
	kxf::String AppNode::GetLicense() const
	{
		return LoadLicense(m_Licence, Type::Application);
	}
}

namespace Kortex::Application::About
{
	kxf::String ModuleNode::GetName() const
	{
		return m_Module.GetModuleInfo().GetName();
	}
	kxf::Version ModuleNode::GetVersion() const
	{
		return m_Module.GetModuleInfo().GetVersion();
	}
	kxf::ResourceID ModuleNode::GetIconID() const
	{
		return m_Module.GetModuleInfo().GetImageID();
	}
	
	bool ModuleNode::HasLicense() const
	{
		return !LoadLicense(m_Licence, Type::Software).IsEmpty();
	}
	kxf::String ModuleNode::GetLicense() const
	{
		return LoadLicense(m_Licence, Type::Software);
	}
}
