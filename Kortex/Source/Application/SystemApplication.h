#pragma once
#include <Kortex/Kortex.hpp>
#include "ApplicationModule.h"
#include "BroadcastProcessor.h"
#include <kxf/Application/GUIApplication.h>
#include <kxf/Serialization/XML.h>
#include <kxf/Network/URI.h>
#include <wx/apptrait.h>
#include <wx/snglinst.h>

namespace Kortex
{
	class LogEvent;
	class GameID;
	class IGameInstance;
	class IGameProfile;
	class IApplication;
	class AppOption;
	class IThemeManager;
	class INotificationCenter;
	class BroadcastProcessor;
}
namespace Kortex::GameInstance
{
	class TemplateLoader;
}

namespace Kortex
{
	class SystemApplication;
	class SystemApplicationTraits final: public wxGUIAppTraits
	{
		friend class SystemApplication;

		private:
			SystemApplication& m_SystemApp;

			FILE* m_LogTargetFILE = nullptr;
			wxLogStderr* m_LogTarget = nullptr;

		private:
			FILE* CreateLogFile() const;

		public:
			SystemApplicationTraits(SystemApplication& systemApp);

		public:
			wxLog* CreateLogTarget() override;
	};
}

namespace Kortex
{
	class SystemApplication final: kxf::RTTI::ImplementInterface<SystemApplication, kxf::GUIApplication>
	{
		friend class SystemApplicationTraits;
		friend class IApplication;
		friend class AppOption;

		friend class IGameInstance;
		friend class GameInstance::TemplateLoader;
		friend class GameID;

		public:
			static SystemApplication& GetInstance() noexcept
			{
				return static_cast<SystemApplication&>(*ICoreApplication::GetInstance());
			}

		private:
			BroadcastProcessor m_BroadcastProcessor;
			BroadcastReciever m_BroadcastReciever;

			kxf::String m_RootFolder;
			kxf::String m_ExecutableName;
			kxf::String m_ExecutablePath;

			Application::ApplicationModule m_ApplicationModule;
			std::unique_ptr<IApplication> m_Application;
			std::unique_ptr<IThemeManager> m_ThemeManager;
			std::unique_ptr<INotificationCenter> m_NotificationCenter;
			bool m_IsApplicationInitialized = false;
			int m_ExitCode = 0;

			std::unique_ptr<IGameInstance> m_ActiveGameInstance;
			std::vector<std::unique_ptr<IGameInstance>> m_GameInstanceTemplates;
			std::vector<std::unique_ptr<IGameInstance>> m_ShallowGameInstances;

			SystemApplicationTraits* m_AppTraits = nullptr;
			wxSingleInstanceChecker m_SingleInstanceChecker;
			kxf::XMLDocument m_GlobalConfig;

		private:
			void InitLogging();
			void UninitLogging();

			void InitComponents();
			void UninitComponents();
			void SetPostCreateVariables();

			void LoadGlobalConfig();
			void SaveGlobalConfig();
			void TerminateActiveInstance();

		public:
			SystemApplication();
			~SystemApplication() override;

		private:
			bool OnInit() override;
			void OnExit() override;
			void OnError(LogEvent& event);

			void OnGlobalConfigChanged(AppOption& option);
			void OnInstanceConfigChanged(AppOption& option, IGameInstance& instance);
			void OnProfileConfigChanged(AppOption& option, IGameProfile& profile);

			auto& GetGameInstanceTemplates()
			{
				return m_GameInstanceTemplates;
			}
			auto& GetShallowGameInstances()
			{
				return m_ShallowGameInstances;
			}
			IGameInstance* GetActiveGameInstance();
			void AssignActiveGameInstance(std::unique_ptr<IGameInstance> instance);

			bool OnException();
			bool OnMainLoopException() override;
			void OnUnhandledException() override;
			void OnFatalException() override;
			kxf::String RethrowCatchAndGetExceptionInfo() const;

		public:
			bool IsAnotherRunning() const;
			void ConfigureForInternetExplorer10(bool init) const;
			bool QueueDownloadToMainProcess(const kxf::String& link) const;
			
			kxf::String GetShortName() const;
			kxf::String GetRootFolder() const
			{
				return m_RootFolder;
			}
			kxf::String GetExecutablePath() const
			{
				return m_ExecutablePath;
			}
			kxf::String GetExecutableName() const
			{
				return m_ExecutableName;
			}

			kxf::XMLDocument& GetGlobalConfig()
			{
				return m_GlobalConfig;
			}
			IThemeManager& GetThemeManager() const
			{
				return *m_ThemeManager;
			}
			BroadcastProcessor& GetBroadcastProcessor()
			{
				return m_BroadcastProcessor;
			}
			wxLog* GetLogger() const;
			void CleanupLogs();
	};
}
