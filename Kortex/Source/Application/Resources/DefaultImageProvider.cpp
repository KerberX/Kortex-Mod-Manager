#include "stdafx.h"
#include "DefaultImageProvider.h"
#include "ImageResourceID.h"
#include "Utility/BitmapSize.h"
#include <kxf/System/ShellOperations.h>

namespace
{
	wxSize GetIconSIze()
	{
		return Kortex::Utility::BitmapSize().FromSystemSmallIcon();
	}
	int GetIconCount()
	{
		return static_cast<int>(Kortex::ImageResourceID::MAX_ELEMENT);
	}

	bool operator<(const wxSize& left, const wxSize& right)
	{
		return left.GetWidth() < right.GetWidth() || left.GetHeight() < right.GetHeight();
	}
}

namespace Kortex::Application
{
	void DefaultImageProvider::OnLoadBitmap(wxBitmap& bitmap)
	{
		if (bitmap.GetSize() < m_ImageList.GetSize())
		{
			wxSize targetSize = m_ImageList.GetSize();
			wxImage image = bitmap.ConvertToImage();
			image.Rescale(targetSize.GetWidth(), targetSize.GetHeight(), wxImageResizeQuality::wxIMAGE_QUALITY_NORMAL);
			bitmap = wxBitmap(image, 32);
		}
	}
	void DefaultImageProvider::LoadImages()
	{
		// Initialize image list with dummy images
		{
			wxIcon tempIcon = wxICON(IDS_ICON_APP);
			for (size_t i = 0; i < static_cast<int>(Imagekxf::ResourceID::MAX_ELEMENT); i++)
			{
				m_ImageList.Add(tempIcon);
			}
		}

		// Add empty transparent icon
		if (wxImage nullIcon(GetIconSIze(), true); true)
		{
			nullIcon.InitAlpha();
			memset(nullIcon.GetAlpha(), 0, (size_t)nullIcon.GetWidth() * (size_t)nullIcon.GetHeight());

			AddSingleItem(Imagekxf::ResourceID::Null, wxS("Null"), nullIcon);
		}

		// Unknown icon
		if (KxFileItem item(wxEmptyString); true)
		{
			item.SetNormalAttributes();
			item.SetName(wxS(".url"));

			AddSingleItem(Imagekxf::ResourceID::ModNetwork_Unknown, wxS("UnknownSite"), KxShell::GetFileIcon(item, true));
		}

		// App logos
		LoadItem(Imagekxf::ResourceID::KortexLogo, wxS("kortex-logo"));
		LoadItem(Imagekxf::ResourceID::KortexLogoIco, wxS("kortex-logo-icon"), Type::Icon);
		LoadItem(Imagekxf::ResourceID::KortexLogoSmall, wxS("kortex-logo-small"));

		LoadItem(Imagekxf::ResourceID::SKSMLogo, wxS("sksm-logo"));
		LoadItem(Imagekxf::ResourceID::SKSMLogoIco, wxS("sksm-logo-icon"), Type::Icon);
		LoadItem(Imagekxf::ResourceID::SKSMLogoSmall, wxS("sksm-logo-small"));

		// Mod networks icons
		LoadItem(Imagekxf::ResourceID::ModNetwork_TESALL, wxS("TESALL.RU"));
		LoadItem(Imagekxf::ResourceID::ModNetwork_Nexus, wxS("NexusMods"));
		LoadItem(Imagekxf::ResourceID::ModNetwork_LoversLab, wxS("LoverLab"));

		// Normal icons
		LoadItem(Imagekxf::ResourceID::SevenZip, wxS("7zip"));
		LoadItem(Imagekxf::ResourceID::SevenZipIco, wxS("7zip-icon"), Type::Icon);

		LoadItem(Imagekxf::ResourceID::MO2, wxS("MO2"));
		LoadItem(Imagekxf::ResourceID::MO2Ico, wxS("MO2-icon"), Type::Icon);

		LoadItem(Imagekxf::ResourceID::LOOT, wxS("LOOT"));
		LoadItem(Imagekxf::ResourceID::JSON, wxS("JSON"));
		LoadItem(Imagekxf::ResourceID::Dokany, wxS("Dokany"));
		LoadItem(Imagekxf::ResourceID::LibCURL, wxS("curl"));
		LoadItem(Imagekxf::ResourceID::WebSocket, wxS("websocket"));

		LoadItem(Imagekxf::ResourceID::Application, wxS("application"));
		LoadItem(Imagekxf::ResourceID::ApplicationDetail, wxS("application-detail"));
		LoadItem(Imagekxf::ResourceID::ApplicationProhibition, wxS("application-prohibition"));
		LoadItem(Imagekxf::ResourceID::ApplicationRun, wxS("application-run"));
		LoadItem(Imagekxf::ResourceID::ApplicationTask, wxS("application-task"));
		LoadItem(Imagekxf::ResourceID::ApplicationDockDown, wxS("application-dock-down"));
		LoadItem(Imagekxf::ResourceID::ApplicationSidebarCollapse, wxS("application-sidebar-collapse"));
		LoadItem(Imagekxf::ResourceID::ApplicationSidebarExpand, wxS("application-sidebar-expand"));

		LoadItem(Imagekxf::ResourceID::Home, wxS("home"));
		LoadItem(Imagekxf::ResourceID::Disk, wxS("disk"));
		LoadItem(Imagekxf::ResourceID::Tags, wxS("tags"));
		LoadItem(Imagekxf::ResourceID::Chart, wxS("chart"));
		LoadItem(Imagekxf::ResourceID::Image, wxS("image"));
		LoadItem(Imagekxf::ResourceID::Puzzle, wxS("puzzle"));
		LoadItem(Imagekxf::ResourceID::Eraser, wxS("eraser"));
		LoadItem(Imagekxf::ResourceID::Pictures, wxS("pictures"));
		LoadItem(Imagekxf::ResourceID::Briefcase, wxS("briefcase"));
		LoadItem(Imagekxf::ResourceID::PencilSmall, wxS("pencil-small"));
		LoadItem(Imagekxf::ResourceID::PlugDisconnect, wxS("plug-disconnect"));
		LoadItem(Imagekxf::ResourceID::LocaleAlternate, wxS("locale-alternate"));
		LoadItem(Imagekxf::ResourceID::ProjectionScreen, wxS("projection-screen"));
		LoadItem(Imagekxf::ResourceID::WrenchScrewdriver, wxS("wrench-screwdriver"));

		LoadItem(Imagekxf::ResourceID::Plus, wxS("plus"));
		LoadItem(Imagekxf::ResourceID::PlusSmall, wxS("plus-small"));

		LoadItem(Imagekxf::ResourceID::Minus, wxS("minus"));
		LoadItem(Imagekxf::ResourceID::MinusSmall, wxS("minus-small"));

		LoadItem(Imagekxf::ResourceID::QuestionFrame, wxS("question-frame"));

		LoadItem(Imagekxf::ResourceID::InformationFrame, wxS("information-frame"));
		LoadItem(Imagekxf::ResourceID::InformationFrameEmpty, wxS("information-frame-empty"));

		LoadItem(Imagekxf::ResourceID::CrossCircleFrame, wxS("cross-circle-frame"));
		LoadItem(Imagekxf::ResourceID::CrossCircleEmpty, wxS("cross-circle-frame-empty"));
		LoadItem(Imagekxf::ResourceID::CrossWhite, wxS("cross-white"));

		LoadItem(Imagekxf::ResourceID::TickCircleFrame, wxS("tick-circle-frame"));
		LoadItem(Imagekxf::ResourceID::TickCircleFrameEmpty, wxS("tick-circle-frame-empty"));

		LoadItem(Imagekxf::ResourceID::Exclamation, wxS("exclamation"));
		LoadItem(Imagekxf::ResourceID::ExclamationCircleFrame, wxS("exclamation-circle-frame"));
		LoadItem(Imagekxf::ResourceID::ExclamationCircleFrameEmpty, wxS("exclamation-circle-frame-empty"));

		LoadItem(Imagekxf::ResourceID::Direction, wxS("direction"));
		LoadItem(Imagekxf::ResourceID::DirectionPlus, wxS("direction--plus"));
		LoadItem(Imagekxf::ResourceID::DirectionMinus, wxS("direction--minus"));

		LoadItem(Imagekxf::ResourceID::Block, wxS("block"));
		LoadItem(Imagekxf::ResourceID::BlockPlus, wxS("block--plus"));
		LoadItem(Imagekxf::ResourceID::BlockMinus, wxS("block--minus"));

		LoadItem(Imagekxf::ResourceID::ControlCursor, wxS("control-cursor"));
		LoadItem(Imagekxf::ResourceID::ControlPause, wxS("control-pause"));
		LoadItem(Imagekxf::ResourceID::ControlStop, wxS("control-stop"));
		LoadItem(Imagekxf::ResourceID::ControlStopSquare, wxS("control-stop-square"));
		LoadItem(Imagekxf::ResourceID::ControlDown, wxS("control-down"));
		LoadItem(Imagekxf::ResourceID::ControlLeft, wxS("control-left"));
		LoadItem(Imagekxf::ResourceID::ControlRight, wxS("control-right"));
		LoadItem(Imagekxf::ResourceID::ControlUp, wxS("control-up"));

		LoadItem(Imagekxf::ResourceID::Cheque, wxS("cheque"));
		LoadItem(Imagekxf::ResourceID::ChequePlus, wxS("cheque--plus"));
		LoadItem(Imagekxf::ResourceID::ChequeMinus, wxS("cheque--minus"));

		LoadItem(Imagekxf::ResourceID::Flag, wxS("flag"));
		LoadItem(Imagekxf::ResourceID::FlagPlus, wxS("flag--plus"));
		LoadItem(Imagekxf::ResourceID::FlagMinus, wxS("flag--minus"));

		LoadItem(Imagekxf::ResourceID::Folder, wxS("folder"));
		LoadItem(Imagekxf::ResourceID::FolderOpen, wxS("folder-open"));
		LoadItem(Imagekxf::ResourceID::FolderPlus, wxS("folder--plus"));
		LoadItem(Imagekxf::ResourceID::FolderMinus, wxS("folder--minus"));
		LoadItem(Imagekxf::ResourceID::FolderSearchResult, wxS("folder-search-result"));
		LoadItem(Imagekxf::ResourceID::FolderZipper, wxS("folder-zipper"));
		LoadItem(Imagekxf::ResourceID::FolderArrow, wxS("folder--arrow"));

		LoadItem(Imagekxf::ResourceID::Folders, wxS("folders"));
		LoadItem(Imagekxf::ResourceID::FoldersPlus, wxS("folders--plus"));
		
		LoadItem(Imagekxf::ResourceID::Document, wxS("document"));
		LoadItem(Imagekxf::ResourceID::DocumentNew, wxS("document-new"));
		LoadItem(Imagekxf::ResourceID::DocumentImport, wxS("document-import"));
		LoadItem(Imagekxf::ResourceID::DocumentExport, wxS("document-export"));
		LoadItem(Imagekxf::ResourceID::DocumentPencil, wxS("document--pencil"));
		LoadItem(Imagekxf::ResourceID::DocumentPlus, wxS("document--plus"));
		LoadItem(Imagekxf::ResourceID::DocumentMinus, wxS("document--minus"));

		LoadItem(Imagekxf::ResourceID::Documents, wxS("documents"));
		LoadItem(Imagekxf::ResourceID::DocumentsPlus, wxS("documents--plus"));

		LoadItem(Imagekxf::ResourceID::Calendar, wxS("calendar"));
		LoadItem(Imagekxf::ResourceID::CalendarDay, wxS("calendar-day"));

		LoadItem(Imagekxf::ResourceID::NotificationCounter, wxS("notification-counter"));
		LoadItem(Imagekxf::ResourceID::NotificationCounter42, wxS("notification-counter-42"));

		LoadItem(Imagekxf::ResourceID::Gear, wxS("gear"));
		LoadItem(Imagekxf::ResourceID::GearMinus, wxS("gear--minus"));
		LoadItem(Imagekxf::ResourceID::GearPencil, wxS("gear--pencil"));
		LoadItem(Imagekxf::ResourceID::GearPlus, wxS("gear--plus"));
		
		LoadItem(Imagekxf::ResourceID::Jar, wxS("jar"));
		LoadItem(Imagekxf::ResourceID::JarEmpty, wxS("jar-empty"));
		
		LoadItem(Imagekxf::ResourceID::Arrow270, wxS("arrow-270"));
		LoadItem(Imagekxf::ResourceID::ArrowCircleDouble, wxS("arrow-circle-double"));
		LoadItem(Imagekxf::ResourceID::ArrowCircle135Left, wxS("arrow-circle-135-left"));
		LoadItem(Imagekxf::ResourceID::ArrowCurve180Left, wxS("arrow-curve-180-left"));
		
		LoadItem(Imagekxf::ResourceID::Categories, wxS("categories"));
		LoadItem(Imagekxf::ResourceID::Category, wxS("category"));
		LoadItem(Imagekxf::ResourceID::CategoryGroup, wxS("category-group"));
		LoadItem(Imagekxf::ResourceID::CategoryGroupSelect, wxS("category-group-select"));
		LoadItem(Imagekxf::ResourceID::CategoryItem, wxS("category-item"));
		LoadItem(Imagekxf::ResourceID::CategoryMinus, wxS("category-item-minus"));
		LoadItem(Imagekxf::ResourceID::CategoryItemSelect, wxS("category-item-select"));

		LoadItem(Imagekxf::ResourceID::Box, wxS("box"));
		LoadItem(Imagekxf::ResourceID::BoxMinus, wxS("box--minus"));
		LoadItem(Imagekxf::ResourceID::BoxPlus, wxS("box--plus"));
		LoadItem(Imagekxf::ResourceID::BoxSearchResult, wxS("box-search-result"));
		
		LoadItem(Imagekxf::ResourceID::Compile, wxS("compile"));
		LoadItem(Imagekxf::ResourceID::CompileError, wxS("compile-error"));
		LoadItem(Imagekxf::ResourceID::CompileWarning, wxS("compile-warning"));

		LoadItem(Imagekxf::ResourceID::Key, wxS("key"));
		LoadItem(Imagekxf::ResourceID::Lock, wxS("lock"));
		LoadItem(Imagekxf::ResourceID::LockSSL, wxS("lock-ssl"));
		
		LoadItem(Imagekxf::ResourceID::Magnifier, wxS("magnifier"));
		LoadItem(Imagekxf::ResourceID::MagnifierPlus, wxS("magnifier--plus"));
		LoadItem(Imagekxf::ResourceID::MagnifierMinus, wxS("magnifier--minus"));
		LoadItem(Imagekxf::ResourceID::MagnifierPencil, wxS("magnifier--pencil"));
		LoadItem(Imagekxf::ResourceID::MagnifierLeft, wxS("magnifier-left"));
		LoadItem(Imagekxf::ResourceID::MagnifierZoom, wxS("magnifier-zoom"));
		LoadItem(Imagekxf::ResourceID::MagnifierZoomIn, wxS("magnifier-zoom-in"));
		LoadItem(Imagekxf::ResourceID::MagnifierZoomOut, wxS("magnifier-zoom-out"));
		
		LoadItem(Imagekxf::ResourceID::SortAlphabet, wxS("sort-alphabet"));
		LoadItem(Imagekxf::ResourceID::SortAlphabetDescending, wxS("sort-alphabet-descending"));
		LoadItem(Imagekxf::ResourceID::SorNumber, wxS("sort-number"));
		LoadItem(Imagekxf::ResourceID::SorNumberDescending, wxS("sort-number-descending"));

		LoadItem(Imagekxf::ResourceID::Edit, wxS("edit"));
		LoadItem(Imagekxf::ResourceID::EditBold, wxS("edit-bold"));
		LoadItem(Imagekxf::ResourceID::EditItalic, wxS("edit-italic"));
		LoadItem(Imagekxf::ResourceID::EditStrike, wxS("edit-strike"));
		LoadItem(Imagekxf::ResourceID::EditUnderline, wxS("edit-underline"));

		LoadItem(Imagekxf::ResourceID::EditCode, wxS("edit-code"));
		LoadItem(Imagekxf::ResourceID::EditCodeDivision, wxS("edit-code-division"));

		LoadItem(Imagekxf::ResourceID::EditHeading, wxS("edit-heading"));
		LoadItem(Imagekxf::ResourceID::EditHeading1, wxS("edit-heading-1"));
		LoadItem(Imagekxf::ResourceID::EditHeading2, wxS("edit-heading-2"));
		LoadItem(Imagekxf::ResourceID::EditHeading3, wxS("edit-heading-3"));
		LoadItem(Imagekxf::ResourceID::EditHeading4, wxS("edit-heading-4"));
		LoadItem(Imagekxf::ResourceID::EditHeading5, wxS("edit-heading-5"));
		LoadItem(Imagekxf::ResourceID::EditHeading6, wxS("edit-heading-6"));

		LoadItem(Imagekxf::ResourceID::EditAlignmentLeft, wxS("edit-alignment-left"));
		LoadItem(Imagekxf::ResourceID::EditAlignmentRight, wxS("edit-alignment-right"));
		LoadItem(Imagekxf::ResourceID::EditAlignmentCenter, wxS("edit-alignment-center"));
		LoadItem(Imagekxf::ResourceID::EditAlignmentJustify, wxS("edit-alignment-justify"));

		LoadItem(Imagekxf::ResourceID::EditList, wxS("edit-list"));
		LoadItem(Imagekxf::ResourceID::EditListOrder, wxS("edit-list-order"));

		LoadItem(Imagekxf::ResourceID::Bell, wxS("bell"));
		LoadItem(Imagekxf::ResourceID::BellExclamation, wxS("bell--exclamation"));
		LoadItem(Imagekxf::ResourceID::BellRedCircle, wxS("bell--red-circle"));
		LoadItem(Imagekxf::ResourceID::BellPencil, wxS("bell--pencil"));
		LoadItem(Imagekxf::ResourceID::BellPlus, wxS("bell--plus"));
		LoadItem(Imagekxf::ResourceID::BellMinus, wxS("bell--minus"));

		LoadItem(Imagekxf::ResourceID::Bin, wxS("bin"));
		LoadItem(Imagekxf::ResourceID::BinArrow, wxS("bin--arrow"));
		LoadItem(Imagekxf::ResourceID::BinFull, wxS("bin-full"));

		LoadItem(Imagekxf::ResourceID::Broom, wxS("broom"));
	}

	DefaultImageProvider::DefaultImageProvider()
		:m_ImageList(GetIconSIze(), GetIconCount()), m_ImageSet(GetIconCount())
	{
	}
}
