#pragma once
#include <Kortex/Kortex.hpp>
#include "ProjectSection.h"
#include "Utility/LabeledValue.h"
#include "Network/ModSourceStore.h"
#include "GameMods/ModTagStore.h"
#include "GameMods/TagManager/DefaultTag.h"

namespace Kortex::PackageProject
{
	class InfoSection: public ProjectSection
	{
		private:
			kxf::String m_Name;
			kxf::String m_TranslatedName;
			kxf::String m_Version;
			kxf::String m_Author;
			kxf::String m_Translator;
			kxf::String m_Description;
			Utility::LabeledValue::Vector m_CustomFields;
			Utility::LabeledValue::Vector m_Documents;
			Utility::LabeledValue::Vector m_WebSites;
			ModSourceStore m_ModSourceStore;
			ModTagStore m_TagStore;
	
		public:
			InfoSection(ModPackageProject& project);
			virtual ~InfoSection();
	
		public:
			const kxf::String& GetName() const
			{
				return m_Name;
			}
			void SetName(const kxf::String& value)
			{
				m_Name = value;
			}
			
			const kxf::String& GetTranslatedName() const
			{
				return m_TranslatedName;
			}
			void SetTranslatedName(const kxf::String& value)
			{
				m_TranslatedName = value;
			}
			
			const kxf::String& GetVersion() const
			{
				return m_Version;
			}
			void SetVersion(const kxf::String& value)
			{
				m_Version = value;
			}
			
			const kxf::String& GetAuthor() const
			{
				return m_Author;
			}
			void SetAuthor(const kxf::String& value)
			{
				m_Author = value;
			}
			
			const kxf::String& GetTranslator() const
			{
				return m_Translator;
			}
			void SetTranslator(const kxf::String& value)
			{
				m_Translator = value;
			}
			
			const kxf::String& GetDescription() const
			{
				return m_Description;
			}
			void SetDescription(const kxf::String& value)
			{
				m_Description = value;
			}
			
			const Utility::LabeledValue::Vector& GetCustomFields() const
			{
				return m_CustomFields;
			}
			Utility::LabeledValue::Vector& GetCustomFields()
			{
				return m_CustomFields;
			}
			
			const Utility::LabeledValue::Vector& GetDocuments() const
			{
				return m_Documents;
			}
			Utility::LabeledValue::Vector& GetDocuments()
			{
				return m_Documents;
			}
			
			const ModSourceStore& GetModSourceStore() const
			{
				return m_ModSourceStore;
			}
			ModSourceStore& GetModSourceStore()
			{
				return m_ModSourceStore;
			}
			
			const ModTagStore& GetTagStore() const
			{
				return m_TagStore;
			}
			ModTagStore& GetTagStore()
			{
				return m_TagStore;
			}
	};
}
