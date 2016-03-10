#pragma once
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <inttypes.h>
#include "CVector3.h"
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
		OBJECT_DAT = 0xFFE2,

		SNAPSHOT   = 0xFFE3,
	};
};

class INetVar;
class VirtualInstance;

class INetObject
{
public:
	uint32_t          m_NetCode;
	std::unordered_map<std::string, INetVar*> m_Vars;
	bool              m_Active = true;
	// These are set by DEFINE_NETOBJECT.
	virtual uint32_t      GetNetObject_Type() const = 0;
	virtual std::string   GetNetObject_Name() const = 0;
	
	// Snapshotting
	// - size_t result is the LARGEST update element (for padding)
	void GetObjectSnapshot(size_t& max_value, uint16_t& num_update, std::vector<std::vector<unsigned char>>& buffers, bool bForced = false);

	// Accessors for ENET peer if this has one assigned.
	ENetPeer* m_pPeer = nullptr;
	VirtualInstance* m_pInstance = nullptr;
	std::recursive_mutex m_Mutex;

public:
	virtual ~INetObject() { };
};


class INetVar
{
protected:
	INetObject* m_pParent;
	const char* m_Name;
public:
	virtual size_t Snapshot(std::vector<unsigned char>& buffer, bool bForced) = 0;
	
	inline const char* GetName() const { return m_Name; }
	inline INetObject* GetParent() const { return m_pParent; }

	INetVar(INetObject* parent, const char* name) : m_Name(name), m_pParent(parent)
	{
		m_pParent->m_Vars[m_Name] = this;
	}

	virtual ~INetVar()
	{
//		m_pParent->m_Vars.erase(m_Name);
	}
};

template< class VarType >
class CNetVarBase : public INetVar
{
private:
	bool        m_bChanged;

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

	size_t Snapshot(std::vector<unsigned char>& buffer, bool bForced)
	{
		// Variable hasn't changed and snapshot isn't forced.
		if (!bForced && !m_bChanged)
			return 0;

		if (!m_Data)
			return 0;

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
		return sizeof(NETTIK::INetworkCodes::msg_t) + sizeof(uint32_t) + (sizeof(m_Name) / sizeof(char)) + sizeof(VarType);
	}

	CNetVarBase(INetObject* parent, const char* name) : INetVar(parent, name)
	{
		m_Data = new VarType;
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
	CNetVar_Vector3(INetObject* parent, const char* name) : CNetVarBase(parent, name)
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

#define CNetVar(type, name) \
	CNetVarBase<type> name = CNetVarBase<type>(this, #name)
#define CNetVarVector3(name) \
	CNetVar_Vector3 name = CNetVar_Vector3(this, #name)

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

	virtual void ProcessDeletes() = 0; 
	virtual void PostUpdate() = 0;
	virtual void GetSnapshot(size_t& max_value, uint16_t& num_updates, std::vector<std::vector<unsigned char>>& buffers, bool bForced) = 0;

	virtual uint32_t SERVER_Add(void* object) = 0;
	virtual bool SERVER_Remove(uint32_t entityCode) = 0;

	virtual ~IEntityManager() { }
};

template <typename EntityType>
class CEntities : public IEntityManager
{
private:
	std::vector<EntityType*> m_Objects;
	typedef typename std::vector<EntityType*>::iterator entvec_it;
	std::vector<entvec_it> m_PendingDeletes;
	std::string m_name;
	VirtualInstance* m_pBaseInstance;


	std::recursive_mutex m_Mutex;
public:

	void ProcessDeletes()
	{
		for (auto it = m_PendingDeletes.begin(); it != m_PendingDeletes.end();)
		{
			m_Objects.erase(*it);
			it = m_PendingDeletes.erase(it);
		}
	}

	void PostUpdate()
	{
		for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it)
			(*it)->Update();
	}

	void GetSnapshot(size_t& max_value, uint16_t& num_updates, std::vector<std::vector<unsigned char>>& buffers, bool bForced = false)
	{
		for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it)
		{
			(*it)->GetObjectSnapshot(max_value, num_updates, buffers, bForced);
		}
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
		m_Objects.push_back(object);

		for (auto it = m_Objects.begin(); it != m_Objects.end(); it++)
		{
			// Inform this object to player.
			if (object->m_pPeer)
			{
				//(*it)->Serialize(object->m_pPeer);
			}

			// Tell this object of this new object.
			if ((*it)->m_pPeer)
			{
				//object->Serialize((*it)->m_pPeer);
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

				m_PendingDeletes.push_back(it);
				return true;
			}
			else
				it++;
		}

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