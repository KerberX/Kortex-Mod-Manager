#pragma once
#include <Kortex/Kortex.hpp>
class KxURI;

namespace Kortex::Utility
{
	bool CopyTextToClipboard(const kxf::String& text);

	// Returns numerical architecture value: 32, 64.
	inline kxf::String ArchitectureToNumber(bool is64Bit)
	{
		return is64Bit ? wxS("64") : wxS("32");
	}
	
	// Returns string architecture value: x86, x64.
	inline kxf::String ArchitectureToString(bool is64Bit)
	{
		return is64Bit ? wxS("x64") : wxS("x86");
	}

	// Canonical ratio of a given size such as '16:9' for 1920x1080 or an empty string if the ratio can not be determined.
	kxf::String GetResolutionRatio(const kxf::Size& resolution);

	// Checks if extension matches one of masks (* and ? symbols are supported). Masks in 'extensions' should be without dot and in lowercase.
	bool FileExtensionMatches(const kxf::String& filePath, const KxStringVector& extensions);
	bool SingleFileExtensionMatches(const kxf::String& filePath, const kxf::String& ext);

	// Creates filter for 'KxFileBrowseDialog' from array of extensions (without dot).
	kxf::String MakeExtensionsFilter(const KxStringVector& extensions);

	// Checks is specified string contains chars forbidden by the file system.
	bool HasForbiddenFileNameChars(const kxf::String& string);

	// Removes all chars forbidden by file system from specified string.
	kxf::String MakeSafeFileName(const kxf::String& string);

	// Creates label enclosed in specified symbols.
	// MakeNoneLabel creates label with following format: <None>
	kxf::String MakeBracketedLabel(wxStandardID id, const wxUniChar& cLeft = wxS('<'), const wxUniChar& cRight = wxS('>'));
	kxf::String MakeBracketedLabel(KxStandardID id, const wxUniChar& cLeft = wxS('<'), const wxUniChar& cRight = wxS('>'));
	kxf::String MakeBracketedLabel(const kxf::String& text, const wxUniChar& cLeft = wxS('<'), const wxUniChar& cRight = wxS('>'));
	inline kxf::String MakeNoneLabel()
	{
		return MakeBracketedLabel(wxID_NONE, wxS('<'), wxS('>'));
	}

	template<class TItems>
	TItems ExpandVariablesInVector(const TItems& items)
	{
		TItems newItems;
		newItems.reserve(items.size());
		for (const auto& item: items)
		{
			newItems.emplace_back(KVarExp(item));
		}
		return newItems;
	}
}
