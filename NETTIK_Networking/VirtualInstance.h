#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "CEntities.h"

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
	typedef typename std::unordered_map<std::string, IEntityManager*>::iterator mgrvec_it;
	std::vector<mgrvec_it> m_PendingDeletes;
public:

	// Gets instance's name.
	inline std::string GetName() const
	{
		return m_sName;
	}

	void DoPostUpdate();
	void DoSnapshot(bool bReliableFlag, bool bForced = false, ENetPeer* peer = nullptr);

	void DestroyEntityManager(std::string name);
	void DestroyEntityManager(IEntityManager* mgr);

	VirtualInstance(std::string name, NETTIK::IController* controller);
	virtual ~VirtualInstance();

	template <class EntityType>
	IEntityManager* CreateEntityManager(std::string name)
	{
		IEntityManager* mgr = new CEntities<EntityType>(this);
		mgr->SetName(name);

		m_EntManagers.insert(make_pair(name, mgr));
		return mgr;
	}


	template <class EntityType>
	CEntities<EntityType>* GetEntityManager(std::string name)
	{
		auto it = m_EntManagers.find(name);
		if (it != m_EntManagers.end())
			return dynamic_cast<CEntities<EntityType>*>((*it).second);
		else
			return nullptr;
	}

};
using VirtualInstance_ptr = std::unique_ptr<VirtualInstance>;