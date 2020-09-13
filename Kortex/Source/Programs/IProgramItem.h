#pragma once
#include <Kortex/Kortex.hpp>
#include "Utility/WithBitmap.h"
#include <KxFramework/KxXML.h>

namespace Kortex
{
	class IProgramItem
	{
		public:
			using Vector = std::vector<std::unique_ptr<IProgramItem>>;
			using RefVector = std::vector<IProgramItem*>;

		public:
			virtual ~IProgramItem() = default;

		public:
			virtual bool IsOK() const = 0;
			virtual void Load(const kxf::XMLNode& node) = 0;
			virtual void Save(kxf::XMLNode& node) const = 0;

			virtual bool RequiresVFS() const = 0;
			virtual bool CanRunNow() const = 0;
			virtual void OnRun() = 0;

			virtual bool ShouldShowInMainMenu() const = 0;
			virtual void ShowInMainMenu(bool value) = 0;

			virtual kxf::String RawGetName() const
			{
				return GetName();
			}
			virtual kxf::String GetName() const = 0;
			virtual void SetName(const kxf::String& value) = 0;
			
			virtual kxf::String RawGetIconPath() const
			{
				return GetIconPath();
			}
			virtual kxf::String GetIconPath() const = 0;
			virtual void SetIconPath(const kxf::String& value) = 0;
			
			virtual kxf::String RawGetExecutable() const
			{
				return GetExecutable();
			}
			virtual kxf::String GetExecutable() const = 0;
			virtual void SetExecutable(const kxf::String& value) = 0;

			virtual kxf::String RawGetArguments() const
			{
				return GetArguments();
			}
			virtual kxf::String GetArguments() const = 0;
			virtual void SetArguments(const kxf::String& value) = 0;

			virtual kxf::String RawGetWorkingDirectory() const
			{
				return GetWorkingDirectory();
			}
			virtual kxf::String GetWorkingDirectory() const = 0;
			virtual void SetWorkingDirectory(const kxf::String& value) = 0;

			virtual Utility::WithBitmap& GetSmallBitmap() = 0;
			const Utility::WithBitmap& GetSmallBitmap() const
			{
				return const_cast<IProgramItem&>(*this).GetSmallBitmap();
			}

			virtual Utility::WithBitmap& GetLargeBitmap() = 0;
			const Utility::WithBitmap& GetLargeBitmap() const
			{
				return const_cast<IProgramItem&>(*this).GetLargeBitmap();
			}
	};
}
