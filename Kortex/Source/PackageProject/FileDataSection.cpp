#include "stdafx.h"
#include "FileDataSection.h"
#include "ModPackageProject.h"
#include "Utility/Common.h"
#include <Kortex/Application.hpp>
#include <kxf::UI::Framework/KxString.h>

namespace Kortex::PackageProject
{
	FileItem::FileItem()
		:m_Priority(FileDataSection::ms_DefaultPriority)
	{
	}
	
	void FileItem::MakeUniqueID()
	{
		if (m_ID.IsEmpty())
		{
			m_ID = kxf::String::Format("0x%1", this);
		}
		else
		{
			m_ID = kxf::String::Format("%s!0x%1", this);
		}
	}
	
	bool FileItem::IsDefaultPriority() const
	{
		return m_Priority == FileDataSection::ms_DefaultPriority;
	}
	int32_t FileItem::GetPriority() const
	{
		return m_Priority;
	}
	void FileItem::SetPriority(int32_t value)
	{
		m_Priority = FileDataSection::CorrectPriority(value);
	}
}

namespace Kortex::PackageProject
{
	bool FileDataSection::IsPriorityValid(int32_t value)
	{
		return value >= FileDataSection::ms_MinUserPriority && value <= FileDataSection::ms_MaxUserPriority;
	}
	int32_t FileDataSection::CorrectPriority(int32_t value)
	{
		return std::clamp(value, ms_MinUserPriority, ms_MaxUserPriority);
	}
	bool FileDataSection::IsFileIDValid(const kxf::String& id)
	{
		if (!id.IsEmpty() && !Utility::HasForbiddenFileNameChars(id))
		{
			kxf::String idLower = KxString::ToLower(id);
			return idLower != wxS("fomod");
		}
		return false;
	}
	
	FileDataSection::FileDataSection(ModPackageProject& project)
		:ProjectSection(project)
	{
	}
	
	FileItem* FileDataSection::FindItemWithID(const kxf::String& id, size_t* index) const
	{
		const kxf::String idLower = KxString::ToLower(id);
		auto it = std::find_if(m_Items.begin(), m_Items.end(), [&idLower](const auto& entry)
		{
			return KxString::ToLower(entry->GetID()) == idLower;
		});
	
		if (it != m_Items.end())
		{
			if (index)
			{
				*index = std::distance(m_Items.begin(), it);
			}
			return it->get();
		}
		return nullptr;
	}
}
