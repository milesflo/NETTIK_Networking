//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <memory>
#include <inttypes.h>
#include <list>
#include <map>
#include "NETTIK_Networking.hpp"

class CPlayerRealm
{
private:
	std::map<uint32_t,
		std::unique_ptr<NetObject>
	> m_PlayerList;

	std::list<uint32_t> m_FreeList;
	std::vector<uint32_t> m_DeletionQueue;

	//! Finds a free ID to allocate to entity.
	uint32_t GetFreeID();

public:

	//! Creates a new instance of an object and makes the realm own it.
	template <class TypeName>
	TypeName* Create(ENetPeer* peer)
	{
		std::unique_ptr<TypeName> player(new TypeName());
		player->m_pPeer = peer;
		player->m_RealmID = GetFreeID();

		uint32_t new_ID;
		new_ID = player->m_RealmID;

		TypeName* result;
		result = player.get();

		m_PlayerList[new_ID] = std::move(player);

		return result;
	}

	void Add(NetObject* object, ENetPeer* peer);

	void ProcessRealmDeletes();
	bool Remove(uint32_t id);
	bool RemoveByNetID(uint32_t netid);
	NetObject* GetPlayer(uint32_t id);
	NetObject* GetPlayerByNetID(uint32_t netid);

	NetObject* GetPlayer(ENetPeer* peer);

	CPlayerRealm();
	virtual ~CPlayerRealm();

};