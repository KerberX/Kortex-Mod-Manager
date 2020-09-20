#pragma once
#include <Kortex/Kortex.hpp>
#include "../WizardPage.h"
#include "Utility/OperationWithProgress.h"
#include <kxf::UI::Framework/KxPanel.h>
#include <kxf::UI::Framework/KxLabel.h>
#include <kxf::UI::Framework/KxProgressBar.h>
#include <kxf::UI::Framework/KxArchiveEvent.h>

namespace Kortex::InstallWizard
{
	class InstallOperation;

	class InstallationPage: public WizardPage
	{
		friend class WizardDialog;
		friend class InstallOperation;

		private:
			KxPanel* m_PagePanel = nullptr;

			KxProgressBar* m_MinorProgress = nullptr;
			KxProgressBar* m_MajorProgress = nullptr;
			KxLabel* m_MinorStatus = nullptr;
			KxLabel* m_MajorStatus = nullptr;

			PackageProject::FileItem::RefVector m_InstallableFiles;
			InstallOperation* m_InstallThread = nullptr;

			bool m_ShouldCancel = false;
			bool m_IsComplete = false;

		private:
			bool OnBeginInstall();
			bool OnEndInstall();
			void OnMinorProgress(KxFileOperationEvent& event);
			void OnMajorProgress(KxFileOperationEvent& event);

		protected:
			void OnLoadUIOptions(const Application::ActiveInstanceOption& option) override;
			void OnSaveUIOptions(Application::ActiveInstanceOption& option) const override;
			void OnPackageLoaded() override;

			bool OnOpenPage() override;
			bool OnClosePage() override;

			WizardButton GetCancelButton() override
			{
				return WizardButton({}, false);
			}
			WizardButton GetBackwardButton() override
			{
				return WizardButton({}, false);
			}
			WizardButton GetForwardButton() override
			{
				return WizardButton(KTr(wxID_CLOSE), true);
			}

		public:
			InstallationPage(WizardDialog& wizard)
				:WizardPage(wizard)
			{
			}

		public:
			wxWindow* Create() override;
			wxWindow* GetWindow() override
			{
				return m_PagePanel;
			}
			
			WizardPageID GetID() const override
			{
				return WizardPageID::Installation;
			}
			kxf::String GetCaption() const override
			{
				return KTr("InstallWizard.Page.Installing");
			}
			kxf::String GetOptionName() const override
			{
				return wxS("Page/Installation");
			}
	
		public:
			void CollectAllInstallableEntries();
			void SortInstallableFiles();
			void ShowInstallableFilesPreview();
			const PackageProject::FileItem::RefVector& GetInstallableFiles() const
			{
				return  m_InstallableFiles;
			}

			bool ShouldCancel() const
			{
				return m_ShouldCancel;
			}
			bool IsCompleted() const
			{
				return m_IsComplete;
			}
	};
}

namespace Kortex::InstallWizard
{
	class InstallOperation: public Utility::OperationWithProgressBase
	{
		private:
			InstallationPage& m_InstallPage;

		public:
			InstallOperation(InstallationPage& page)
				:Utility::OperationWithProgressBase(true), m_InstallPage(page)
			{
			}

		public:
			void LinkHandler(wxEvtHandler* eventHandler, wxEventType type) override
			{
				using T = wxEventTypeTag<KxFileOperationEvent>;
				GetEventHandler()->Bind(static_cast<T>(type), &InstallationPage::OnMinorProgress, &m_InstallPage);
				Utility::OperationWithProgressBase::LinkHandler(eventHandler, type);
			}
	};
}
