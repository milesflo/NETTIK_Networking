//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "VirtualInstance.h"

std::shared_ptr<NetObject> VirtualInstance::FindObject(NetObject::NetID netid)
{
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
	{
		std::shared_ptr<NetObject> entry = it->second->GetByNetID(netid);

		if (entry != nullptr)
			return entry;
	}

	return nullptr;
}

void VirtualInstance::DoPostUpdate(float elapsedTime)
{
	DoListUpdate();

	// Process pending ent manager deletions.
	for (auto it = m_PendingDeletes.begin(); it != m_PendingDeletes.end(); )
	{
		delete((*it)->second);
		m_EntManagers.erase(*it);
		it = m_PendingDeletes.erase(it);
	}
	// Process ent managers still in list.
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
	{
		it->second->PostUpdate(elapsedTime);
	}
}

void VirtualInstance::DoListUpdate()
{
	std::vector<std::shared_ptr<NetObject>> updateList;

	// Get active objects to update.
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
	{

		LockableVector<std::shared_ptr<NetObject>>& object_manager = (*it).second->GetObjects();
		object_manager.safe_lock();

		auto pObjects = object_manager.get();
		for (auto object_it = pObjects->begin(); object_it != pObjects->end(); ++object_it)
		{
			updateList.push_back(*object_it);
		}

		object_manager.safe_unlock();
	}

	// 
	for (std::shared_ptr<NetObject>& pObject : updateList)
	{
		pObject->UpdateListDelta();
	}
}

void VirtualInstance::SendAllObjectLists(ENetPeer* pOwner)
{
	for (std::pair<const std::string, IEntityManager*>& pManager : m_EntManagers)
	{
		IEntityManager* pInstance = pManager.second;
		printf(" %p ", pInstance);
		pInstance->SendObjectLists( pOwner );
		printf(" fin ");
	}
}


void VirtualInstance::DoSnapshot(SnapshotStream& stream, bool bReliableFlag, bool bForced)
{
	size_t max_value = 0;
	if (stream.get()->size() > 1)
	{
		for (auto it = stream.get()->begin(); it != stream.get()->end(); ++it)
		{
			if (it->size() > max_value)
				max_value = it->size();
		}
	}

	uint16_t num_updates = static_cast<uint16_t>(stream.get()->size()) - 1;

	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
		it->second->GetSnapshot(max_value, num_updates, stream, bReliableFlag, bForced);

	if (num_updates == 0)
		return;

	INetworkCodes::msg_t code;
	code = NETID_Reserved::RTTI_Object::OBJECT_FRAME;

	SnapshotHeader::Generate(stream, m_iSequenceID, num_updates, max_value);
}

void VirtualInstance::DestroyEntityManager(std::string name)
{
	auto it = m_EntManagers.find(name);
	m_PendingDeletes.push_back(it);
}

void VirtualInstance::DestroyEntityManager(IEntityManager* mgr)
{
	DestroyEntityManager(mgr->GetName());
}

VirtualInstance::~VirtualInstance()
{
	// Clear entity managers
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end();)
	{
		delete(it->second);
		it = m_EntManagers.erase(it);
	}
};

VirtualInstance::VirtualInstance(std::string name, NetSystem* controller)
{
	m_ParentController = controller;
	m_sName = name;
}