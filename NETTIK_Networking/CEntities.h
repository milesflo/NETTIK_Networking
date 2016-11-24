//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "IEntityManager.h"
#include <unordered_map>
#define GEN_TEMPLATE_FN(Parent, RType) template <class TypeObject> RType Parent<TypeObject>

// Forward delcare the controller interfaces
class NetSystem;

template <class TypeObject>
class CEntities : public IEntityManager
{
public:
	//-------------------------------------------------
	// Counts the internal entity lists.
	//-------------------------------------------------
	size_t CountMaintained();

	//-------------------------------------------------
	// Returns a controlled object specified in the
	// index.
	//-------------------------------------------------
	TypeObject* GetControlled(uint32_t index = 0);

	//-------------------------------------------------
	// Assignments for callbacks
	//-------------------------------------------------
	void SetCallbackCreate(std::function<void(TypeObject*)> func)
	{
		m_fCallbackCreate = func;
	}
	void SetCallbackDelete(std::function<void(TypeObject*)> func)
	{
		m_fCallbackDelete = func;
	}

	NetObject* AddLocal(uint32_t netid, uint32_t controller = NET_CONTROLLER_NONE)
	{
		m_Objects.safe_lock();
		m_MaintainedObjects.safe_lock();

		TypeObject* instance;
		instance = new TypeObject();

		NetObject* object;
		object = dynamic_cast<NetObject*>(instance);
		
		if (!object)
			NETTIK_EXCEPTION("Cast of object to NetObject failed.");

		object->SetNetID( netid );
		object->m_pInstance = m_pBaseInstance;
		object->m_pManager = this;
		object->SetController( controller );
		object->SetIsServer(false);

		// For fast lookup.
		m_ObjectRefs[object->GetNetID()] = object;

		m_Objects.get()->push_back(object);
		m_MaintainedObjects.get()->push_back(instance);

		m_Objects.safe_unlock();
		m_MaintainedObjects.safe_unlock();

		if (m_fCallbackCreate != nullptr)
		{
			m_fCallbackCreate(instance);
		}
		object->Initialize();

		return instance;
	}

	bool RemoveLocal(uint32_t code)
	{
		bool result = false;

		// Erase from object pointer list.
		m_Objects.safe_lock();
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end();)
		{
			if ((*it)->GetNetID() == code)
			{
				auto map_it = m_ObjectRefs.find((*it)->GetNetID());
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
			if ((*it)->GetNetID() == code)
			{
				if (m_fCallbackDelete != nullptr)
				{
					m_fCallbackDelete(dynamic_cast<TypeObject*>(*it));
				}

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
			ReplicationInfo replicationInfo(m_pGlobalController->IsServer(), (*it)->m_pPeer, (*it)->GetController(), elapsedTime);

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
		{
			// Only snapshot locally owned object.
			if ((*it)->IsNetworkLocal())
			{
				(*it)->TakeObjectSnapshot(max_value, num_updates, stream, bReliableFlag, bForced);
			}
		}

		m_Objects.safe_unlock();
	}

	//-----------------------------------------------------------
	// NETTIK 2: Builds a dynamic shared_ptr of the container
	// object.
	//-----------------------------------------------------------
	std::shared_ptr<TypeObject> Build();

	uint32_t Add(NetObject* object);

	bool Remove(uint32_t code);

	CEntities(VirtualInstance* baseInstance);

	virtual ~CEntities();

private:
	//------------------------------------
	// Internal object lists.
	//------------------------------------
	LockableVector<TypeObject*> m_MaintainedObjects;               // List of locally owned and maintained objects.
	std::unordered_map<NetObject::NetID, NetObject*> m_ObjectRefs; // List of object references to map NetObjects directly to.

	//------------------------------------
	// Callbacks
	//------------------------------------
	std::function<void(TypeObject*)> m_fCallbackCreate = nullptr;
	std::function<void(TypeObject*)> m_fCallbackDelete = nullptr;


	//------------------------------------
	// Parent references
	//------------------------------------
	VirtualInstance * m_pBaseInstance;
	NetSystem       * m_pGlobalController;

};

//-----------------------------------------------------------
// NETTIK 2: Builds a dynamic shared_ptr of the container
// object.
//-----------------------------------------------------------
//std::shared_ptr<TypeObject> Build();

GEN_TEMPLATE_FN(CEntities, std::shared_ptr<TypeObject>)::Build()
{
	auto built_ent = std::make_shared<TypeObject>();
	built_ent->SetNetID( GetNextID() );
	built_ent->m_pInstance = m_pBaseInstance;
	built_ent->m_pManager = this;
	built_ent->SetController( NET_CONTROLLER_LOCAL );
	
	NetSystemServer* pServer = dynamic_cast<NetSystemServer*>(m_pGlobalController);
	if (!pServer)
	{
		pServer->Queue
		return nullptr;
	}

	m_ObjectRefs[ built_ent->GetNetID() ] = built_ent.get();

	return data;
}

template <class TypeObject>
TypeObject* CEntities<TypeObject>::GetControlled(uint32_t index)
{
	uint32_t current_index = 0;
	for (auto it = m_ObjectRefs.begin(); it != m_ObjectRefs.end(); ++it)
	{
		if ((*it).second->GetController() == NET_CONTROLLER_LOCAL && current_index == index)
			return dynamic_cast<TypeObject*>((*it).second);
	}

	return nullptr;
}

template <class TypeObject>
uint32_t CEntities<TypeObject>::Add(NetObject* object)
{
	// Sets object attributes.
	object->SetNetID( GetNextID() );
	object->m_pInstance = m_pBaseInstance;
	object->m_pManager = this;
	object->SetController( NET_CONTROLLER_LOCAL );

	// For fast lookup.
	m_ObjectRefs[object->GetNetID()] = object;

	// Get the server controller.
	NetSystemServer* server = dynamic_cast<NetSystemServer*>(m_pGlobalController);

	if (!server)
		NETTIK_EXCEPTION("Cannot stream objects if controller isn't a server service.");

	SnapshotStream reliableStream;
	SnapshotStream unreliableStream;

	// Creation object for new entity. 
	SnapshotEntList creationUpdate;
	creationUpdate.set_frametype(FrameType::kFRAME_Alloc);
	creationUpdate.set_name(m_ManagerName);
	creationUpdate.set_netid(object->GetNetID());
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
		creationUpdate.set_netid((*object_it)->GetNetID());
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
			(*object_it)->SendLists(object->m_pPeer);
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

	// Emit callback event.
	if (m_fCallbackCreate != nullptr)
	{
		m_fCallbackCreate(static_cast<TypeObject*>(object));
	}

	object->Initialize();

	return object->GetNetID();
}


template <class TypeObject>
bool CEntities<TypeObject>::Remove(uint32_t code)
{
	bool result = false;

	NetSystemServer* server = dynamic_cast<NetSystemServer*>(m_pGlobalController);

	if (!server)
		NETTIK_EXCEPTION("Cannot stream objects if controller isn't a server service.");

	SnapshotEntList deletionUpdate;
	deletionUpdate.set_frametype(FrameType::kFRAME_Dealloc);
	deletionUpdate.set_name(m_ManagerName);
	deletionUpdate.set_netid(code);

	char* instance_name = const_cast<char*>(m_pBaseInstance->GetName().c_str());
	deletionUpdate.set_data(reinterpret_cast<unsigned char*>(instance_name), m_pBaseInstance->GetName().size() + 1);

	m_Objects.safe_lock();


	for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end();)
	{
		if ((*it)->GetNetID() == code)
		{

			auto map_it = m_ObjectRefs.find((*it)->GetNetID());
			if (map_it != m_ObjectRefs.end())
				m_ObjectRefs.erase(map_it);

			//(*it)->m_pInstance = nullptr;
			//(*it)->m_pManager = nullptr;

			if (m_fCallbackDelete != nullptr)
			{
				m_fCallbackDelete(static_cast<TypeObject*>(*it));
			}
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

	NetSystem* controller = NetSystem::GetSingleton( );

	if (!controller)
		throw std::runtime_error("NetSystem not found.");

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
inline size_t CEntities<TypeObject>::CountMaintained()
{
	size_t result;
	
	m_MaintainedObjects.safe_lock();
	result = m_MaintainedObjects.get()->size();
	m_MaintainedObjects.safe_unlock();
	return result;
}

#undef GEN_TEMPLATE_FN