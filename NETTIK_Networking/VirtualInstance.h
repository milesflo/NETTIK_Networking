//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "CEntities.h"
#include "SnapshotStream.h"

class NetSystem;
class NetSystemServer;
class IEntityManager;
class NetObject;

class VirtualInstance
{
protected:
	std::unordered_map<std::string, IEntityManager*> m_EntManagers;

	NetSystem* m_ParentController;
	std::string          m_sName;
	
	uint32_t             m_iSequenceID = 0;
	typedef std::unordered_map<std::string, IEntityManager*>::iterator mgrvec_it;
	std::vector<mgrvec_it> m_PendingDeletes;
public:

	//------------------------------------------------------
	// Sends a creation stream list to the target peer. 
	// Can only be called on server as server always has
	// creation authoristaion and clients dont.
	//------------------------------------------------------
	void SendAllObjectLists(ENetPeer* pOwner);

	std::shared_ptr<NetObject> FindObject(NetObject::NetID netid);

	//! Gets instance's name.
	std::string& GetName();

	void DoPostUpdate(float elapsedTime);
	void DoSnapshot(SnapshotStream& collection, bool bReliableFlag, bool bForced = false);
	void DoListUpdate();

	void DestroyEntityManager(std::string name);
	void DestroyEntityManager(IEntityManager* mgr);

	VirtualInstance(std::string name, NetSystem* controller);
	virtual ~VirtualInstance();

	template <class T>
	IEntityManager* CreateManager(std::string name, IEntityManager::EventCallback<T> callbackCreate = nullptr, IEntityManager::EventCallback<T> callbackDelete = nullptr)
	{
		CEntities<T>* mgr = new CEntities<T>(this);
		mgr->SetName(name);
		mgr->SetCallbackCreate(callbackCreate);
		mgr->SetCallbackDelete(callbackDelete);

		m_EntManagers.insert(make_pair(name, mgr));
		return mgr;
	}

	template <class T>
	CEntities<T>* GetManager(std::string name)
	{
		auto it = m_EntManagers.find(name);
		if (it != m_EntManagers.end())
			return dynamic_cast<CEntities<T>*>((*it).second);
		else
			return nullptr;
	}

	IEntityManager* GetManager(std::string name)
	{
		auto it = m_EntManagers.find(name);
		if (it != m_EntManagers.end())
			return dynamic_cast<IEntityManager*>((*it).second);
		else
			return nullptr;
	}

};

//! Gets instance's name.
inline std::string& VirtualInstance::GetName()
{
	return m_sName;
}
