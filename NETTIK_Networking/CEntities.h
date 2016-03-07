#pragma once
#include <vector>
#include <string>
#include <memory>
#include <inttypes.h>
#include "INetworkPacketFactory.hpp"
#include "EntityMessages.pb.h"
#define DEFINE_NETOBJECT(typeName, typeID) \
	uint32_t    GetNetObject_Type() const { return typeID; } \
	std::string GetNetObject_Name() const { return typeName; } \
	\
	\
	static constexpr uint32_t  GetNetObject_Type_STATIC() { return typeID; }

extern uint32_t m_TotalEntities;

// Reserved network IDs
namespace NETID_Reserved
{
	enum RTTI_Object
	{
		OBJECT_NEW = 0xFFE0,
		OBJECT_DEL = 0xFFE1,
		OBJECT_DAT = 0xFFE2
	};
};

class VirtualInstance;
class INetObject
{
public:
	uint32_t          m_NetCode;

	// These are set by DEFINE_NETOBJECT.
	virtual uint32_t      GetNetObject_Type() const = 0;
	virtual std::string   GetNetObject_Name() const = 0;
	
	// Functions to serialize contents
	void SerializeNetInfo(ENetPeer* destPeer)
	{
		//NETTIK::IPacketFactory::NetPacket<IMessageNewObject_New> packet(NETID_Reserved::OBJECT_NEW);

		//// Type specfic types (for RTTI or some shit)
		//packet.set_name(GetNetObject_Name());
		//packet.set_type(GetNetObject_Type());

		//// Unique data
		//packet.set_id(m_NetCode);

		//packet._PeerSet(destPeer);
		//packet._ForceDispatch();
	}

	virtual void SerializeUniqueInfo(ENetPeer* destPeer)
	{
	};

	void Serialize(ENetPeer* destPeer)
	{
		SerializeNetInfo(destPeer);
		SerializeUniqueInfo(destPeer);
	}

	void Deserialize(std::vector<ENetPeer*> destPeers)
	{
		//NETTIK::IPacketFactory::NetPacket<IMessageNewObject_Del> packet(NETID_Reserved::OBJECT_DEL);
		//packet.set_id(m_NetCode);

		//for (auto it = destPeers.begin(); it != destPeers.end(); it++)
		//	packet._PeerAdd(*it);

		//packet._ForceDispatch();
	}

	// Accessors for ENET peer if this has one assigned.
	ENetPeer* m_pPeer = nullptr;
	VirtualInstance* m_pInstance = nullptr;
public:
	virtual ~INetObject() { };
};

template< class VarType >
class CNetVarBase
{
private:
	VarType m_Data;

public:
	void Transmit()
	{

	}
	void Set(VarType data)
	{
		if (m_Data == data)
			return;

		m_Data = data;
		Transmit();
	}
};

#define CNetVar(type, name) \
	CNetVarBase<type> name;

class NetObject_Test : public INetObject
{
public:
	DEFINE_NETOBJECT("NetObject_Test", 0xFF);
	CNetVar(int, m_iTest);

	NetObject_Test()
	{
		printf("NetObject::Test - Constructed new object.\n");
	}
	virtual ~NetObject_Test()
	{
		printf("NetObject::Test - Destructed object.\n");
	}
};

class IEntityManager
{
public:
	virtual void SetName(std::string name) = 0;
	virtual std::string GetName() const = 0;

	virtual void Update(std::vector<NETTIK::IPacketFactory::CBasePacket*>& queue) = 0;
	virtual uint32_t SERVER_Add(void* object) = 0;
	virtual bool SERVER_Remove(uint32_t entityCode) = 0;
	virtual ~IEntityManager() { }
};

template <class EntityType>
class CEntities : public IEntityManager
{
private:
	std::vector<EntityType*> m_Objects;
	std::string m_name;
	VirtualInstance* m_pBaseInstance;

public:

	inline void SetName(std::string name)
	{
		m_name = name;
	}
	
	inline std::string GetName() const
	{
		return m_name;
	}

	uint32_t SERVER_Add(void* _ObjectPtr)
	{
		EntityType* object;
		object = static_cast<EntityType*>(_ObjectPtr);

		object->m_NetCode = (m_TotalEntities++);
		object->m_pInstance = m_pBaseInstance;
		m_Objects.push_back(object);

		for (auto it = m_Objects.begin(); it != m_Objects.end(); it++)
		{
			// Inform this object to player.
			if (object->m_pPeer)
			{
				(*it)->Serialize(object->m_pPeer);
			}

			// Tell this object of this new object.
			if ((*it)->m_pPeer)
			{
				object->Serialize((*it)->m_pPeer);
			}
		}

		return object->m_NetCode;
	}

	bool SERVER_Remove(uint32_t code)
	{
		for (auto it = m_Objects.begin(); it != m_Objects.end();)
		{
			if ((*it)->m_NetCode == code)
			{
				if ((*it)->m_pPeer != nullptr)
				{
					// Delete all entities on peer.
				}
				it = m_Objects.erase(it);
				return true;
			}
			else
				it++;
		}

		return false;
	}



	void Update(std::vector<NETTIK::IPacketFactory::CBasePacket*>& queue)
	{

	}

	CEntities(VirtualInstance* baseInstance)
	{
		m_pBaseInstance = baseInstance;

		NETTIK::IController* controller;
		controller = NETTIK::IController::GetPeerSingleton();

		if (!controller)
			throw std::runtime_error("IController not found.");

	}

	virtual ~CEntities() { }
};

template <class EntityType>
using CEntities_ptr = std::unique_ptr<CEntities<EntityType>>;