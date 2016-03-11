#pragma once
#include <memory>
#include <inttypes.h>
#include <list>
#include <map>
#include "NETTIK_Networking.hpp"

template <class TypeName>
class CPlayerRealm
{
private:
	std::map<uint32_t,
		std::unique_ptr<TypeName>
	> m_PlayerList;

	std::list<uint32_t> m_FreeList;

public:

	uint32_t GetFreeID()
	{
		uint32_t i;

		if (m_FreeList.size() == 0)
			i = m_PlayerList.size();
		else
		{
			i = m_FreeList.front();
			m_FreeList.pop_front();
		}
		return i;
	}

	TypeName* Add(ENetPeer* peer)
	{
		std::unique_ptr<TypeName> player(new TypeName());
		player->m_pPeer = peer;
		player->m_RealmID = GetFreeID();

		uint32_t new_ID;
		new_ID = player->m_RealmID;

		m_PlayerList[new_ID] = std::move(player);

		return m_PlayerList[new_ID].get();
	}

	bool Remove(uint32_t id)
	{
		auto it = m_PlayerList.find(id);
		if (it == m_PlayerList.end())
			return false;

		m_FreeList.push_back(it->second->m_RealmID);
		m_PlayerList.erase(it);

		return true;
	}

	TypeName* GetPlayer(uint32_t id)
	{
		auto it = m_PlayerList.find(id);
		if (it == m_PlayerList.end())
			return nullptr;

		return (*it).second.get();
	}

	TypeName* GetPlayer(ENetPeer* peer)
	{
		for (auto it = m_PlayerList.begin(); it != m_PlayerList.end(); it++)
		{
			if ((*it).second->m_pPeer == peer)
				return (*it).second.get();
		}

		return nullptr;
	}

	CPlayerRealm() { }
	virtual ~CPlayerRealm() { }

};