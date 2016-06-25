#pragma once
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <algorithm>
#include <inttypes.h>
#include "LockableVector.h"

// TODO: Make CVector3 fully established class for work on Professional Skills assignment (take from TL-Source?)
#include "IEntityManager.h"
#include "INetworkPacketFactory.hpp"
#include "EntityMessages.pb.h"

// Network entity specific objects
#include "NetObject.h"
#include "NetVar.h"
#include "CNetVarBase.h"
#include "SnapshotStream.h"

#include "Constraints.h"
#include "NETIDReserved.h"
#include "ReplicationInfo.h"
#include "ControllerFlags.h"
extern uint32_t m_TotalEntities;

// Forward delcare some stuff.
namespace NETTIK
{
	class IController;
	class IControllerServer;
	class IControllerClient;
}

template <class TypeObject>
class CEntities : public IEntityManager
{
private:
	LockableVector<TypeObject*> m_MaintainedObjects;
	LockableVector<NetObject*>  m_Objects;

	std::unordered_map<uint32_t, NetObject*> m_ObjectRefs;

	std::string      m_name;
	VirtualInstance* m_pBaseInstance;

	std::function<void(TypeObject*)> m_fCallbackCreate;
	std::function<void(TypeObject*)> m_fCallbackDelete;

	NETTIK::IController*   m_pGlobalController;

public:

	size_t Count();
	size_t CountMaintained();

	// Returns a controlled object specified in the index.
	TypeObject* GetControlled(uint32_t index = 0);

	LockableVector<NetObject*>& GetObjects()
	{
		return m_Objects;
	}

	void SetCallbackCreate(std::function<void(TypeObject*)> func)
	{
		m_fCallbackCreate = func;
	}

	void SetCallbackDelete(std::function<void(TypeObject*)> func)
	{
		m_fCallbackDelete = func;
	}

	inline std::string GetName() const
	{
		return m_name;
	}

	NetObject* AddLocal(uint32_t netid, uint32_t controller = NET_CONTROLLER_NONE)
	{
		printf("%s: created object of type '%s'\n", __FUNCTION__, controller == NET_CONTROLLER_LOCAL ? "local" : "remote");

		m_Objects.safe_lock();
		m_MaintainedObjects.safe_lock();

		TypeObject* instance;
		instance = new TypeObject();

		NetObject* object;
		object = dynamic_cast<NetObject*>(instance);
		
		if (!object)
			NETTIK_EXCEPTION("Cast of object to NetObject failed.");

		object->m_NetCode = netid;
		object->m_pInstance = m_pBaseInstance;
		object->m_pManager = this;
		object->m_Controller = controller;
		object->SetIsServer(false);

		// For fast lookup.
		m_ObjectRefs[object->m_NetCode] = object;

		m_Objects.get()->push_back(object);
		m_MaintainedObjects.get()->push_back(instance);

		m_Objects.safe_unlock();
		m_MaintainedObjects.safe_unlock();

		m_fCallbackCreate(instance);
		return instance;
	}

	bool RemoveLocal(uint32_t code)
	{
		bool result = false;

		// Erase from object pointer list.
		m_Objects.safe_lock();
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end();)
		{
			if ((*it)->m_NetCode == code)
			{
				auto map_it = m_ObjectRefs.find((*it)->m_NetCode);
				if (map_it != m_ObjectRefs.end())
					m_ObjectRefs.erase(map_it);

				m_Objects.get()->erase(it);
				break;
			}
			else
				++it;
		}
		m_Objects.safe_unlock();

		// Delete from maintained objects.
		m_MaintainedObjects.safe_lock();
		for (auto it = m_MaintainedObjects.get()->begin(); it != m_MaintainedObjects.get()->end();)
		{
			if ((*it)->m_NetCode == code)
			{
				// test___
				m_fCallbackDelete(dynamic_cast<TypeObject*>(*it));
				(*it)->m_pManager = nullptr;
				delete(*it);
				m_MaintainedObjects.get()->erase(it);

				result = true;
				break;
			}
			else
				++it;
		}
		m_MaintainedObjects.safe_unlock();

		return result;
	}

	NetObject* GetByPeerID(ENetPeer* id)
	{
		for (auto it = m_ObjectRefs.begin(); it != m_ObjectRefs.end(); ++it)
		{
			NetObject* object;
			object = (*it).second;

			if (object->m_pPeer == id)
				return object;
		}

		return nullptr;
	}

	NetObject* GetByNetID(uint32_t id)
	{
		auto it = m_ObjectRefs.find(id);
		if (it == m_ObjectRefs.end())
			return nullptr;

		return (it->second);
	}

	void PostUpdate(float elapsedTime)
	{
		if (!m_pGlobalController->IsRunning())
			return;

		m_Objects.safe_lock();


		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
		{
			ReplicationInfo replicationInfo(m_pGlobalController->IsServer(), (*it)->m_pPeer, (*it)->m_Controller, elapsedTime);

			(*it)->NetworkUpdate(replicationInfo);
		}

		m_Objects.safe_unlock();
	}

	void GetSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& stream, bool bReliableFlag, bool bForced = false)
	{
		if (!m_pGlobalController->IsRunning())
			return;

		m_Objects.safe_lock();

		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
			(*it)->TakeObjectSnapshot(max_value, num_updates, stream, bReliableFlag, bForced);

		m_Objects.safe_unlock();
	}

	void SetName(std::string name);

	uint32_t Add(NetObject* object);

	bool Remove(uint32_t code);

	CEntities(VirtualInstance* baseInstance);

	virtual ~CEntities();
};

template <class TypeObject>
TypeObject* CEntities<TypeObject>::GetControlled(uint32_t index)
{
	uint32_t current_index = 0;
	for (auto it = m_ObjectRefs.begin(); it != m_ObjectRefs.end(); ++it)
	{
		if ((*it).second->m_Controller == NET_CONTROLLER_LOCAL && current_index == index)
			return dynamic_cast<TypeObject*>((*it).second);
	}

	return nullptr;
}

template <class TypeObject>
void CEntities<TypeObject>::SetName(std::string name)
{
	if (name.size() > max_entmgr_name)
		NETTIK_EXCEPTION("Entity manager name length exceeds MAX_ENTMGR_NAME.");

	m_name = name;
}

template <class TypeObject>
uint32_t CEntities<TypeObject>::Add(NetObject* object)
{
	// Sets object attributes.
	object->m_NetCode = (m_TotalEntities++);
	object->m_pInstance = m_pBaseInstance;
	object->m_pManager = this;

	// For fast lookup.
	m_ObjectRefs[object->m_NetCode] = object;

	// Get the server controller.
	NETTIK::IControllerServer* server;
	server = dynamic_cast<NETTIK::IControllerServer*>(m_pGlobalController);

	if (!server)
		NETTIK_EXCEPTION("Cannot stream objects if controller isn't a server service.");

	SnapshotStream reliableStream;
	SnapshotStream unreliableStream;

	// Creation object for new entity. 
	SnapshotEntList creationUpdate;
	creationUpdate.set_frametype(FrameType::kFRAME_Alloc);
	creationUpdate.set_name(m_name);
	creationUpdate.set_netid(object->m_NetCode);
	creationUpdate.set_controller(NET_CONTROLLER_NONE);

	char* instance_name = const_cast<char*>(m_pBaseInstance->GetName().c_str());
	creationUpdate.set_data(reinterpret_cast<unsigned char*>(instance_name), m_pBaseInstance->GetName().size() + 1);

	SnapshotStream::Stream creationStreamNewObject;
	creationUpdate.write(creationStreamNewObject);

	reliableStream.get()->push_back(creationStreamNewObject);

	m_Objects.safe_lock();
	m_Objects.get()->push_back(object);

	m_pBaseInstance->DoSnapshot(reliableStream, true, false);
	m_pBaseInstance->DoSnapshot(unreliableStream, false, false);

	for (auto object_it = m_Objects.get()->begin(); object_it != m_Objects.get()->end(); ++object_it)
	{
		// Update the creation update list to the current object
		uint32_t controllerStatus;
		controllerStatus = (*object_it)->m_pPeer == object->m_pPeer ? NET_CONTROLLER_LOCAL : NET_CONTROLLER_NONE;
		creationUpdate.set_netid((*object_it)->m_NetCode);
		creationUpdate.set_controller(controllerStatus);

		// Create a new stream to synch the current instance state to the new
		// peer object.
		SnapshotStream::Stream creationStreamExistingObject;
		creationUpdate.write(creationStreamExistingObject);

		// Set up a reliable stream for allocating new net IDs.
		SnapshotStream creationStreamReliable;
		creationStreamReliable.get()->push_back(creationStreamExistingObject);

		// Generate this packet's header.
		SnapshotHeader::Generate(creationStreamReliable, 0, 1, creationStreamExistingObject.size());

		// Send it to the new peer.
		if (object->m_pPeer)
		{
			server->SendStream(creationStreamReliable, true, object->m_pPeer);
		}

		// Inform all the objects of the snapshot changes (new objects)
		if ((*object_it)->m_pPeer)
		{
			//	server->SendStream(reliableStream, true, (*object_it)->m_pPeer);
			//	server->SendStream(unreliableStream, false, (*object_it)->m_pPeer);
		}
	}

	// Broadcast the creation update.
	if (reliableStream.modified())
		server->BroadcastStream(reliableStream, true);

	if (unreliableStream.modified())
		server->BroadcastStream(unreliableStream, false);

	// Clear streams to make way for full snapshot stream.
	reliableStream.clear();
	unreliableStream.clear();

	// Get a full snapshot of the current object list to send to the new player.
	m_pBaseInstance->DoSnapshot(reliableStream, true, true);
	m_pBaseInstance->DoSnapshot(unreliableStream, false, true);


	// Inform this object to player.
	if (object->m_pPeer)
	{
		if (reliableStream.modified())
			server->SendStream(reliableStream, true, object->m_pPeer);

		if (unreliableStream.modified())
			server->SendStream(unreliableStream, false, object->m_pPeer);
	}

	m_Objects.safe_unlock();
	m_fCallbackCreate(static_cast<TypeObject*>(object));

	return object->m_NetCode;
}


template <class TypeObject>
bool CEntities<TypeObject>::Remove(uint32_t code)
{
	bool result = false;

	NETTIK::IControllerServer* server;
	server = dynamic_cast<NETTIK::IControllerServer*>(m_pGlobalController);

	if (!server)
		NETTIK_EXCEPTION("Cannot stream objects if controller isn't a server service.");

	SnapshotEntList deletionUpdate;
	deletionUpdate.set_frametype(FrameType::kFRAME_Dealloc);
	deletionUpdate.set_name(m_name);
	deletionUpdate.set_netid(code);

	char* instance_name = const_cast<char*>(m_pBaseInstance->GetName().c_str());
	deletionUpdate.set_data(reinterpret_cast<unsigned char*>(instance_name), m_pBaseInstance->GetName().size() + 1);

	m_Objects.safe_lock();


	for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end();)
	{
		if ((*it)->m_NetCode == code)
		{

			auto map_it = m_ObjectRefs.find((*it)->m_NetCode);
			if (map_it != m_ObjectRefs.end())
				m_ObjectRefs.erase(map_it);

			(*it)->m_pInstance = nullptr;
			(*it)->m_pManager = nullptr;

			m_fCallbackDelete(static_cast<TypeObject*>(*it));
			m_Objects.get()->erase(it);
			result = true;
			break;
		}
		else
			++it;
	}

	SnapshotStream::Stream deletionStream;
	deletionUpdate.write(deletionStream);

	SnapshotStream reliableStream;
	reliableStream.get()->push_back(deletionStream);

	// Generate this packet's header.
	SnapshotHeader::Generate(reliableStream, 0, 1, deletionStream.size());

	server->BroadcastStream(reliableStream, true);

	m_Objects.safe_unlock();
	return result;
}

template <class TypeObject>
CEntities<TypeObject>::CEntities(VirtualInstance* baseInstance)
{
	m_pBaseInstance = baseInstance;

	NETTIK::IController* controller;
	controller = NETTIK::IController::GetPeerSingleton();

	if (!controller)
		throw std::runtime_error("IController not found.");

	m_pGlobalController = controller;
}

template <class TypeObject>
CEntities<TypeObject>::~CEntities()
{
	m_Objects.safe_lock();
	m_MaintainedObjects.safe_lock();

	for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end();)
		it = m_Objects.get()->erase(it);

	// memory leak but needs investigating because of heap corruption.
	for (auto it = m_MaintainedObjects.get()->begin(); it != m_MaintainedObjects.get()->end();)
		it = m_MaintainedObjects.get()->erase(it);

	m_MaintainedObjects.safe_unlock();
	m_Objects.safe_unlock();
}

template <class TypeObject>
inline size_t CEntities<TypeObject>::Count()
{
	size_t result;

//	m_Objects.safe_lock();
	result = m_Objects.get()->size();
//	m_Objects.safe_unlock();
	return result;

}

template <class TypeObject>
inline size_t CEntities<TypeObject>::CountMaintained()
{
	size_t result;
	
	m_MaintainedObjects.safe_lock();
	result = m_MaintainedObjects.get()->size();
	m_MaintainedObjects.safe_unlock();
	return result;
}