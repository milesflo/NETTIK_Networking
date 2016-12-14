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
	using EventCallback_t = EventCallback<TypeObject>;

	//-------------------------------------------------
	// Counts the internal entity lists.
	//-------------------------------------------------
	size_t CountMaintained();

	//-------------------------------------------------
	// Returns a controlled object specified in the
	// index.
	//-------------------------------------------------
	std::shared_ptr<TypeObject> GetControlled(uint32_t index = 0);

	//-------------------------------------------------
	// Assignments for callbacks
	//-------------------------------------------------
	void SetCallbackCreate(EventCallback_t func)
	{
		m_fCallbackCreate = func;
	}
	void SetCallbackDelete(EventCallback_t func)
	{
		m_fCallbackDelete = func;
	}

	/*
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
	*/

	std::shared_ptr<NetObject> GetByPeerID(ENetPeer* peer)
	{
		auto net_list = m_NetworkObjects.get();
		std::unique_lock<std::recursive_mutex> guard(m_NetworkObjects.mutex());

		for (auto it = net_list->begin(); it != net_list->end(); ++it)
		{
			if ((*it)->m_pPeer == peer)
				return (*it);
		}

		return nullptr;
	}

	std::shared_ptr<NetObject> GetByNetID(NetObject::NetID id)
	{
		auto net_list = m_NetworkObjects.get();
		std::unique_lock<std::recursive_mutex> guard(m_NetworkObjects.mutex());

		for (auto it = net_list->begin(); it != net_list->end(); ++it)
		{
			if ((*it)->GetNetID() == id)
				return (*it);
		}

		return nullptr;
	}

	void PostUpdate(float elapsedTime)
	{
		if (!m_pGlobalController->IsRunning())
			return;

		m_NetworkObjects.safe_lock();

		auto object_list = m_NetworkObjects.get();

		for (auto it = object_list->begin(); it != object_list->end();)
		{
			ReplicationInfo replicationInfo(m_pGlobalController->IsServer(), (*it)->m_pPeer, (*it)->GetController(), elapsedTime);

			// If there's only one reference left of the object, collect the garbage and 
			// release the entity.
			if (it->use_count() == 1 && m_pGlobalController->IsServer())
			{
				CMessageDispatcher::Add(kMessageType_Warn, "(SERVER) Freeing object with loose references");
				it = object_list->erase(it);
				continue;
			}

			(*it)->NetworkUpdate(replicationInfo);
			++ it;
		}

		m_NetworkObjects.safe_unlock();
	}

	void GetSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& stream, bool bReliableFlag, bool bForced = false)
	{
		if (!m_pGlobalController->IsRunning())
			return;

		m_NetworkObjects.safe_lock();

		auto object_list = m_NetworkObjects.get();

		for (auto obj = object_list->begin(); obj != object_list->end(); ++obj)
		{
			std::shared_ptr<NetObject>& net_obj = (*obj);
			// Only snapshot locally owned object.
			if (net_obj->IsNetworkLocal())
			{
				net_obj->TakeObjectSnapshot(max_value, num_updates, stream, bReliableFlag, bForced);
			}
		}

		m_NetworkObjects.safe_unlock();
	}

	//-----------------------------------------------------------
	// NETTIK 2: Builds a dynamic shared_ptr of the container
	// object.
	//-----------------------------------------------------------
	std::shared_ptr<TypeObject> Build(ENetPeer* pOwner);

	std::shared_ptr<NetObject> BuildLocal(NetObject::NetID netID, std::uint32_t controller = NET_CONTROLLER_NONE);

	void FreeLocal(NetObject::NetID netid);


	bool FreeEntity(NetObject* pTarget);


	CEntities(VirtualInstance* baseInstance);

	virtual ~CEntities();

	void SendObjectLists(ENetPeer* pOwner);

private:

	void FreeListEnt(EntityList& list, NetObject::NetID id);

	//------------------------------------
	// Internal object lists.
	//------------------------------------
	//std::unordered_map<NetObject::NetID, NetObject*> m_ObjectRefs; // List of object references to map NetObjects directly to.

	//------------------------------------
	// Callbacks
	//------------------------------------
	EventCallback_t m_fCallbackCreate = nullptr;
	EventCallback_t m_fCallbackDelete = nullptr;


	//------------------------------------
	// Parent references
	//------------------------------------
	VirtualInstance * m_pBaseInstance;
	NetSystem       * m_pGlobalController;

};

GEN_TEMPLATE_FN(CEntities, bool)::FreeEntity(NetObject* pTarget)
{
	NetSystemServer* pServer = dynamic_cast<NetSystemServer*>(m_pGlobalController);

	// Don't free the entity if the netsystem is not a client, or hasn't been initialised.
	if (!pServer)
	{
		return false;
	}
	
	SnapshotEntList deletionUpdate;
	deletionUpdate.set_frametype(FrameType::kFRAME_Dealloc);
	deletionUpdate.set_name(m_ManagerName);
	deletionUpdate.set_netid(pTarget->GetNetID());

	char* instance_name = const_cast<char*>(m_pBaseInstance->GetName().c_str());
	deletionUpdate.set_data(reinterpret_cast<unsigned char*>(instance_name), m_pBaseInstance->GetName().size() + 1);

	if (m_fCallbackDelete)
	{
		m_fCallbackDelete( dynamic_cast<TypeObject*>(pTarget) );
	}

	SnapshotStream::Stream deletionStream;
	deletionUpdate.write(deletionStream);

	SnapshotStream reliableStream;
	reliableStream.get()->push_back(deletionStream);

	// Generate this packet's header.
	SnapshotHeader::Generate(reliableStream, 0, 1, deletionStream.size());
	pServer->BroadcastStream(reliableStream, true);

	return true;
}

//-----------------------------------------------------------
// NETTIK 2: Builds a dynamic shared_ptr of the container
// object.
//-----------------------------------------------------------
GEN_TEMPLATE_FN(CEntities, std::shared_ptr<TypeObject>)::Build(ENetPeer* pOwner)
{
	auto built_ent = std::make_shared<TypeObject>();
	built_ent->SetNetID( GetNextID() );
	built_ent->m_pInstance = m_pBaseInstance;
	built_ent->m_pManager = this;
	built_ent->SetController( NET_CONTROLLER_LOCAL );
	built_ent->m_pPeer = pOwner;
	NetSystemServer* pServer = dynamic_cast<NetSystemServer*>(m_pGlobalController);
	if (!pServer)
	{
		return nullptr;
	}

	SnapshotStream reliableStream;
	SnapshotStream unreliableStream;

	// Broadcast creation of new object.
	SnapshotEntList creationUpdate;
	creationUpdate.set_frametype(kFRAME_Alloc);
	creationUpdate.set_name(m_ManagerName);
	creationUpdate.set_netid(built_ent->GetNetID());
	creationUpdate.set_controller(NET_CONTROLLER_NONE); // Controller shouldn't be set as remote objects are synched as "remote".

	// Trust the packet wont change the instance name, and copy
	// it directly to the creation update stream.
	unsigned char* psStream = const_cast<unsigned char*>( reinterpret_cast<const unsigned char*>( m_pBaseInstance->GetName().c_str() ) );
	creationUpdate.set_data(psStream, m_pBaseInstance->GetName().size() + 1);
	
	// Copy the creation update into a stream component which will
	// be appended to the reliable stream.
	SnapshotStream::Stream creationStreamNewObject;
	creationUpdate.write(creationStreamNewObject);
	reliableStream.get()->push_back(creationStreamNewObject);


	m_pBaseInstance->DoSnapshot(reliableStream, true, false);
	m_pBaseInstance->DoSnapshot(unreliableStream, false, false);

	auto object_list = m_NetworkObjects.get();

	// With the creation update, broadcast any delta changes made
	// after the object creation is made (incase the new object has 
	// changes to its netvars).
	m_NetworkObjects.safe_lock();
	object_list->push_back(built_ent);

	// Find all current object states and send them to the new peer.

	if (pOwner != nullptr)
	{
		SendObjectLists( pOwner );
	}

	// Broadcast the creation update.
	if (reliableStream.modified())
		pServer->BroadcastStream(reliableStream, true);

	if (unreliableStream.modified())
		pServer->BroadcastStream(unreliableStream, false);

	// Clear streams to make way for full snapshot stream.
	reliableStream.clear();
	unreliableStream.clear();

	// Inform this object to player.
	if (built_ent->m_pPeer)
	{
		// Get a full snapshot of the current object list to send to the new player.
		m_pBaseInstance->DoSnapshot(reliableStream, true, true);
		m_pBaseInstance->DoSnapshot(unreliableStream, false, true);

		if (reliableStream.modified())
			pServer->SendStream(reliableStream, true, built_ent->m_pPeer);

		// Reliable objects should be reliably sent as this is the first
		// initial floatation of the new entitiy.
		if (unreliableStream.modified())
			pServer->SendStream(unreliableStream, true, built_ent->m_pPeer);
	}

	m_NetworkObjects.safe_unlock();

	// Emit callback event.
	if (m_fCallbackCreate != nullptr)
		m_fCallbackCreate(built_ent.get());

	// Emit initialisation virtual func.
	built_ent->Initialize();
	return built_ent;
}

GEN_TEMPLATE_FN(CEntities, void)::SendObjectLists(ENetPeer* pOwner)
{
	auto object_list = m_NetworkObjects.get();
	
	SnapshotEntList creationUpdate;
	creationUpdate.set_frametype(FrameType::kFRAME_Alloc);
	creationUpdate.set_name(m_ManagerName);

	// Set the instance name by casting in the name's ponter and copying
	// its contents to the buffer.
	char* instance_name = const_cast<char*>(m_pBaseInstance->GetName().c_str());
	creationUpdate.set_data(reinterpret_cast<unsigned char*>(instance_name), m_pBaseInstance->GetName().size() + 1);

	for (auto active_object = object_list->begin(); active_object != object_list->end(); ++active_object)
	{
		std::shared_ptr<NetObject>& netObj = (*active_object);
		ENetPeer* pObjectPeer = netObj->m_pPeer;

		std::uint32_t controllerStatus;

		if (pOwner != nullptr && pObjectPeer == pOwner)
			controllerStatus = NET_CONTROLLER_LOCAL;
		else
			controllerStatus = NET_CONTROLLER_NONE;

		creationUpdate.set_netid(netObj->GetNetID());
		creationUpdate.set_controller(controllerStatus);

		// Creates a new stream to sync the current instance state to
		// the new pOwner peer.
		SnapshotStream::Stream creationStreamExistingObject;
		creationUpdate.write(creationStreamExistingObject);

		// Set up a reliable stream for allocating new net IDs.
		SnapshotStream creationStreamReliable;
		creationStreamReliable.get()->push_back(creationStreamExistingObject);

		// Generate this packet's header.
		SnapshotHeader::Generate(creationStreamReliable, 0, 1, creationStreamExistingObject.size());

		// Send it to the new peer.
		m_pGlobalController->SendStream(creationStreamReliable, true, pOwner);
		netObj->SendLists(pOwner);
	}
}

GEN_TEMPLATE_FN(CEntities, std::shared_ptr<NetObject>)::BuildLocal(NetObject::NetID netID, std::uint32_t controller)
{
	auto built_ent = std::make_shared<TypeObject>();
	built_ent->SetNetID( netID );
	built_ent->m_pInstance = m_pBaseInstance;
	built_ent->m_pManager = this;
	built_ent->SetController( controller );
	built_ent->SetIsReplicated( true );

	// Move the new object into the manager lists.
	std::unique_lock<std::recursive_mutex> guard(m_NetworkObjects.mutex());
	m_NetworkObjects.get()->push_back(built_ent);
	guard.unlock();

	if (built_ent->IsNetworkLocal())
		m_NetworkObjectsLocal.push_back(built_ent);

	// Dispatch callback.
	if (m_fCallbackCreate)
	{
		m_fCallbackCreate(built_ent.get());
	}

	built_ent->Initialize();
	return built_ent;
}

GEN_TEMPLATE_FN(CEntities, void)::FreeListEnt(EntityList& list, NetObject::NetID id)
{
	std::unique_lock<std::recursive_mutex> guard(list.mutex());
	auto net_list = list.get();

	for (auto obj_it = net_list->begin(); obj_it != net_list->end();)
	{
		std::shared_ptr<NetObject>& net_obj = (*obj_it);
		
		if (net_obj->GetNetID() == id)
		{
			obj_it = net_list->erase(obj_it);
			return;
		}
		else
		{
			++ obj_it;
		}
	}
}

GEN_TEMPLATE_FN(CEntities, void)::FreeLocal(NetObject::NetID netID)
{
	FreeListEnt(m_NetworkObjects,      netID);
	FreeListEnt(m_NetworkObjectsLocal, netID);

	CMessageDispatcher::Add(kMessageType_Warn, "Tried to free local entity without it existing in local list (desync?)");
}


GEN_TEMPLATE_FN(CEntities, std::shared_ptr<TypeObject>)::GetControlled(std::uint32_t index)
{
	auto net_list = m_NetworkObjectsLocal.get();
	std::unique_lock<std::recursive_mutex> guard(m_NetworkObjectsLocal.mutex());

	std::uint32_t current_index = 0;

	for (auto obj = net_list->begin(); obj != net_list->end(); ++obj)
	{
		std::shared_ptr<NetObject>& net_obj = (*obj);

		if (net_obj->IsNetworkLocal() && current_index == index)
		{
			return std::dynamic_pointer_cast<TypeObject>(net_obj);
		}
		
		++ current_index;
	}

	return nullptr;
}

/*
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
*/
/*
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
*/

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
	m_NetworkObjects.safe_lock();
	m_NetworkObjectsLocal.safe_lock();

	for (auto it = m_NetworkObjects.get()->begin(); it != m_NetworkObjects.get()->end();)
		it = m_NetworkObjects.get()->erase(it);

	for (auto it = m_NetworkObjectsLocal.get()->begin(); it != m_NetworkObjectsLocal.get()->end();)
		it = m_NetworkObjectsLocal.get()->erase(it);

	m_NetworkObjectsLocal.safe_unlock();
	m_NetworkObjects.safe_unlock();
}

template <class TypeObject>
inline size_t CEntities<TypeObject>::CountMaintained()
{
	size_t result;
	
	m_NetworkObjectsLocal.safe_lock();
	result = m_NetworkObjectsLocal.get()->size();
	m_NetworkObjectsLocal.safe_unlock();
	return result;
}

#undef GEN_TEMPLATE_FN