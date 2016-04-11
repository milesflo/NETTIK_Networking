#include "CPlayerRealm.h"

CPlayerRealm::CPlayerRealm()
{

}

CPlayerRealm::~CPlayerRealm()
{

}

uint32_t CPlayerRealm::GetFreeID()
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

void CPlayerRealm::Add(NetObject* object, ENetPeer* peer)
{
	object->m_RealmID = GetFreeID();
	object->m_pPeer = peer;

	uint32_t new_ID;
	new_ID = object->m_RealmID;

	m_PlayerList[new_ID] = std::move(std::unique_ptr<NetObject>(object));
}

void CPlayerRealm::ProcessRealmDeletes()
{
	for (auto it = m_DeletionQueue.begin(); it != m_DeletionQueue.end(); )
	{
		auto sub_it = m_PlayerList.find(*it);
		if (sub_it != m_PlayerList.end())
			m_PlayerList.erase(sub_it); // delete realm object

		it = m_DeletionQueue.erase(it);
	}
}

bool CPlayerRealm::Remove(uint32_t id)
{
	auto it = m_PlayerList.find(id);
	if (it == m_PlayerList.end())
		return false;

	m_FreeList.push_back(it->second->m_RealmID);
	m_PlayerList.erase(it);

	return true;
}

bool CPlayerRealm::RemoveByNetID(uint32_t netid)
{
	for (auto it = m_PlayerList.begin(); it != m_PlayerList.end(); ++it)
	{
		if (it->second->m_NetCode == netid)
		{
			m_FreeList.push_back(it->second->m_RealmID);
			it->second.release();
			m_PlayerList.erase(it);
			return true;
		}
	}

	return false;
}

NetObject* CPlayerRealm::GetPlayer(uint32_t id)
{
	auto it = m_PlayerList.find(id);
	if (it == m_PlayerList.end())
		return nullptr;

	return (*it).second.get();
}

NetObject* CPlayerRealm::GetPlayerByNetID(uint32_t netid)
{
	for (auto it = m_PlayerList.begin(); it != m_PlayerList.end(); ++it)
	{
		if (it->second->m_NetCode == netid)
		{
			return it->second.get();
		}
	}

	return nullptr;
}

NetObject* CPlayerRealm::GetPlayer(ENetPeer* peer)
{
	for (auto it = m_PlayerList.begin(); it != m_PlayerList.end(); ++it)
	{
		if (it->second->m_pPeer == peer)
			return it->second.get();
	}

	return nullptr;
}