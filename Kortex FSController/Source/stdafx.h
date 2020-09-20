#pragma once
#pragma comment(lib, "delayimp")

/* kxf::UI::Framework */
#include <kxf::UI::Framework/kxf::UI::Framework.h>
#include <kxf::UI::Framework/KxWinUndef.h>

#if _WIN64
#pragma comment(lib, "Bin/kxf::UI::Framework x64.lib")
#else
#pragma comment(lib, "Bin/kxf::UI::Framework.lib")
#endif

// C++
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <stack>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <variant>
#include <optional>
#include <locale>
#include <limits>
#include <type_traits>
