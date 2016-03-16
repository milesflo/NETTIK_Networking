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
#include "CVector3.h"
#include "INetworkPacketFactory.hpp"
#include "EntityMessages.pb.h"

// Network entity specific objects
#include "NetObject.h"
#include "NetVar.h"
#include "CNetVarBase.h"
#include "SnapshotStream.h"

#include "Constraints.h"
#include "NETIDReserved.h"
extern uint32_t m_TotalEntities;

// Forward delcare some stuff.
namespace NETTIK
{
	class IController;
	class IControllerServer;
}

template <class TypeObject>
class CEntities : public IEntityManager
{
private:
	LockableVector<TypeObject*> m_MaintainedObjects;

	LockableVector<NetObject*> m_Objects;
	std::unordered_map<uint32_t, NetObject*> m_ObjectRefs;

	std::string      m_name;
	VirtualInstance* m_pBaseInstance;

	NETTIK::IController*   m_pGlobalController;
public:

	inline std::string GetName() const
	{
		return m_name;
	}

	NetObject* AddLocal(uint32_t netid)
	{

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
		object->SetIsServer(false);

		// For fast lookup.
		m_ObjectRefs[object->m_NetCode] = object;

		m_Objects.get()->push_back(object);
		m_MaintainedObjects.get()->push_back(instance);

		m_Objects.safe_unlock();
		m_MaintainedObjects.safe_unlock();

		return instance;
	}

	bool RemoveLocal(uint32_t code)
	{
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
				delete(*it);
				m_MaintainedObjects.get()->erase(it);
				break;
			}
			else
				++it;
		}
		m_MaintainedObjects.safe_unlock();

		return false;
	}

	NetObject* GetByNetID(uint32_t id)
	{
		auto it = m_ObjectRefs.find(id);
		if (it == m_ObjectRefs.end())
			return nullptr;

		return dynamic_cast<NetObject*>(it->second);
	}

	void PostUpdate()
	{
		if (!m_pGlobalController->IsRunning())
			return;

		m_Objects.safe_lock();

		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
			(*it)->Update();

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

	void SetName(std::string name)
	{
		if (name.size() > max_entmgr_name)
			NETTIK_EXCEPTION("Entity manager name length exceeds MAX_ENTMGR_NAME.");

		m_name = name;
	}

	uint32_t Add(NetObject* object)
	{
		object->m_NetCode = (m_TotalEntities++);
		object->m_pInstance = m_pBaseInstance;
		object->m_pManager = this;

		// For fast lookup.
		m_ObjectRefs[object->m_NetCode] = object;

		NETTIK::IControllerServer* server;
		server = dynamic_cast<NETTIK::IControllerServer*>(m_pGlobalController);

		if (!server)
			NETTIK_EXCEPTION("Cannot stream objects if controller isn't a server service.");

		SnapshotStream reliableStream;
		SnapshotStream unreliableStream;

		m_Objects.safe_lock();
		m_Objects.get()->push_back(object);

		SnapshotEntList creationUpdate;
		creationUpdate.set_frametype(FrameType::kFRAME_Alloc);
		creationUpdate.set_name(m_name);
		creationUpdate.set_netid(object->m_NetCode);

		char* instance_name = const_cast<char*>(m_pBaseInstance->GetName().c_str());
		creationUpdate.set_data(reinterpret_cast<unsigned char*>(instance_name), m_pBaseInstance->GetName().size() + 1);

		SnapshotStream::Stream creationStream;
		creationUpdate.write(creationStream);

		reliableStream.get()->push_back(creationStream);

		m_pBaseInstance->DoSnapshot(reliableStream, true, true);
		m_pBaseInstance->DoSnapshot(unreliableStream, false, true);

		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
		{
			// Update the creation update list to the current object
			creationUpdate.set_netid((*it)->m_NetCode);

			// Create a new stream to synch the current instance state to the new
			// peer object.
			SnapshotStream::Stream creationStream;
			creationUpdate.write(creationStream);

			// Set up a reliable stream for allocating new net IDs.
			SnapshotStream creationStreamReliable;
			creationStreamReliable.get()->push_back(creationStream);

			// Generate this packet's header.
			SnapshotHeader::Generate(creationStreamReliable, 0, 1, creationStream.size());

			// Send it to the new peer.
			server->SendStream(creationStreamReliable, true, object->m_pPeer);

			// Inform all the objects of the snapshot changes (new objects)
			if ((*it)->m_pPeer)
			{
				//object->Serialize((*it)->m_pPeer);
				server->SendStream(creationStreamReliable, true, (*it)->m_pPeer);

			}
		}

		if (reliableStream.modified())
			server->BroadcastStream(reliableStream, true);

		if (unreliableStream.modified())
			server->BroadcastStream(unreliableStream, false);


		// Inform this object to player.
		if (object->m_pPeer)
		{
			//(*it)->Serialize(object->m_pPeer);

			if (reliableStream.modified())
				server->SendStream(reliableStream, true, object->m_pPeer);

			if (unreliableStream.modified())
				server->SendStream(unreliableStream, false, object->m_pPeer);
		}

		m_Objects.safe_unlock();
		return object->m_NetCode;
	}

	bool Remove(uint32_t code)
	{

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

		SnapshotStream::Stream deletionStream;
		deletionUpdate.write(deletionStream);

		SnapshotStream reliableStream;
		reliableStream.get()->push_back(deletionStream);

		// Generate this packet's header.
		SnapshotHeader::Generate(reliableStream, 0, 1, deletionStream.size());

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

		server->BroadcastStream(reliableStream, true);

		m_Objects.safe_unlock();
		return false;
	}

	CEntities(VirtualInstance* baseInstance)
	{
		m_pBaseInstance = baseInstance;

		NETTIK::IController* controller;
		controller = NETTIK::IController::GetPeerSingleton();

		if (!controller)
			throw std::runtime_error("IController not found.");

		m_pGlobalController = controller;
	}

	virtual ~CEntities()
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
};
