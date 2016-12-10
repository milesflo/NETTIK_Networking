#include "IEntityManager.h"

uint32_t s_TotalEntities = 0;

//-------------------------------------------------
// Returns a mutex locked vector of the internal
// objects.
//-------------------------------------------------
LockableVector<std::shared_ptr<NetObject>>& IEntityManager::GetObjects()
{
	return m_NetworkObjects;
}

void IEntityManager::SetName(std::string name)
{
	// Make sure the name doesn't run over the networked
	// snapshot name limit.
	if (name.size() > max_entmgr_name)
		NETTIK_EXCEPTION("Entity manager name length exceeds MAX_ENTMGR_NAME.");

	m_ManagerName = name;
}

const std::string&  IEntityManager::GetName() const
{
	return m_ManagerName;
}

//-------------------------------------------------
// Gets the next UID of the application. Increments
// the next ID on call.
//-------------------------------------------------
std::uint32_t IEntityManager::GetNextID()
{
	return (s_TotalEntities++);
}

size_t IEntityManager::Count()
{
	size_t result;
	result = m_NetworkObjects.get()->size();

	return result;
}

IEntityManager::~IEntityManager()
{

}
