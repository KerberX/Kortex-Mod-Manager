#include "pch.hpp"
#include "DefaultApplication.h"
#include "SystemApplication.h"
#include "Options/CmdLineDatabase.h"
#include "Log.h"

#include "GameInstance/IGameProfile.h"
#include "GameInstance/IGameInstance.h"

#include <kxf/System/ShellOperations.h>
#include <wx/cmdline.h>

namespace Kortex::Application
{
	// IApplication
	void DefaultApplication::OnCreate()
	{
		m_BroadcastReciever = std::make_unique<BroadcastReciever>();

		// Setup paths
		m_DataDirectory = GetRootDirectory() / "Data";
		m_SettingsDirectory = kxf::Shell::GetKnownDirectory(kxf::KnownDirectoryID::ApplicationDataLocal) / GetID();
		m_SettingsFile = m_SettingsDirectory / "Settings.xml";
		m_LogsDirectory = m_SettingsDirectory / "Logs";
		m_DefaultInstancesDirectory = m_SettingsDirectory / "Instances";

		// Variables
		m_Variables.SetItem("App", "DataDirectory", m_DataDirectory);
		m_Variables.SetDynamicItem("App", "Revision", [this]()
		{
			return m_Variables.GetItem("App", "CommitHash").GetAs<kxf::String>().Left(7);
		});
	}
	void DefaultApplication::OnDestroy()
	{
		m_BroadcastReciever = nullptr;
	}

	bool DefaultApplication::OnInit()
	{
		const bool anotherInstanceRunning = IsAnotherInstanceRunning();

		return false;
	}
	int DefaultApplication::OnExit()
	{
		Log::Info("DefaultApplication::OnExit");

		return 0;
	}
	bool DefaultApplication::OnException()
	{
		Log::FatalError(ExamineCaughtException());
		return false;
	}
	
	void DefaultApplication::OnGlobalConfigChanged(AppOption& option)
	{
	}
	void DefaultApplication::OnInstanceConfigChanged(AppOption& option, IGameInstance& instance)
	{
	}
	void DefaultApplication::OnProfileConfigChanged(AppOption& option, IGameProfile& profile)
	{
	}

	DefaultApplication::DefaultApplication() = default;
	DefaultApplication::~DefaultApplication() = default;

	kxf::String DefaultApplication::ExpandVariables(const kxf::String& variables) const
	{
		if (m_ActiveGameInstance)
		{
			return m_ActiveGameInstance->ExpandVariables(variables);
		}
		return ExpandVariablesLocally(variables);
	}
	kxf::String DefaultApplication::ExpandVariablesLocally(const kxf::String& variables) const
	{
		return m_Variables.Expand(variables);
	}

	kxf::object_ptr<IGameInstance> DefaultApplication::GetActiveGameInstance() const
	{
		return m_ActiveGameInstance;
	}

	bool DefaultApplication::OpenInstanceSelectionDialog()
	{
		return false;
	}
	bool DefaultApplication::Uninstall()
	{
		return false;
	}

	// kxf::Application::ICommandLine
	size_t DefaultApplication::EnumCommandLineArgs(std::function<bool(kxf::String)> func) const
	{
		if (m_CommandLineParser)
		{
			size_t count = 0;
			for (size_t i = 0; i < m_CommandLineParser->GetParamCount(); i++)
			{
				count++;
				if (!std::invoke(func, m_CommandLineParser->GetParam(i)))
				{
					break;
				}
			}
			return count;
		}
		return 0;
	}
	void DefaultApplication::OnCommandLineInit(wxCmdLineParser& parser)
	{
		m_CommandLineParser = &parser;

		parser.SetSwitchChars("-");
		parser.AddOption(CmdLineName::GameInstance, {}, "Game instance ID");
		parser.AddOption(CmdLineName::AddDownload, {}, "Download URI");
		parser.AddOption(CmdLineName::SettingsDirectory, {}, "Directory path for application-wide config");
	}
	bool DefaultApplication::OnCommandLineParsed(wxCmdLineParser& parser)
	{
		return true;
	}
	bool DefaultApplication::OnCommandLineError(wxCmdLineParser& parser)
	{
		if (m_CommandLineParser)
		{
			m_CommandLineParser->Usage();
		}
		return false;
	}
	bool DefaultApplication::OnCommandLineHelp(wxCmdLineParser& parser)
	{
		if (m_CommandLineParser)
		{
			m_CommandLineParser->Usage();
		}
		return false;
	}
}