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

extern uint32_t m_TotalEntities;

// Reserved network IDs for entity manipulation.
namespace NETID_Reserved
{
	enum RTTI_Object
	{
		OBJECT_NEW = 0xFFE0,
		OBJECT_DEL = 0xFFE1,
		OBJECT_DAT = 0xFFE2,

		SNAPSHOT   = 0xFFE3,
	};
};

// Forward delcare some stuff.
namespace NETTIK
{
	class IController;
	class IControllerServer;
}


template <typename EntityType>
class CEntities : public IEntityManager
{
private:
	LockableVector<EntityType*> m_Objects;
	std::string      m_name;
	VirtualInstance* m_pBaseInstance;

	NETTIK::IController*   m_pGlobalController;
public:

	void PostUpdate()
	{
		m_Objects.safe_lock();
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
			(*it)->Update();
		m_Objects.safe_unlock();
	}

	void GetSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& stream, bool bReliableFlag, bool bForced = false)
	{
//		printf("snapshot.\n");
		m_Objects.safe_lock();
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
		{
			(*it)->TakeObjectSnapshot(max_value, num_updates, stream, bReliableFlag, bForced);
		}
		m_Objects.safe_unlock();
	}

	inline void SetName(std::string name)
	{
		m_name = name;
	}
	
	inline std::string GetName() const
	{
		return m_name;
	}

	uint32_t Add(void* _ObjectPtr)
	{
		EntityType* object;
		object = static_cast<EntityType*>(_ObjectPtr);

		object->m_NetCode = (m_TotalEntities++);
		object->m_pInstance = m_pBaseInstance;
		
		m_Objects.safe_lock();

		NETTIK::IControllerServer* server;
		server = dynamic_cast<IControllerServer*>(m_pGlobalController);

		if (!server)
			NETTIK_EXCEPTION("Cannot stream objects if controller isn't a server service.");

		SnapshotStream reliableStream;
		SnapshotStream unreliableStream;

		m_pBaseInstance->DoSnapshot(reliableStream, true, true);
		m_pBaseInstance->DoSnapshot(unreliableStream, false, true);
	
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); it++)
		{

			// Tell this object of this new object.
			if ((*it)->m_pPeer)
			{
				//object->Serialize((*it)->m_pPeer);

				if (reliableStream.modified())
					server->SendStream(reliableStream, true, (*it)->m_pPeer);

				if (unreliableStream.modified())
					server->SendStream(unreliableStream, false, (*it)->m_pPeer);
			}
		}

		// Inform this object to player.
		if (object->m_pPeer)
		{
			//(*it)->Serialize(object->m_pPeer);

			if (reliableStream.modified())
				server->SendStream(reliableStream, true, object->m_pPeer);

			if (unreliableStream.modified())
				server->SendStream(unreliableStream, false, object->m_pPeer);
		}

		m_Objects.get()->push_back(object);

		m_Objects.safe_unlock();
		return object->m_NetCode;
	}

	bool Remove(uint32_t code)
	{
		m_Objects.safe_lock();

		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end();)
		{
			if ((*it)->m_NetCode == code)
			{
				if ((*it)->m_pPeer != nullptr)
				{
					// Delete all entities on peer.
				}

				m_Objects.get()->erase(it);
				m_Objects.safe_unlock();

				return true;
			}
			else
				it++;
		}

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
	}
};

template <class EntityType>
using CEntities_ptr = std::unique_ptr<CEntities<EntityType>>;