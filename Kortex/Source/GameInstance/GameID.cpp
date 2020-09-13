#include "stdafx.h"
#include "GameID.h"
#include "IGameInstance.h"
#include "Application/SystemApplication.h"

namespace Kortex
{
	bool GameID::TestGameID(const kxf::String& id) const
	{
		return !id.IsEmpty();
	}
	IGameInstance* GameID::GetInstanceByID(const kxf::String& id) const
	{
		if (TestGameID(id))
		{
			IGameInstance* active = IGameInstance::GetActive();
			if (active && active->GetGameID() == id)
			{
				return active;
			}
			return IGameInstance::GetShallowInstance(id);
		}
		return nullptr;
	}

	GameID::GameID(const kxf::String& id)
		:m_ID(TestGameID(id) ? id : wxEmptyString)
	{
	}
	GameID::GameID(const IGameInstance& instance)
		:m_ID(instance.IsOK() ? instance.GetGameID().m_ID : wxEmptyString)
	{
	}

	bool GameID::IsOK() const
	{
		return TestGameID(m_ID);
	}
	kxf::String GameID::ToString() const
	{
		return IsOK() ? m_ID : wxEmptyString;
	}
	IGameInstance* GameID::ToGameInstance() const
	{
		return GetInstanceByID(m_ID);
	}

	GameID& GameID::operator=(const kxf::String& id)
	{
		m_ID = TestGameID(id) ? id : wxEmptyString;
		return *this;
	}
}
