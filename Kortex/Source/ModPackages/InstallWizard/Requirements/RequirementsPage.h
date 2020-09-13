#pragma once
#include <Kortex/Kortex.hpp>
#include "ModPackages/InstallWizard/WizardPage.h"

namespace Kortex::InstallWizard
{
	namespace RequirementsPageNS
	{
		class DisplayModel;
	}

	class RequirementsPage: public WizardPage
	{
		private:
			RequirementsPageNS::DisplayModel* m_DisplayModel = nullptr;

		protected:
			void OnLoadUIOptions(const Application::ActiveInstanceOption& option) override;
			void OnSaveUIOptions(Application::ActiveInstanceOption& option) const override;
			void OnPackageLoaded() override;

		public:
			RequirementsPage(WizardDialog& wizard);

		public:
			wxWindow* Create() override;
			wxWindow* GetWindow() override;
			
			WizardPageID GetID() const override
			{
				return WizardPageID::Requirements;
			}
			kxf::String GetCaption() const override
			{
				return KTr("InstallWizard.Page.Requirements");
			}
			kxf::String GetOptionName() const override
			{
				return wxS("Page/Requirements");
			}
	
		public:
			bool HasMainRequirements() const;
	};
}
