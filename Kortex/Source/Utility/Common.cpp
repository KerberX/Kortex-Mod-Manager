#include "stdafx.h"
#include "Common.h"
#include <KxFramework/KxURI.h>
#include <KxFramework/KxCallAtScopeExit.h>
#include <wx/clipbrd.h>

namespace
{
	kxf::String DoMakeBracketedLabelFromString(const kxf::String& text, const wxUniChar& cLeft, const wxUniChar& cRight)
	{
		return kxf::String::Format(wxS("%1%2%3"), cLeft, text, cRight);
	}
	kxf::String DoMakeBracketedLabelFromInt(int id, const wxUniChar& cLeft, const wxUniChar& cRight)
	{
		return DoMakeBracketedLabelFromString(Kortex::Translate(static_cast<KxStandardID>(id)), cLeft, cRight);
	}
}

namespace Kortex::Utility
{
	bool CopyTextToClipboard(const kxf::String& text)
	{
		if (wxTheClipboard->Open())
		{
			KxCallAtScopeExit atExit([]()
			{
				wxTheClipboard->Close();
			});
			return wxTheClipboard->SetData(new wxTextDataObject(text));
		}
		return false;
	}

	kxf::String GetResolutionRatio(const wxSize& resolution)
	{
		auto MakeRatio = [](double x, double y) constexpr -> int
		{
			return ((x / y) * 100) + 0.1;
		};

		constexpr int r3_2 = MakeRatio(3, 2);
		constexpr int r4_3 = MakeRatio(4, 3);
		constexpr int r5_3 = MakeRatio(5, 3);
		constexpr int r5_4 = MakeRatio(5, 4);
		constexpr int r11_4 = MakeRatio(11, 4);
		constexpr int r14_9 = MakeRatio(14, 9);
		constexpr int r16_9 = MakeRatio(16, 9);
		constexpr int r16_10 = MakeRatio(16, 10);
		constexpr int r17_8 = MakeRatio(17, 8) + 1; // Because 17:8 is 212 and I need 213
		constexpr int r18_9 = MakeRatio(18, 9);
		constexpr int r21_9 = MakeRatio(21, 9);

		switch (MakeRatio(resolution.GetWidth(), resolution.GetHeight()))
		{
			case r3_2:
			{
				return "3:2";
			}
			case r4_3:
			{
				return "4:3";
			}
			case r5_4:
			{
				return "5:4";
			}
			case r5_3:
			{
				return "5:3";
			}
			case r11_4:
			{
				return "11:4";
			}
			case r14_9:
			{
				return "14:9";
			}
			case r16_9:
			{
				return "16:9";
			}
			case r16_10:
			{
				return "16:10";
			}
			case r17_8:
			{
				return "17:8";
			}
			case r18_9:
			{
				return "18:9";
			}
			case r21_9:
			{
				return "21:9";
			}
		};
		return {};
	}

	bool FileExtensionMatches(const kxf::String& filePath, const KxStringVector& extensions)
	{
		kxf::String ext = KxString::ToLower(filePath.AfterLast('.'));
		for (const kxf::String& mask: extensions)
		{
			if (ext.Matches(mask))
			{
				return true;
			}
		}
		return false;
	}
	bool SingleFileExtensionMatches(const kxf::String& filePath, const kxf::String& ext)
	{
		kxf::String fileExt = KxString::ToLower(filePath.AfterLast('.'));
		return fileExt.Matches(ext);
	}
	kxf::String MakeExtensionsFilter(const KxStringVector& extensions)
	{
		return wxS("*.") + KxString::Join(extensions, wxS(";*."));
	}
	bool HasForbiddenFileNameChars(const kxf::String& string)
	{
		for (wxChar c: wxFileName::GetForbiddenChars())
		{
			if (string.Contains(c))
			{
				return true;
			}
		}
		return false;
	}
	kxf::String MakeSafeFileName(const kxf::String& string)
	{
		kxf::String out = string;
		for (wxChar c: wxFileName::GetForbiddenChars())
		{
			out.Replace(c, wxS("_"), true);
		}
		return out;
	}

	kxf::String MakeBracketedLabel(KxStandardID id, const wxUniChar& cLeft, const wxUniChar& cRight)
	{
		return DoMakeBracketedLabelFromInt(id, cLeft, cRight);
	}
	kxf::String MakeBracketedLabel(wxStandardID id, const wxUniChar& cLeft, const wxUniChar& cRight)
	{
		return DoMakeBracketedLabelFromInt(id, cLeft, cRight);
	}
	kxf::String MakeBracketedLabel(const kxf::String& text, const wxUniChar& cLeft, const wxUniChar& cRight)
	{
		return DoMakeBracketedLabelFromString(text, cLeft, cRight);
	}
}
