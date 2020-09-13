#pragma once
#include <kxf/Common.hpp>

// General
#include <kxf/General/Common.h>
#include <kxf/General/String.h>
#include "kxf/General/Color.h"
#include "kxf/General/Angle.h"
#include "kxf/General/StdID.h"
#include "kxf/General/Version.h"
#include "kxf/General/DateTime.h"
#include "kxf/General/ResourceID.h"
#include "kxf/General/OptionalPtr.h"

// Application
#include <kxf/Application/Common.h>
#include <kxf/Application/ICoreApplication.h>
#include <kxf/Application/IGUIApplication.h>

// EventSystem
#include <kxf/EventSystem/Common.h>
#include <kxf/EventSystem/IEvent.h>
#include <kxf/EventSystem/IEventLoop.h>
#include <kxf/EventSystem/IEvtHandler.h>

// FileSystem
#include <kxf/FileSystem/Common.h>
#include <kxf/FileSystem/FSPath.h>
#include <kxf/FileSystem/FileItem.h>
#include <kxf/FileSystem/IFileSystem.h>

// IO
#include <kxf/IO/Common.h>
#include <kxf/IO/StreamDelegate.h>
#include <kxf/IO/StreamWrappers.h>

// System
#include <kxf/System/Common.h>
#include <kxf/System/UndefWindows.h>

// Localization
#include <kxf/Localization/Common.h>
#include <kxf/Localization/ILocalizationPackage.h>

// RTTI
#include <kxf/RTTI/Common.h>
#include <kxf/RTTI/QueryInterface.h>

// Network
#include <kxf/Network/Common.h>
#include <kxf/Network/URI.h>

// Serialization
#include <kxf/Serialization/Common.h>

// Crypto
#include <kxf/Crypto/Common.h>

// Compression
#include <kxf/Compression/Common.h>

// Async
#include <kxf/Async/Common.h>

// Threading
#include <kxf/Threading/Common.h>

// Drawing
#include <kxf/Drawing/Common.h>
#include <kxf/Drawing/Geometry.h>

// UI
#include <kxf/UI/Common.h>
#include <kxf/UI/StdIcon.h>
#include <kxf/UI/StdButton.h>
#include <kxf/UI/WidgetID.h>

// Forward declarations
namespace kxf
{
	class XMLDocument;
	class XMLNode;
}
