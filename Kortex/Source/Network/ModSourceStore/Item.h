#pragma once
#include <Kortex/Kortex.hpp>
#include "Network/Common.h"
#include "Network/IModNetwork.h"
#include "Network/NetworkModInfo.h"
#include "GameInstance/GameID.h"
#include <kxf::UI::Framework/KxURI.h>

namespace Kortex
{
	class ModSourceStore;
	class ModSourceItem
	{
		friend class ModSourceStore;

		private:
			using TID = std::variant<kxf::String, IModNetwork*>;
			using TData = std::variant<KxURI, NetworkModInfo>;

		private:
			TID m_ID = nullptr;
			TData m_Data = NetworkModInfo();

		public:
			ModSourceItem() = default;

			ModSourceItem(const kxf::String& name, const KxURI& uri)
				:m_ID(name), m_Data(uri)
			{
			}
			ModSourceItem(const kxf::String& name, NetworkModInfo id)
				:m_ID(name), m_Data(id)
			{
			}

			ModSourceItem(IModNetwork* modNetwork, const KxURI& uri)
				:m_ID(modNetwork), m_Data(uri)
			{
			}
			ModSourceItem(IModNetwork* modNetwork, NetworkModInfo id)
				:m_ID(modNetwork), m_Data(id)
			{
			}

		public:
			bool IsOK() const;
			bool IsEmptyValue() const;
			
			// Serialization
			void Load(const kxf::XMLNode& node);
			void Save(kxf::XMLNode& node) const;

			// Mod network instance
			IModNetwork* GetModNetwork() const;
			void SetModSource(IModNetwork& modNetwork)
			{
				m_ID = &modNetwork;
			}
			template<class T> void SetModSource()
			{
				m_ID = T::GetInstance();
			}
			bool TryGetModNetwork(IModNetwork*& modNetwork) const
			{
				modNetwork = GetModNetwork();
				return modNetwork != nullptr;
			}
			bool HasModNetwork() const
			{
				return GetModNetwork() != nullptr;
			}

			// Name
			kxf::String GetName() const;
			void SetName(const kxf::String& name);
			bool TryGetName(kxf::String& name) const
			{
				name = GetName();
				return !name.IsEmpty();
			}
			bool HasName() const
			{
				return !GetName().IsEmpty();
			}

			// URI
			KxURI GetURI(const GameID& gameID = GameIDs::NullGameID) const;
			void SetURI(const KxURI& uri, const GameID& gameID = GameIDs::NullGameID)
			{
				m_Data = uri;
			}
			bool TryGetURI(KxURI& uri, const GameID& gameID = GameIDs::NullGameID) const
			{
				uri = GetURI(gameID);
				return uri.IsOk();
			}
			bool HasURI(const GameID& gameID = GameIDs::NullGameID) const
			{
				return GetURI(gameID).IsOk();
			}

			// Network mod info
			NetworkModInfo GetModInfo() const;
			void SetModInfo(NetworkModInfo modInfo)
			{
				m_Data = modInfo;
			}
			bool TryGetModInfo(NetworkModInfo& modInfo) const
			{
				modInfo = GetModInfo();
				return !modInfo.IsEmpty();
			}
			bool HasModInfo() const
			{
				return !GetModInfo().IsEmpty();
			}
	};
}
