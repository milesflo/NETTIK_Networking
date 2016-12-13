//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <list>
#include <map>
#include <memory>
#include "NETTIK_Networking.h"

class CPlayerRealm
{
private:
	using ContainedObject = std::shared_ptr<NetObject>;

	std::map<NetObject::NetID, ContainedObject> m_PlayerList;
	std::list<NetObject::NetID> m_FreeList;
	std::vector<NetObject::NetID> m_DeletionQueue;

	//! Finds a free ID to allocate to entity.
	NetObject::NetID GetFreeID();

public:

	void Add(ContainedObject object, ENetPeer* peer);

	void ProcessRealmDeletes();
	bool Remove(NetObject::NetID id);
	bool RemoveByNetID(NetObject::NetID netid);

	ContainedObject GetPlayer(NetObject::NetID id);
	ContainedObject GetPlayerByNetID(NetObject::NetID netid);
	ContainedObject GetPlayer(ENetPeer* peer);

	CPlayerRealm();
	virtual ~CPlayerRealm();

};