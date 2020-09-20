#pragma once
#include <Kortex/Kortex.hpp>
#include "GameInstance/GameID.h"
#include "Application/BroadcastProcessor.h"
#include <kxf::UI::Framework/KxComboBoxDialog.h>
class KxPanel;
class KxButton;
class KxTextBox;
class KxListBox;
class kxf::ImageList;
class KxBitmapComboBox;
class kxf::UI::SplitterWindow;

namespace Kortex
{
	class IGameInstance;
}

namespace Kortex::GameInstance
{
	class SelectionDialog: public KxComboBoxDialog
	{
		private:
			BroadcastReciever m_BroadcastReciever;

			kxf::UI::SplitterWindow* m_Splitter = nullptr;

			KxPanel* m_LeftPane = nullptr;
			wxBoxSizer* m_LeftSizer = nullptr;
			KxBitmapComboBox* m_GameFilter = nullptr;
			kxf::ImageList* m_GameFilterImageList = nullptr;
			KxListBox* m_InstancesList = nullptr;

			KxPanel* m_RightPane = nullptr;
			wxBoxSizer* m_RightSizer = nullptr;
			KxTextBox* m_TextBox = nullptr;

			KxButton* m_OKButton = nullptr;
			KxButton* m_CreateButton = nullptr;
			KxButton* m_RemoveButton = nullptr;
			KxButton* m_CreateShortcutButton = nullptr;

		private:
			kxf::String m_SelectedGameRoot;
			IGameInstance* m_SelectedInstance = nullptr;

		public:
			bool Create(wxWindow* parent = nullptr);
			SelectionDialog() = default;
			SelectionDialog(wxWindow* parent)
			{
				Create(parent);
			}

		public:
			IGameInstance* GetSelectedInstance() const
			{
				return m_SelectedInstance;
			}

			bool IsGameRootSelected() const
			{
				return !m_SelectedGameRoot.IsEmpty();
			}
			kxf::String GetSelectedGameRoot() const
			{
				return m_SelectedGameRoot;
			}

		private:
			GameID GetCurrentFilter() const;
			IGameInstance* GetCurrentInstance() const;

			void Configure();
			void LoadGameFilter(const GameID& gameID = GameIDs::NullGameID);
			void LoadInstancesList(const GameID& gameID = GameIDs::NullGameID, IGameInstance* selectInstance = nullptr);
			bool AskForGameFolder(const IGameInstance* instance, const kxf::String& currentGamePath);

			void OnFilterSelected(const GameID& gameID = GameIDs::NullGameID);
			void OnInstanceSelected(IGameInstance* instance = nullptr);

			void OnCreateInstance(const GameID& gameID);
			void OnRemoveInstance(IGameInstance* instance);
			void OnSelectInstance(IGameInstance* instance);

			void OnCreateShortcut(wxCommandEvent& event);
			void OnButton(wxNotifyEvent& event);
			void OnUpdateProfiles(wxNotifyEvent& event);
			void OnDisplayInstanceInfo(const IGameInstance* instance);
	};
}
