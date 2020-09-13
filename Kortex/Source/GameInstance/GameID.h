#pragma once
#include <Kortex/Kortex.hpp>

namespace Kortex
{
	class IGameInstance;

	class GameID
	{
		private:
			kxf::String m_ID;

		private:
			bool TestGameID(const kxf::String& id) const;
			IGameInstance* GetInstanceByID(const kxf::String& id) const;

		public:
			GameID(const kxf::String& id = wxEmptyString);
			GameID(const IGameInstance& instance);
			GameID(const GameID&) = default;
			GameID(GameID&&) = default;

		public:
			bool IsOK() const;
			explicit operator bool() const
			{
				return IsOK();
			}
			bool operator!() const
			{
				return !IsOK();
			}

			kxf::String ToString() const;
			operator kxf::String() const
			{
				return ToString();
			}
			
			IGameInstance* ToGameInstance() const;
			IGameInstance* operator->() const
			{
				return ToGameInstance();
			}

		public:
			GameID& operator=(const GameID&) = default;
			GameID& operator=(GameID&&) = default;
			GameID& operator=(const kxf::String& id);

			bool operator==(const GameID& other) const
			{
				return m_ID == other.m_ID;
			}
			bool operator==(const kxf::String& other) const
			{
				return m_ID == other;
			}
			bool operator!=(const GameID& other) const
			{
				return m_ID != other.m_ID;
			}
			bool operator!=(const kxf::String& other) const
			{
				return m_ID != other;
			}
	};
}

namespace Kortex::GameIDs
{
	static const GameID NullGameID = GameID();

	// TES
	static const GameID Morrowind = GameID("Morrowind");
	static const GameID Oblivion = GameID("Oblivion");
	static const GameID Skyrim = GameID("Skyrim");
	static const GameID SkyrimSE = GameID("SkyrimSE");
	static const GameID SkyrimVR = GameID("SkyrimVR");

	// Fallout
	static const GameID Fallout3 = GameID("Fallout3");
	static const GameID FalloutNV = GameID("FalloutNV");
	static const GameID Fallout4 = GameID("Fallout4");
	static const GameID Fallout4VR = GameID("Fallout4VR");
};
