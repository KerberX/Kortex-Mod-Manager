#include "stdafx.h"
#include "DefaultProgramItem.h"
#include "DefaultProgramManager.h"
#include <Kortex/GameInstance.hpp>
#include <Kortex/ModManager.hpp>
#include <KxFramework/KxFile.h>
#include <KxFramework/KxXML.h>

namespace
{
	using namespace Kortex;
	using namespace Kortex::ModManager;

	enum class Mode
	{
		ToReal,
		ToVirtual
	};

	bool IsRelative(const kxf::String& path)
	{
		return !(path.length() >= 2 && path[1] == wxS(':'));
	}
	kxf::String ResolvePath(const kxf::String& path, Mode mode = Mode::ToReal, bool* isRelative = nullptr)
	{
		if (!path.IsEmpty())
		{
			kxf::String pathExp = KVarExp(path);
			if (IsRelative(pathExp))
			{
				KxUtility::SetIfNotNull(isRelative, true);
				if (mode == Mode::ToVirtual)
				{
					if (IGameInstance* instnace = IGameInstance::GetActive())
					{
						return instnace->GetVirtualGameDir() + wxS('\\') + pathExp;
					}
				}
				else
				{
					if (IModDispatcher* dispatcher = IModDispatcher::GetInstance())
					{
						const FileTreeNode* node = dispatcher->ResolveLocation(pathExp);
						if (node)
						{
							return node->GetFullPath();
						}
					}
				}
				return wxEmptyString;
			}

			KxUtility::SetIfNotNull(isRelative, false);
			return pathExp;
		}
		else
		{
			KxUtility::SetIfNotNull(isRelative, true);
			return wxEmptyString;
		}
	}
}

namespace Kortex::ProgramManager
{
	void DefaultProgramItem::Load(const kxf::XMLNode& node)
	{
		m_ShowInMainMenu = node.GetAttributeBool("ShowInMainMenu");

		m_Name = node.GetFirstChildElement("Name").GetValue();
		m_Executable = node.GetFirstChildElement("Executable").GetValue();
		m_IconPath = node.GetFirstChildElement("Icon").GetValue();
		m_Arguments = node.GetFirstChildElement("Arguments").GetValue();
		m_WorkingDirectory = node.GetFirstChildElement("WorkingDirectory").GetValue();
	}
	void DefaultProgramItem::Save(kxf::XMLNode& node) const
	{
		node.SetAttribute("ShowInMainMenu", m_ShowInMainMenu);

		node.NewElement("Name").SetValue(m_Name);
		node.NewElement("Executable").SetValue(m_Executable);

		if (!m_IconPath.IsEmpty())
		{
			node.NewElement("Icon").SetValue(m_IconPath);
		}
		if (!m_Arguments.IsEmpty())
		{
			node.NewElement("Arguments").SetValue(m_Arguments);
		}
		if (!m_WorkingDirectory.IsEmpty())
		{
			node.NewElement("WorkingDirectory").SetValue(m_WorkingDirectory);
		}
	}

	bool DefaultProgramItem::RequiresVFS() const
	{
		bool isRelative = false;
		ResolvePath(m_Executable, Mode::ToVirtual, &isRelative);
		return isRelative;
	}
	bool DefaultProgramItem::CanRunNow() const
	{
		if (IModManager* manager = IModManager::GetInstance())
		{
			if (!RequiresVFS() || manager->GetFileSystem().IsEnabled())
			{
				return KxFile(ResolvePath(m_Executable, Mode::ToVirtual)).IsFileExist();
			}
		}
		return false;
	}

	kxf::String DefaultProgramItem::GetName() const
	{
		if (m_Name.IsEmpty())
		{
			return KVarExp(m_Executable).AfterLast(wxS('\\'));
		}
		return KVarExp(m_Name);
	}
	kxf::String DefaultProgramItem::GetIconPath() const
	{
		kxf::String path = ResolvePath(m_IconPath, Mode::ToReal);
		if (path.IsEmpty())
		{
			path = ResolvePath(m_Executable, Mode::ToReal);
		}
		return path;
	}
	kxf::String DefaultProgramItem::GetExecutable() const
	{
		return ResolvePath(m_Executable, Mode::ToVirtual);
	}
	kxf::String DefaultProgramItem::GetArguments() const
	{
		return KVarExp(m_Arguments);
	}
	kxf::String DefaultProgramItem::GetWorkingDirectory() const
	{
		return ResolvePath(m_WorkingDirectory, Mode::ToVirtual);
	}

	void DefaultProgramItem::OnRun()
	{
		// Write statistics or something
	}
}
