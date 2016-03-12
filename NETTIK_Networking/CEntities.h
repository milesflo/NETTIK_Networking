#pragma once
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <algorithm>
#include <inttypes.h>
#include "LockableVector.h"

// TODO: Make CVector3 fully established class for work on Professional Skills assignment (take from TL-Source?)
#include "CVector3.h"
#include "INetworkPacketFactory.hpp"
#include "EntityMessages.pb.h"

// Network entity specific objects
#include "NetObject.h"
#include "NetVar.h"

extern uint32_t m_TotalEntities;

// Reserved network IDs
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

template< class VarType >
class CNetVarBase : public NetVar
{
private:
	bool        m_bChanged = true;

protected:
	VarType*     m_Data = nullptr;

	bool SetGuard(VarType& data)
	{
		if (!m_pParent->m_Active)
			return false;

		if (m_Data == nullptr)
			return false;

		if (*m_Data == data)
			return false;
		m_bChanged = true;

		return true;
	}

public:

	CNetVarBase(const CNetVarBase& from)
	{
		m_Name = from.m_Name;
		m_bChanged = from.m_bChanged;
	}

	size_t TakeVariableSnapshot(std::vector<unsigned char>& buffer, bool bForced)
	{
		// Variable hasn't changed and snapshot isn't forced.
		if (!bForced && !m_bChanged)
			return 0;

		if (!m_Data)
			return 0;

		// This is slow AF.
		// TODO: Pass character buffer and fill and update
		// an index of the current stream length.
		uint32_t code;
		code = NETID_Reserved::RTTI_Object::OBJECT_DAT;

		// | netmsg code |
		for (size_t i = 0; i < sizeof(NETTIK::INetworkCodes::msg_t); i++)
			buffer.push_back(((unsigned char*)(&code))[i]);

		// | network id |
		for (size_t i = 0; i < sizeof(uint32_t); i++)
			buffer.push_back(((unsigned char*)(&m_pParent->m_NetCode))[i]);

		// | varname |
		for (size_t i = 0; i < strlen(m_Name); i++)
			buffer.push_back((((m_Name)[i])));
		buffer.push_back(0); // NULL terminate

		// | data buffer ....
		for (size_t i = 0; i < sizeof(VarType); i++)
			buffer.push_back(((unsigned char*)(m_Data))[i]);

		m_bChanged = false;
		return buffer.size();
	}

	CNetVarBase(NetObject* parent, const char* name, bool reliable) : NetVar(parent, name, reliable)
	{
		m_Data = new VarType();
		*m_Data = VarType();
	}


	virtual ~CNetVarBase()
	{
		delete(m_Data);
		m_Data = nullptr;
	}

	virtual void Set(VarType data)
	{
		if (!SetGuard(data))
			return;

		*m_Data = data;
	}
};

class CNetVar_Vector3 : public CNetVarBase<NETTIK::CVector3>
{
public:
	CNetVar_Vector3(NetObject* parent, const char* name, bool reliable) : CNetVarBase(parent, name, reliable)
	{

	}

	void Set(float x, float y, float z)
	{
		NETTIK::CVector3 compose(x, y, z);
		if (!SetGuard(compose))
			return;

		*m_Data = compose;
	}
	virtual ~CNetVar_Vector3() { }
};

#define CNetVar(type, name, reliable)    CNetVarBase<type> name = CNetVarBase<type>(this, #name, reliable)
#define CNetVarVector3(name, reliable)   CNetVar_Vector3   name = CNetVar_Vector3(this, #name, reliable)

class NetObject_Test : public NetObject
{
public:
	DEFINE_NETOBJECT("NetObject_Test");
	CNetVar(int, m_iTest, true);

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

	virtual void PostUpdate() = 0;
	virtual void GetSnapshot(size_t& max_value, uint16_t& num_updates, std::vector<std::vector<unsigned char>>& buffers, bool bReliableFlag, bool bForced) = 0;

	virtual uint32_t SERVER_Add(void* object) = 0;
	virtual bool SERVER_Remove(uint32_t entityCode) = 0;

	virtual ~IEntityManager() { }
};

template <typename EntityType>
class CEntities : public IEntityManager
{
private:
	LockableVector<EntityType*> m_Objects;
	std::string m_name;
	VirtualInstance* m_pBaseInstance;

public:

	void PostUpdate()
	{
		m_Objects.safe_lock();
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
			(*it)->Update();
		m_Objects.safe_unlock();
	}

	void GetSnapshot(size_t& max_value, uint16_t& num_updates, std::vector<std::vector<unsigned char>>& buffers, bool bReliableFlag, bool bForced = false)
	{
//		printf("snapshot.\n");
		m_Objects.safe_lock();
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); ++it)
		{
			(*it)->TakeObjectSnapshot(max_value, num_updates, buffers, bReliableFlag, bForced);
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

	uint32_t SERVER_Add(void* _ObjectPtr)
	{
		EntityType* object;
		object = static_cast<EntityType*>(_ObjectPtr);

		object->m_NetCode = (m_TotalEntities++);
		object->m_pInstance = m_pBaseInstance;
		
		m_Objects.safe_lock();
		m_Objects.get()->push_back(object);
		
		for (auto it = m_Objects.get()->begin(); it != m_Objects.get()->end(); it++)
		{
			// Inform this object to player.
			if (object->m_pPeer)
			{
				//(*it)->Serialize(object->m_pPeer);

				m_pBaseInstance->DoSnapshot(true, true, object->m_pPeer);
				m_pBaseInstance->DoSnapshot(false, true, object->m_pPeer);
			}

			// Tell this object of this new object.
			if ((*it)->m_pPeer)
			{
				//object->Serialize((*it)->m_pPeer);
				m_pBaseInstance->DoSnapshot(true, true, (*it)->m_pPeer);
				m_pBaseInstance->DoSnapshot(false, true, (*it)->m_pPeer);
			}
		}

		m_Objects.safe_unlock();
		return object->m_NetCode;
	}

	bool SERVER_Remove(uint32_t code)
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

	}

	virtual ~CEntities()
	{
	}
};

template <class EntityType>
using CEntities_ptr = std::unique_ptr<CEntities<EntityType>>;