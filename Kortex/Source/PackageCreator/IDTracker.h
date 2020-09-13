#pragma once
#include <Kortex/Kortex.hpp>
#include "PackageProject/ComponentsSection.h"
#include "PackageProject/ModPackageProject.h"

namespace Kortex::PackageDesigner
{
	class IDTracker
	{
		protected:
			void TrackID_ReplaceOrRemove(const kxf::String& trackedID, const kxf::String& newID, KxStringVector& list, bool remove) const
			{
				if (remove)
				{
					TrackID_RemoveFromStringVector(trackedID, list);
				}
				else
				{
					TrackID_ReplaceInStringVector(trackedID, newID, list);
				}
			}
			void TrackID_ReplaceInStringVector(const kxf::String& trackedID, const kxf::String& newID, KxStringVector& list) const;
			void TrackID_RemoveFromStringVector(const kxf::String& trackedID, KxStringVector& list) const;

			void TrackID_ReplaceOrRemove(const kxf::String& trackedID, const kxf::String& newID, PackageProject::FlagItem::Vector& list, bool remove) const
			{
				if (remove)
				{
					TrackID_RemoveFromFlagVector(trackedID, list);
				}
				else
				{
					TrackID_ReplaceInFlagVector(trackedID, newID, list);
				}
			}
			void TrackID_ReplaceInFlagVector(const kxf::String& trackedID, const kxf::String& newID, PackageProject::FlagItem::Vector& list) const;
			void TrackID_RemoveFromFlagVector(const kxf::String& trackedID, PackageProject::FlagItem::Vector& list) const;

		protected:
			virtual bool TrackChangeID(const kxf::String& trackedID, const kxf::String& newID) = 0;
			virtual bool TrackRemoveID(const kxf::String& trackedID) = 0;
			virtual bool TrackAddID(const kxf::String& trackedID)
			{
				return false;
			}
	};
}
