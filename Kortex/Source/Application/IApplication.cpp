#include "stdafx.h"
#include "IApplication.h"
#include "SystemApplication.h"
#include "Options/CmdLineDatabase.h"
#include <Kortex/Application.hpp>
#include "IMainWindow.h"
#include "Utility/Log.h"
#include <kxf::UI::Framework/KxSystem.h>
#include <kxf::UI::Framework/KxTaskScheduler.h>

namespace Kortex::OName
{
	KortexDefOption(RestartDelay);
}

namespace Kortex
{
	SystemApplication* IApplication::GetSystemApp()
	{
		return SystemApplication::GetInstance();
	}

	void IApplication::OnConfigureCommandLine()
	{
		wxCmdLineParser& cmdLineParser = GetCmdLineParser();

		cmdLineParser.SetSwitchChars("-");
		cmdLineParser.AddOption(CmdLineName::InstanceID, wxEmptyString, "Instance ID");
		cmdLineParser.AddOption(CmdLineName::GlobalConfigPath, wxEmptyString, "Folder path for app-wide config");
		cmdLineParser.AddOption(CmdLineName::DownloadLink, wxEmptyString, "Download link");
	}

	kxf::String IApplication::RethrowCatchAndGetExceptionInfo() const
	{
		return GetSystemApp()->RethrowCatchAndGetExceptionInfo();
	}

	kxf::String IApplication::GetRootFolder() const
	{
		return GetSystemApp()->GetRootFolder();
	}
	kxf::String IApplication::GetExecutablePath() const
	{
		return GetSystemApp()->GetExecutablePath();
	}
	kxf::String IApplication::GetExecutableName() const
	{
		return GetSystemApp()->GetExecutableName();
	}

	bool IApplication::Is64Bit() const
	{
		#if defined _WIN64
		return true;
		#else
		return false;
		#endif
	}
	bool IApplication::IsSystem64Bit() const
	{
		return KxSystem::Is64Bit();
	}
	bool IApplication::IsAnotherRunning() const
	{
		return GetSystemApp()->IsAnotherRunning();
	}

	bool IApplication::QueueDownloadToMainProcess(const kxf::String& link)
	{
		return GetSystemApp()->QueueDownloadToMainProcess(link);
	}
	std::optional<kxf::String> IApplication::GetLinkFromCommandLine() const
	{
		using namespace Application;

		kxf::String link;
		if (GetCmdLineParser().Found(CmdLineName::DownloadLink, &link) && !link.IsEmpty())
		{
			return link;
		}
		return std::nullopt;
	}

	wxCmdLineParser& IApplication::GetCmdLineParser() const
	{
		return GetSystemApp()->GetCmdLineParser();
	}
	kxf::String IApplication::FormatCommandLine(const CmdLineParameters& parameters)
	{
		kxf::String commandLine;
		auto AddCommand = [&commandLine](const kxf::String& command)
		{
			if (!command.IsEmpty())
			{
				if (!commandLine.IsEmpty())
				{
					commandLine += wxS(' ');
				}
				commandLine += command;
			}
		};

		if (!parameters.InstanceID.IsEmpty())
		{
			AddCommand(kxf::String::Format(wxS("-%1 \"%2\""), CmdLineName::InstanceID, parameters.InstanceID));
		}
		if (!parameters.DownloadLink.IsEmpty())
		{
			AddCommand(kxf::String::Format(wxS("-%1 \"%2\""), CmdLineName::DownloadLink, parameters.DownloadLink));
		}
		if (!parameters.GlobalConfigPath.IsEmpty())
		{
			AddCommand(kxf::String::Format(wxS("-%1 \"%2\""), CmdLineName::GlobalConfigPath, parameters.GlobalConfigPath));
		}
		return commandLine;
	}
	bool IApplication::ScheduleRestart(const kxf::String& commandLine, std::optional<wxTimeSpan> timeout)
	{
		if (KxTaskScheduler taskSheduler; taskSheduler.IsOK())
		{
			wxTimeSpan delay;
			if (timeout && timeout->IsPositive())
			{
				delay = *timeout;
			}
			else
			{
				delay = wxTimeSpan::Seconds(GetGlobalOption(OName::RestartDelay).GetValueInt(3));
			}

			KxTaskSchedulerTask task = taskSheduler.NewTask();
			task.SetExecutable(GetExecutablePath(), commandLine);
			task.SetRegistrationTrigger("Restart", delay, wxDateTime::Now() + delay * 2);
			task.DeleteExpiredTaskAfter(delay);

			const kxf::String taskName = wxS("Kortex.ScheduleRestart");
			taskSheduler.DeleteTask(taskName);
			return taskSheduler.SaveTask(task, taskName);
		}
		return false;
	}

	void IApplication::EnableIE10Support()
	{
		GetSystemApp()->ConfigureForInternetExplorer10(true);
	}
	void IApplication::DisableIE10Support()
	{
		GetSystemApp()->ConfigureForInternetExplorer10(false);
	}

	kxf::String IApplication::GetID() const
	{
		return GetSystemApp()->GetAppName();
	}
	kxf::String IApplication::GetName() const
	{
		return GetSystemApp()->GetAppDisplayName();
	}
	kxf::String IApplication::GetShortName() const
	{
		return GetSystemApp()->GetShortName();
	}
	kxf::String IApplication::GetDeveloper() const
	{
		return GetSystemApp()->GetVendorName();
	}
	kxf::Version IApplication::GetVersion() const
	{
		return GetSystemApp()->GetAppVersion();
	}
	kxf::Version IApplication::GetWxWidgetsVersion() const
	{
		return wxGetLibraryVersionInfo();
	}
	kxf::XMLDocument& IApplication::GetGlobalConfig() const
	{
		return GetSystemApp()->GetGlobalConfig();
	}
	IModule& IApplication::GetModule() const
	{
		return GetSystemApp()->m_ApplicationModule;
	}

	wxWindow* IApplication::GetActiveWindow() const
	{
		return ::wxGetActiveWindow();
	}
	wxWindow* IApplication::GetTopWindow() const
	{
		return GetSystemApp()->GetTopWindow();
	}
	void IApplication::SetTopWindow(wxWindow* window)
	{
		return GetSystemApp()->SetTopWindow(window);
	}
	bool IApplication::IsActive() const
	{
		return GetSystemApp()->IsActive();
	}
	bool IApplication::IsMainWindowActive() const
	{
		if (IsActive())
		{
			const IMainWindow* mainWindow = IMainWindow::GetInstance();
			if (mainWindow)
			{
				return mainWindow->GetFrame().GetHandle() == ::GetForegroundWindow();
			}
		}
		return false;
	}

	void IApplication::ExitApp(int exitCode)
	{
		GetSystemApp()->ExitApp(exitCode);
	}
	wxLog& IApplication::GetLogger()
	{
		return *GetSystemApp()->GetLogger();
	}
	BroadcastProcessor& IApplication::GetBroadcastProcessor()
	{
		return GetSystemApp()->GetBroadcastProcessor();
	}

	LoadTranslationStatus IApplication::TryLoadTranslation(KxTranslation& translation,
														   const KxTranslation::AvailableMap& availableTranslations,
														   const kxf::String& component,
														   const kxf::String& desiredLocale
	) const
	{
		auto LoadLang = [&translation, &component, &availableTranslations](const kxf::String& name, bool isFullName = false) -> bool
		{
			auto it = availableTranslations.find(isFullName ? name : name + '.' + component);
			if (it != availableTranslations.end())
			{
				Utility::Log::LogInfo("Trying to load translation from file \"%1\" for \"%2\" component", it->second, name);
				if (translation.LoadFromFile(it->second))
				{
					translation.SetLocale(name.BeforeFirst('.'));
					return true;
				}
			}
			return false;
		};

		if (!availableTranslations.empty())
		{
			// Try load translation for desired locale
			if (LoadLang(desiredLocale))
			{
				return LoadTranslationStatus::Success;
			}

			// Try default locales
			if (LoadLang(KxTranslation::GetUserDefaultLocale()) ||
				LoadLang(KxTranslation::GetSystemPreferredLocale()) ||
				LoadLang(KxTranslation::GetSystemDefaultLocale()) ||
				LoadLang("en-US"))
			{
				return LoadTranslationStatus::Success;
			}

			// Try first available
			const auto& first = *availableTranslations.begin();
			if (LoadLang(first.first, true))
			{
				return LoadTranslationStatus::Success;
			}
			return LoadTranslationStatus::LoadingError;
		}
		return LoadTranslationStatus::NoTranslations;
	}
}
