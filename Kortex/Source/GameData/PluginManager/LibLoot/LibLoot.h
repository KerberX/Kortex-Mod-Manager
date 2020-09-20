#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf::UI::Framework/KxSingleton.h>
#include <kxf::UI::Framework/kxf::Version.h>

namespace Kortex::Utility
{
	class OperationWithProgressDialogBase;
}

namespace Kortex::PluginManager
{
	class LibLoot: public kxf::SingletonPtr<LibLoot>
	{
		public:
			static kxf::String GetLibraryName();
			static kxf::Version GetLibraryVersion();

		public:
			LibLoot() = default;

		public:
			kxf::String GetDataPath() const;
			kxf::String GetMasterListPath() const;
			kxf::String GetUserListPath() const;

			bool CanSortNow() const;
			bool SortPlugins(KxStringVector& sortedList, Utility::OperationWithProgressDialogBase* context = nullptr);
	};
}
