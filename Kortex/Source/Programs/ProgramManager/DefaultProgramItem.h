#pragma once
#include <Kortex/Kortex.hpp>
#include "Programs/IProgramItem.h"

namespace Kortex::ProgramManager
{
	class DefaultProgramItem: public IProgramItem
	{
		private:
			Utility::WithBitmap m_SmallBitmap;
			Utility::WithBitmap m_LargeBitmap;

			kxf::String m_Name;
			kxf::String m_IconPath;
			kxf::String m_Executable;
			kxf::String m_Arguments;
			kxf::String m_WorkingDirectory;

			bool m_ShowInMainMenu = false;

		public:
			bool IsOK() const override
			{
				return !m_Name.IsEmpty() && !m_Executable.IsEmpty();
			}
			void Load(const kxf::XMLNode& node) override;
			void Save(kxf::XMLNode& node) const override;
		
			bool RequiresVFS() const override;
			bool CanRunNow() const override;
			void OnRun() override;

			bool ShouldShowInMainMenu() const override
			{
				return m_ShowInMainMenu;
			}
			void ShowInMainMenu(bool value) override
			{
				m_ShowInMainMenu = value;
			}

			kxf::String RawGetName() const override
			{
				return m_Name;
			}
			kxf::String GetName() const override;
			void SetName(const kxf::String& value) override
			{
				m_Name = value;
			}
		
			kxf::String RawGetIconPath() const override
			{
				return m_IconPath;
			}
			kxf::String GetIconPath() const override;
			void SetIconPath(const kxf::String& value) override
			{
				m_IconPath = value;
			}
		
			kxf::String RawGetExecutable() const override
			{
				return m_Executable;
			}
			kxf::String GetExecutable() const override;
			void SetExecutable(const kxf::String& value) override
			{
				m_Executable = value;
			}
		
			kxf::String RawGetArguments() const override
			{
				return m_Arguments;
			}
			kxf::String GetArguments() const override;
			void SetArguments(const kxf::String& value) override
			{
				m_Arguments = value;
			}

			kxf::String RawGetWorkingDirectory() const override
			{
				return m_WorkingDirectory;
			}
			kxf::String GetWorkingDirectory() const override;
			void SetWorkingDirectory(const kxf::String& value) override
			{
				m_WorkingDirectory = value;
			}

			Utility::WithBitmap& GetSmallBitmap() override
			{
				return m_SmallBitmap;
			}
			Utility::WithBitmap& GetLargeBitmap() override
			{
				return m_LargeBitmap;
			}
	};
}
