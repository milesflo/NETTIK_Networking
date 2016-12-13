//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "CPlayerRealm.h"

CPlayerRealm::CPlayerRealm()
{

}

CPlayerRealm::~CPlayerRealm()
{

}

NetObject::NetID CPlayerRealm::GetFreeID()
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

void CPlayerRealm::Add(CPlayerRealm::ContainedObject object, ENetPeer* peer)
{
	object->SetRealmID( GetFreeID() );
	object->m_pPeer = peer;

	uint32_t new_ID = object->GetRealmID();
	m_PlayerList[new_ID] = object;
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

bool CPlayerRealm::Remove(NetObject::NetID id)
{
	auto it = m_PlayerList.find(id);
	if (it == m_PlayerList.end())
		return false;

	m_FreeList.push_back(it->second->GetRealmID());
	m_PlayerList.erase(it);

	return true;
}

bool CPlayerRealm::RemoveByNetID(NetObject::NetID netid)
{
	for (auto it = m_PlayerList.begin(); it != m_PlayerList.end(); ++it)
	{
		if (it->second->GetNetID() == netid)
		{
			m_FreeList.push_back(it->second->GetRealmID());
			it->second.reset();
			m_PlayerList.erase(it);
			return true;
		}
	}

	return false;
}

CPlayerRealm::ContainedObject CPlayerRealm::GetPlayer(NetObject::NetID id)
{
	auto it = m_PlayerList.find(id);
	if (it == m_PlayerList.end())
		return nullptr;

	return (*it).second;
}

CPlayerRealm::ContainedObject CPlayerRealm::GetPlayerByNetID(NetObject::NetID netid)
{
	for (auto it = m_PlayerList.begin(); it != m_PlayerList.end(); ++it)
	{
		if (it->second->GetNetID() == netid)
		{
			return it->second;
		}
	}

	return nullptr;
}

CPlayerRealm::ContainedObject CPlayerRealm::GetPlayer(ENetPeer* peer)
{
	for (auto it = m_PlayerList.begin(); it != m_PlayerList.end(); ++it)
	{
		if (it->second->m_pPeer == peer)
			return it->second;
	}

	return nullptr;
}