#pragma once
#include <Kortex/Kortex.hpp>
#include "Application/DefaultWorkspace.h"
#include "Workspace.h"
#include "Utility/LabeledValue.h"
#include <KxFramework/KxPanel.h>
class KxLabel;
class KxTextBox;

namespace Kortex
{
	class WorkspaceDocument;
	class ModPackageProject;
}

namespace Kortex::PackageDesigner
{
	class PageBase: public Application::DefaultWindowWorkspace<KxPanel>
	{
		public:
			static const int ms_LeftMargin = LayoutConstants::HorizontalSpacing * 4;

		protected:
			Workspace* m_MainWorkspace = nullptr;
			WorkspaceDocument* m_Controller = nullptr;

		protected:
			bool OnCreateWorkspace() override;
			bool OnOpenWorkspace() override;
			bool OnCloseWorkspace() override;
			void OnReloadWorkspace() override;

			ModPackageProject* GetProject() const;

		public:
			PageBase(Workspace& mainWorkspace, WorkspaceDocument& controller);

		public:
			kxf::ResourceID GetIcon() const override
			{
				return Imagekxf::ResourceID::Box;
			}
			kxf::String GetName() const override;
			IWorkspaceContainer* GetPreferredContainer() const override
			{
				return m_MainWorkspace->QueryInterface<IWorkspaceContainer>();
			}
			virtual kxf::String GetPageName() const = 0;

			Workspace* GetMainWorkspace() const
			{
				return m_MainWorkspace;
			}
			KxTextBox* CreateInputField(wxWindow* window);

			static KxLabel* CreateCaptionLabel(wxWindow* window, const kxf::String& label);
			static KxLabel* CreateNormalLabel(wxWindow* window, const kxf::String& label, bool addColon = true, bool addLine = false);
			template<class T> static T* AddControlsRow(wxSizer* sizer, const kxf::String& labelText, T* control, int controlProportion = 1, KxLabel** labelOut = nullptr)
			{
				KxLabel* label = CreateNormalLabel(control->GetParent(), labelText);
				if (labelOut)
				{
					*labelOut = label;
				}
				sizer->Add(label, 0, wxEXPAND);

				if (controlProportion == 0)
				{
					wxBoxSizer* controlSizer = new wxBoxSizer(wxVERTICAL);
					controlSizer->Add(control, 0);
					sizer->Add(controlSizer, 1, wxEXPAND);
				}
				else
				{
					sizer->Add(control, controlProportion, wxEXPAND);
				}
				return control;
			}
			template<class T> static T* AddControlsRow2(wxWindow* window, wxSizer* sizer, const kxf::String& label, T* object, int objectProportion = 1, bool addSpacer = true)
			{
				wxBoxSizer* labelSizer = new wxBoxSizer(wxVERTICAL);
				labelSizer->Add(CreateNormalLabel(window, label), 0, wxEXPAND);
				if (addSpacer)
				{
					labelSizer->AddStretchSpacer(1);
				}

				sizer->Add(labelSizer, 1, wxEXPAND);
				sizer->Add(object, objectProportion, wxEXPAND);
				return object;
			}
			static KxAuiToolBar* CreateListToolBar(wxWindow* window, bool isVertical = false, bool showText = false);
		
			static void ShowTooltipWarning(wxWindow* window, const kxf::String& message, const wxRect& rect = KxNullWxRect);
			static void WarnIDCollision(wxWindow* window, const wxRect& rect = KxNullWxRect);
	};
}
