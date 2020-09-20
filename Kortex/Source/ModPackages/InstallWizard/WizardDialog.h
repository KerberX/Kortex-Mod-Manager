#pragma once
#include <Kortex/Kortex.hpp>
#include "StepStack.h"
#include "Info/InfoPage.h"
#include "Requirements/RequirementsPage.h"
#include "Components/ComponentsPage.h"
#include "Installation/InstallationPage.h"
#include "Completed/CompletedPage.h"
#include "ModPackages/ModPackage.h"
#include "GameMods/ModManager/BasicGameMod.h"
#include "Utility/TempFolderKeeper.h"
#include <kxf::UI::Framework/KxButton.h>
#include <kxf::UI::Framework/KxStdDialog.h>
#include <kxf::UI::Framework/KxNotebook.h>
#include <kxf::UI::Framework/KxAuiNotebook.h>
#include <kxf::UI::Framework/KxWithOptions.h>

namespace Kortex::InstallWizard
{
	wxDECLARE_EVENT(KEVT_IW_DONE, wxNotifyEvent);

	enum class DialogOptions
	{
		None = 0,
		Debug = 1 << 0,
		Cleanup = 1 << 1,
	};
}

namespace Kortex::InstallWizard
{
	class WizardDialog:
		public KxStdDialog,
		public Utility::TempFolderKeeper,
		public KxWithOptions<DialogOptions, DialogOptions::None>
	{
		friend class WizardPage;
		friend class InstallOperation;

		public:
			static void ShowInvalidPackageDialog(wxWindow* window, const kxf::String& packagePath);

		private:
			// Pages
			InfoPage m_PageInfo;
			RequirementsPage m_PageRequirements;
			ComponentsPage m_PageComponents;
			InstallationPage m_PageInstallation;
			CompletedPage m_PageCompleted;

			// Currently displayed page
			WizardPage* m_CurrentPage = nullptr;

			// UI
			const kxf::String m_CancelDefaultLabel;
			const kxf::String m_BackwardDefaultLabel;
			const kxf::String m_ForwardDefaultLabel;

			KxButton* m_CancelButton = nullptr;
			KxButton* m_BackwardButton = nullptr;
			KxButton* m_ForwardButton = nullptr;
			wxSimplebook* m_PageContainer = nullptr;

			std::unique_ptr<ModPackage> m_Package;
			std::unique_ptr<IGameMod> m_Mod;
			const IGameMod* m_ExistingMod = nullptr;

		private:
			int GetViewSizerProportion() const override
			{
				return 1;
			}
			wxOrientation GetViewSizerOrientation() const override
			{
				return wxVERTICAL;
			}
			wxOrientation GetViewLabelSizerOrientation() const override
			{
				return wxVERTICAL;
			}
			bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* id = nullptr) const override
			{
				return false;
			}
			bool IsCloseBoxEnabled() const override
			{
				return true;
			}
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_PageContainer;
			}

		private:
			bool CreateUI(wxWindow* parent);
			void LoadUIOptions();

			bool DoSwitchPage(WizardPage& targetPage);

		public:
			void OpenPackage(const kxf::String& packagePath);
			bool LoadPackage();
			bool ProcessLoadPackage();
			void FindExistingMod();
			void AcceptExistingMod(const IGameMod& mod);
			void LoadHeaderImage();

			bool AskCancel(bool canCancel);
			void OnClose(wxCloseEvent& event);
			void OnCancelButton(wxCommandEvent& event);
			void OnGoBackward(wxCommandEvent& event);
			void OnGoForward(wxCommandEvent& event);

			void SetModData();
			KxArchive::FileIndexVector GetFilesOfFolder(const PackageProject::FolderItem* folder) const;
			kxf::String GetFinalPath(const KxFileItem& fileItem, const kxf::String& installLocation, const PackageProject::FileItem* fileEntry) const;
			void RunInstall();

		public:
			WizardDialog();
			WizardDialog(wxWindow* parent, const kxf::String& packagePath);
			bool Create(wxWindow* parent, const kxf::String& packagePath);
			bool Create(wxWindow* parent, std::unique_ptr<ModPackage> package);
			~WizardDialog();

		public:
			const ModPackageProject& GetConfig() const
			{
				return m_Package->GetConfig();
			}
			ModPackageProject& GetConfig()
			{
				return m_Package->GetConfig();
			}
			const GenericArchive& GetArchive() const
			{
				return m_Package->GetArchive();
			}
			GenericArchive& GetArchive()
			{
				return m_Package->GetArchive();
			}
			const IGameMod* GetExistingMod() const
			{
				return m_ExistingMod;
			}
			const ModPackage& GetPackage() const
			{
				return *m_Package;
			}

			std::array<WizardPage*, 5> GetPages()
			{
				return {&m_PageInfo, &m_PageRequirements, &m_PageComponents, &m_PageInstallation, &m_PageCompleted};
			}
			InfoPage& GetInfoPage()
			{
				return m_PageInfo;
			}
			RequirementsPage& GetRequirementsPage()
			{
				return m_PageRequirements;
			}
			ComponentsPage& GetComponentsPage()
			{
				return m_PageComponents;
			}
			InstallationPage& GetInstallationPage()
			{
				return m_PageInstallation;
			}
			CompletedPage& GetCompletedPage()
			{
				return m_PageCompleted;
			}

			const IGameMod& GetMod() const
			{
				return *m_Mod;
			}
			IGameMod& GetMod()
			{
				return *m_Mod;
			}
			std::unique_ptr<IGameMod> TakeMod()
			{
				return std::move(m_Mod);
			}

			bool SwitchPage(WizardPage& targetPage);
			bool SwitchPage(WizardPageID targetPage);
	};
}
