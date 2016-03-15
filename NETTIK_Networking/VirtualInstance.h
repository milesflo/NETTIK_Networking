#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "CEntities.h"
#include "SnapshotStream.h"
#include "NetObject.h"

namespace NETTIK
{
	class IController;
};

class VirtualInstance
{
private:
	std::unordered_map<std::string, IEntityManager*> m_EntManagers;

	NETTIK::IController* m_ParentController;
	std::string          m_sName;
	
	uint32_t             m_iSequenceID = 0;
	typedef std::unordered_map<std::string, IEntityManager*>::iterator mgrvec_it;
	std::vector<mgrvec_it> m_PendingDeletes;
public:

	NetObject* FindObject(uint32_t netid)
	{

		for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
		{
			NetObject* entry;
			entry = it->second->GetByNetID(netid);

			if (entry != nullptr)
				return entry;

		}

		return nullptr;
	}

	//! Gets instance's name.
	inline std::string& GetName()
	{
		return m_sName;
	}

	void DoPostUpdate();
	void DoSnapshot(SnapshotStream& collection, bool bReliableFlag, bool bForced = false);

	void DestroyEntityManager(std::string name);
	void DestroyEntityManager(IEntityManager* mgr);

	VirtualInstance(std::string name, NETTIK::IController* controller);
	virtual ~VirtualInstance();

	template <class T>
	IEntityManager* CreateEntityManager(std::string name)
	{
		IEntityManager* mgr = new CEntities<T>(this);
		mgr->SetName(name);

		m_EntManagers.insert(make_pair(name, mgr));
		return mgr;
	}

	template <class T>
	CEntities<T>* GetEntityManager(std::string name)
	{
		auto it = m_EntManagers.find(name);
		if (it != m_EntManagers.end())
			return dynamic_cast<CEntities<T>*>((*it).second);
		else
			return nullptr;
	}

	IEntityManager* GetEntitiyManagerInterface(std::string name)
	{
		auto it = m_EntManagers.find(name);
		if (it != m_EntManagers.end())
			return dynamic_cast<IEntityManager*>((*it).second);
		else
			return nullptr;
	}

};
using VirtualInstance_ptr = std::unique_ptr<VirtualInstance>;