#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/AppOption.h"
#include <kxf::UI::Framework/KxString.h>

namespace Kortex::NetworkManager::NXMHandler
{
	class OptionStore
	{
		public:
			struct Instance
			{
				kxf::String ID;

				explicit operator bool() const
				{
					return !ID.IsEmpty();
				}
				bool operator!() const
				{
					return ID.IsEmpty();
				}
			};
			struct Command
			{
				kxf::String Executable;
				kxf::String Arguments;

				explicit operator bool() const
				{
					return !Executable.IsEmpty();
				}
				bool operator!() const
				{
					return Executable.IsEmpty();
				}
			};

		private:
			template<class T, class TItems> static T* DoGetOption(TItems&& items, const kxf::String& nexusID)
			{
				auto it = items.find(KxString::ToLower(nexusID));
				if (it !=  items.end())
				{
					using T2 = std::remove_cv_t<T>;
					if (auto value = std::get_if<T2>(&it->second))
					{
						return value;
					}
					else if (auto value = std::get_if<T2>(&it->second))
					{
						return value;
					}
				}
				return nullptr;
			}

		private:
			std::unordered_map<kxf::String, std::variant<Instance, Command>> m_Options;

		public:
			void Save(AppOption& option) const;
			void Load(const AppOption& option);

			template<class T> const T* GetOption(const kxf::String& nexusID) const;
			template<> const Instance* GetOption(const kxf::String& nexusID) const
			{
				return DoGetOption<const Instance>(m_Options, nexusID);
			}
			template<> const Command* GetOption(const kxf::String& nexusID) const
			{
				return DoGetOption<const Command>(m_Options, nexusID);
			}

			void SetOption(const kxf::String& nexusID, const Instance& value)
			{
				m_Options.insert_or_assign(KxString::ToLower(nexusID), value);
			}
			void SetOption(const kxf::String& nexusID, const Command& value)
			{
				m_Options.insert_or_assign(KxString::ToLower(nexusID), value);
			}
			void RemoveOption(const kxf::String& nexusID)
			{
				m_Options.erase(KxString::ToLower(nexusID));
			}
	};
}
