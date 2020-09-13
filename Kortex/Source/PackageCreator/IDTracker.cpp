#include "stdafx.h"
#include "IDTracker.h"

namespace Kortex::PackageDesigner
{
	void IDTracker::TrackID_ReplaceInStringVector(const kxf::String& trackedID, const kxf::String& newID, KxStringVector& list) const
	{
		for (auto it = list.begin(); it != list.end(); ++it)
		{
			if (*it == trackedID)
			{
				*it = newID;
				break;
			}
		}
	}
	void IDTracker::TrackID_RemoveFromStringVector(const kxf::String& trackedID, KxStringVector& list) const
	{
		auto it = std::find(list.begin(), list.end(), trackedID);
		if (it != list.end())
		{
			list.erase(it);
		}
	}
	
	void IDTracker::TrackID_ReplaceInFlagVector(const kxf::String& trackedID, const kxf::String& newID, PackageProject::FlagItem::Vector& list) const
	{
		for (auto it = list.begin(); it != list.end(); ++it)
		{
			if (it->GetName() == trackedID)
			{
				it->SetName(newID);
				break;
			}
		}
	}
	void IDTracker::TrackID_RemoveFromFlagVector(const kxf::String& trackedID, PackageProject::FlagItem::Vector& list) const
	{
		auto it = std::find_if(list.begin(), list.end(), [&trackedID](const PackageProject::FlagItem& flagEntry)
		{
			return flagEntry.GetName() == trackedID;
		});
		if (it != list.end())
		{
			list.erase(it);
		}
	}
}
