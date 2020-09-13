#pragma once
#include <Kortex/Kortex.hpp>
#include <KxFramework/KxString.h>

namespace Kortex::Utility::Log
{
	enum class Level
	{
		Info,
		Error,
		Trace,
		Debug,
		Status,
		Message,
		Warning,
		Progress,
		FatalError,
	};
}

namespace Kortex::Utility::Log
{
	bool IsEnabled();
	bool IsLevelEnabled(Level level);
	void LogString(Level level, const kxf::String& value);
}

namespace Kortex::Utility::Log
{
	namespace Internal
	{
		inline void LogIfEnabled(Level level, const kxf::String& format)
		{
			if (IsLevelEnabled(level))
			{
				LogString(level, format);
			}
		}
		template<class... Args> void LogIfEnabled(Level level, const kxf::String& format, Args&&... arg)
		{
			if (IsLevelEnabled(level))
			{
				LogString(level, kxf::String::Format(format, std::forward<Args>(arg)...));
			}
		}
	}

	template<class... Args> void LogInfo(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Info, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogError(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Error, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogTrace(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Trace, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogDebug(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Debug, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogStatus(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Status, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogMessage(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Message, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogWarning(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Warning, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogProgress(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::Progress, format, std::forward<Args>(arg)...);
	}
	template<class... Args> void LogFatalError(const kxf::String& format, Args&&... arg)
	{
		Internal::LogIfEnabled(Level::FatalError, format, std::forward<Args>(arg)...);
	}
}
