#pragma once
#include <Kortex/Kortex.hpp>
#include "ModPackages/InstallWizard/WizardPage.h"
#include "UI/WebView.h"
#include <KxFramework/KxNotebook.h>
#include <KxFramework/KxAuiNotebook.h>
#include <KxFramework/KxTreeList.h>
#include <KxFramework/KxListBox.h>
#include <KxFramework/KxImageView.h>
#include <KxFramework/KxThumbView.h>
#include <KxFramework/KxSplitterWindow.h>

namespace Kortex::UI
{
	class ImageViewerEvent;
}

namespace Kortex::InstallWizard
{
	namespace InfoPageNS
	{
		class InfoDisplayModel;
	}

	class InfoPage: public WizardPage
	{
		private:
			KxAuiNotebook* m_TabsContainer = nullptr;
			InfoPageNS::InfoDisplayModel* m_InfoDisplayModel = nullptr;
			UI::WebView m_DescriptionView;

			KxSplitterWindow* m_DocumentsContainer = nullptr;
			KxListBox* m_DocumentsList = nullptr;
			UI::WebView m_DocumentSimple;
			UI::WebView m_DocumentAdvanced;

			KxThumbView* m_ScreenshotsView = nullptr;
			std::unordered_map<int, const PackageProject::ImageItem*> m_ImagesMap;
			int m_CurrentImageIndex = -1;

			bool m_PageVisited = false;
			bool m_DescriptionLoaded = false;

		private:
			wxWindow* CreateInfoTab();
			wxWindow* CreateDescriptionTab();
			wxWindow* CreateDocumentsTab();
			wxWindow* CreateScreenshotsTab();

			void LoadInfoTab(const ModPackageProject& package);
			void LoadDescriptionTab(const ModPackageProject& package);
			void LoadDocumentsTab(const ModPackageProject& package);
			void LoadScreenshotsTab(const ModPackageProject& package);
			
			void OnTabOpened(wxAuiNotebookEvent& event);
			void OnSelectDocument(int index, bool useAdvancedEditor = false);
			void SetImageViewerNavigationInfo(UI::ImageViewerEvent& event) const;
			void OnNavigateImageViewer(UI::ImageViewerEvent& event);

		protected:
			void OnLoadUIOptions(const Application::ActiveInstanceOption& option) override;
			void OnSaveUIOptions(Application::ActiveInstanceOption& option) const override;
			void OnPackageLoaded() override;

			bool OnOpenPage() override;
			bool OnClosePage() override;

			WizardButton GetBackwardButton() override
			{
				return WizardButton({}, false);
			}
			WizardButton GetForwardButton() override
			{
				return WizardButton({}, true);
			}

		public:
			InfoPage(WizardDialog& wizard);

		public:
			wxWindow* Create() override;
			wxWindow* GetWindow() override
			{
				return m_TabsContainer;
			}
			
			WizardPageID GetID() const override
			{
				return WizardPageID::Info;
			}
			kxf::String GetCaption() const override
			{
				return KTr("InstallWizard.Page.Info");
			}
			kxf::String GetOptionName() const override
			{
				return wxS("Page/Information");
			}
			
			bool IsVisited() const
			{
				return m_PageVisited;
			}
	};
}
