#include "VirtualInstance.h"
#include "IController.hpp"
#include "SnapshotHeader.h"
#include <vector>
using namespace NETTIK;

void VirtualInstance::DoPostUpdate()
{
	// Process pending ent manager deletions.
	for (auto it = m_PendingDeletes.begin(); it != m_PendingDeletes.end(); )
	{
		delete((*it)->second);
		m_EntManagers.erase(*it);
		it = m_PendingDeletes.erase(it);
	}
	// Process ent managers still in list.
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
	{
		it->second->PostUpdate();
	}

}

void VirtualInstance::DoSnapshot(SnapshotStream& stream, bool bReliableFlag, bool bForced)
{
	size_t max_value = 0;
	uint16_t num_updates = 0;

	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
		it->second->GetSnapshot(max_value, num_updates, stream, bReliableFlag, bForced);

	if (num_updates == 0)
		return;

	printf("num updates: %d (%d) | max value: %d\n", stream.get()->size() - 1, num_updates, max_value);

	NETTIK::INetworkCodes::msg_t code;
	code = NETID_Reserved::RTTI_Object::SNAPSHOT;

	SnapshotStream::Stream& header = stream.header();

	SnapshotHeader generator(header);
	generator.set_code(code);
	generator.set_sequence(m_iSequenceID);
	generator.set_count(num_updates);
	generator.set_max(max_value);
	generator.write();

	generator.copy_from(stream, max_value);
}

void VirtualInstance::DestroyEntityManager(std::string name)
{
	auto it = m_EntManagers.find(name);
	m_PendingDeletes.push_back(it);
}

void VirtualInstance::DestroyEntityManager(IEntityManager* mgr)
{
	DestroyEntityManager(mgr->GetName());
}

VirtualInstance::~VirtualInstance()
{
	// Clear entity managers
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end();)
	{
		delete(it->second);
		it = m_EntManagers.erase(it);
	}
};

VirtualInstance::VirtualInstance(std::string name, IController* controller)
{
	m_ParentController = controller;
	m_sName = name;
}