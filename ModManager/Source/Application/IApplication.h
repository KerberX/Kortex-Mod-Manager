#pragma once
#include "Framework.hpp"
#include "Options/Option.h"
#include <kxf/General/Version.h>
#include <kxf/General/IVariablesCollection.h>
#include <kxf/Localization/Locale.h>
#include <kxf/Localization/ILocalizationPackage.h>
class wxLog;
class wxCmdLineParser;

namespace kxf
{
	class XMLDocument;
}

namespace Kortex
{
	class IModule;
	class IManager;
	class IGameInstance;
	class IGameProfile;
	class IMainWindow;

	class BroadcastProcessor;
}

namespace Kortex
{
	class IApplication: public kxf::RTTI::Interface<IApplication>, public Application::WithOptions<IApplication>
	{
		KxRTTI_DeclareIID(IApplication, {0xb5e8047c, 0x9239, 0x45c4, {0x86, 0xf6, 0x6c, 0x83, 0xa8, 0x42, 0x06, 0x3e}});

		friend class SystemApplication;
		friend class AppOption;

		public:
			static IApplication& GetInstance() noexcept;

		protected:
			virtual void OnCreate() = 0;
			virtual void OnDestroy() = 0;
			virtual bool OnInit() = 0;
			virtual int OnExit() = 0;
			virtual bool OnException() = 0;
			kxf::String ExamineCaughtException() const;

			virtual void OnGlobalConfigChanged(AppOption& option) = 0;
			virtual void OnInstanceConfigChanged(AppOption& option, IGameInstance& instance) = 0;
			virtual void OnProfileConfigChanged(AppOption& option, IGameProfile& profile) = 0;

		public:
			kxf::FSPath GetRootDirectory() const;
			virtual kxf::FSPath GetDataDirectory() const = 0;
			virtual kxf::FSPath GetLogsDirectory() const = 0;
			virtual kxf::FSPath GetSettingsDirectory() const = 0;
			virtual kxf::FSPath GetSettingsFile() const = 0;
			virtual kxf::FSPath GetInstancesDirectory() const = 0;
			virtual kxf::String GetStartupInstanceID() const = 0;
			virtual IMainWindow* GetMainWindow() const = 0;

			virtual size_t EnumLoadedModules(std::function<bool(IModule&)> func) = 0;
			virtual size_t EnumLoadedManagers(std::function<bool(IManager&)> func) = 0;

			virtual const kxf::ILocalizationPackage& GetLocalizationPackage() const = 0;
			virtual size_t EnumLocalizationPackages(std::function<bool(kxf::Locale, kxf::FileItem)> func) const = 0;

			virtual kxf::IVariablesCollection& GetVariables() = 0;
			virtual kxf::String ExpandVariables(const kxf::String& variables) const = 0;
			virtual kxf::String ExpandVariablesLocally(const kxf::String& variables) const = 0;

			virtual kxf::object_ptr<IGameInstance> GetActiveGameInstance() const = 0;
			
			virtual bool OpenInstanceSelectionDialog() = 0;
			virtual bool Uninstall() = 0;

		public:
			bool Is64Bit() const;
			bool IsSystem64Bit() const;
			bool IsAnotherInstanceRunning() const;

			kxf::String FormatCommandLine(const std::unordered_map<kxf::String, kxf::String>& parameters);
			bool ScheduleRestart(const kxf::String& commandLine = {}, std::optional<kxf::TimeSpan> timeout = {});
			void Exit(int exitCode = 0);

			kxf::String GetID() const;
			kxf::String GetName() const;
			kxf::String GetShortName() const;
			kxf::String GetDeveloper() const;
			kxf::Version GetVersion() const;
			kxf::XMLDocument& GetGlobalConfig() const;

			wxWindow* GetActiveWindow() const;
			wxWindow* GetTopWindow() const;
			void SetTopWindow(wxWindow* window);
			bool IsActive() const;
			bool IsMainWindowActive() const;

			wxLog& GetLogger();
			BroadcastProcessor& GetBroadcastProcessor();
	};
}