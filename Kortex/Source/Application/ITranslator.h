#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf/General/StringFormatter.h>

namespace Kortex
{
	class IGameInstance;

	class ITranslator
	{
		public:
			static const ITranslator& GetAppTranslator();

			static kxf::String GetVariable(const kxf::String& variable, const kxf::String& variableNamespace = {});
			static kxf::String GetVariable(const IGameInstance& instance, const kxf::String& variable, const kxf::String& variableNamespace = {});
			static kxf::String ExpandVariables(const kxf::String& variables);
			static kxf::String ExpandVariables(const IGameInstance& instance, const kxf::String& variables);

		private:
			template<class T>
			kxf::String ConstructTranslationVar(const T& id) const
			{
				return kxf::String::Format(wxS("$T(%1)"), id);
			}

		protected:
			virtual std::optional<kxf::String> DoGetString(const kxf::String& id) const = 0;
			virtual std::optional<kxf::String> DoGetString(kxf::StdID id) const = 0;
			std::optional<kxf::String> DoGetString(wxStandardID id) const
			{
				return DoGetString(static_cast<KxStandardID>(id));
			}
			
			template<class T>
			std::optional<kxf::String> FetchString(const T& id) const
			{
				OpString value = DoGetString(id);
				if (value)
				{
					return ExpandVariables(*value);
				}
				return std::nullopt;
			}
			
			template<class T>
			std::optional<kxf::String> FetchString(const IGameInstance& instance, const T& id) const
			{
				OpString value = DoGetString(id);
				if (value)
				{
					return ExpandVariables(instance, *value);
				}
				return std::nullopt;
			}

		public:
			virtual ~ITranslator() = default;

		public:
			template<class T>
			kxf::String GetString(const T& id) const
			{
				OpString value = FetchString(id);
				return value ? *value : ConstructTranslationVar(id);
			}
			
			template<class T>
			kxf::String GetString(const IGameInstance& instance, const T& id) const
			{
				OpString value = FetchString(instance, id);
				return value ? *value : ConstructTranslationVar(id);
			}

			template<class T>
			std::optional<kxf::String> TryGetString(const T& id) const
			{
				return FetchString(id);
			}
			
			template<class T>
			std::optional<kxf::String> TryGetString(const IGameInstance& instance, const T& id) const
			{
				return FetchString(instance, id);
			}

			template<class T, class... Args>
			kxf::String FormatString(const T& id, Args&&... arg) const
			{
				return kxf::String::Format(GetString(id), std::forward<Args>(arg)...);
			}

			template<class T, class... Args>
			kxf::String FormatString(const IGameInstance& instance, const T& id, Args&&... arg) const
			{
				return kxf::String::Format(GetString(instance, id), std::forward<Args>(arg)...);
			}

			template<class T, class... Args>
			std::optional<kxf::String> TryFormatString(const T& id, Args&&... arg) const
			{
				OpString value = TryGetString(id);
				if (value)
				{
					value = kxf::String::Format(*value, std::forward<Args>(arg)...);
				}
				return value;
			}
			
			template<class T, class... Args>
			kxf::String TryFormatString(const IGameInstance& instance, const T& id, Args&&... arg) const
			{
				OpString value = TryGetString(instance, id);
				if (value)
				{
					value = kxf::String::Format(*value, std::forward<Args>(arg)...);
				}
				return value;
			}
	};
}
