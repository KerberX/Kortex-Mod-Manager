#pragma once
#include <Kortex/Kortex.hpp>
#include "GameInstance/GameID.h"
#include "Resources/IImageProvider.h"
#include "Options/Option.h"
#include <KxFramework/KxXML.h>
#include <KxFramework/KxSingleton.h>
#include <KxFramework/KxTranslation.h>
#include <KxFramework/kxf::Version.h>
#include <KxFramework/KxURI.h>
class kxf::ImageList;
class kxf::ImageSet;

namespace Kortex
{
	class LogEvent;
	class SystemApplication;
	class BroadcastProcessor;
	class IVariableTable;
	class IGameInstance;
	class IGameProfile;
	class ITranslator;
}
namespace Kortex
{
	struct CmdLineParameters;
}

namespace Kortex
{
	enum class LoadTranslationStatus
	{
		Success = 0,
		NoTranslations,
		LoadingError
	};
}

namespace Kortex
{
	class IApplication:
		public KxSingletonPtr<IApplication>,
		public Application::WithOptions<IApplication>
	{
		friend class SystemApplication;

		public:
			static SystemApplication* GetSystemApp();

		private:
			void OnConfigureCommandLine();

		protected:
			virtual void OnCreate() = 0;
			virtual void OnDestroy() = 0;
			virtual bool OnInit() = 0;
			virtual int OnExit() = 0;

			virtual void OnError(LogEvent& event) = 0;
			virtual bool OnException() = 0;
			kxf::String RethrowCatchAndGetExceptionInfo() const;

			virtual void OnGlobalConfigChanged(AppOption& option) = 0;
			virtual void OnInstanceConfigChanged(AppOption& option, IGameInstance& instance) = 0;
			virtual void OnProfileConfigChanged(AppOption& option, IGameProfile& profile) = 0;

		public:
			kxf::String GetRootFolder() const;
			kxf::String GetExecutablePath() const;
			kxf::String GetExecutableName() const;

			virtual kxf::String GetDataFolder() const = 0;
			virtual kxf::String GetLogsFolder() const = 0;
			virtual kxf::String GetUserSettingsFolder() const = 0;
			virtual kxf::String GetUserSettingsFile() const = 0;
			virtual kxf::String GetInstancesFolder() const = 0;
			virtual kxf::String GetStartupInstanceID() const = 0;

			virtual bool IsTranslationLoaded() const = 0;
			virtual const KxTranslation& GetTranslation() const = 0;
			virtual const ITranslator& GetTranslator() const = 0;
			virtual KxTranslation::AvailableMap GetAvailableTranslations() const = 0;

			virtual const IImageProvider& GetImageProvider() const = 0;

			virtual IVariableTable& GetVariables() = 0;
			virtual kxf::String ExpandVariablesLocally(const kxf::String& variables) const = 0;
			virtual kxf::String ExpandVariables(const kxf::String& variables) const = 0;
			
			virtual bool OpenInstanceSelectionDialog() = 0;
			virtual bool Uninstall() = 0;

		public:
			bool Is64Bit() const;
			bool IsSystem64Bit() const;
			bool IsAnotherRunning() const;

			bool QueueDownloadToMainProcess(const kxf::String& link);
			std::optional<kxf::String> GetLinkFromCommandLine() const;

			wxCmdLineParser& GetCmdLineParser() const;
			kxf::String FormatCommandLine(const CmdLineParameters& parameters);
			bool ScheduleRestart(const kxf::String& commandLine = {}, std::optional<wxTimeSpan> timeout = {});

			void EnableIE10Support();
			void DisableIE10Support();

			kxf::String GetID() const;
			kxf::String GetName() const;
			kxf::String GetShortName() const;
			kxf::String GetDeveloper() const;
			kxf::Version GetVersion() const;
			kxf::Version GetWxWidgetsVersion() const;
			kxf::XMLDocument& GetGlobalConfig() const;
			IModule& GetModule() const;

			wxWindow* GetActiveWindow() const;
			wxWindow* GetTopWindow() const;
			void SetTopWindow(wxWindow* window);
			bool IsActive() const;
			bool IsMainWindowActive() const;

			void ExitApp(int exitCode = 0);
			wxLog& GetLogger();
			BroadcastProcessor& GetBroadcastProcessor();
			LoadTranslationStatus TryLoadTranslation(KxTranslation& translation,
													 const KxTranslation::AvailableMap& availableTranslations,
													 const kxf::String& component,
													 const kxf::String& desiredLocale = wxEmptyString
			) const;
	};
}
