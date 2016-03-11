#include "VirtualInstance.h"
#include "IController.hpp"
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

void VirtualInstance::DoSnapshot(bool bReliableFlag, bool bForced, ENetPeer* peer)
{
	size_t max_value = 0;
	uint16_t num_updates = 0;
	std::vector<std::vector<unsigned char>> buffers;
	std::vector<unsigned char> header;

	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); ++it)
		it->second->GetSnapshot(max_value, num_updates, buffers, bReliableFlag, bForced);

	if (num_updates == 0)
		return;

	printf("num updates: %d (%d) | max value: %d\n", buffers.size(), num_updates, max_value);

	uint32_t code;
	code = NETID_Reserved::RTTI_Object::SNAPSHOT;

	// | netmsg code |
	for (size_t i = 0; i < sizeof(NETTIK::INetworkCodes::msg_t); i++)
		header.push_back(((unsigned char*)(&code))[i]);

	// | seq id |
	for (size_t i = 0; i < sizeof(uint32_t); i++)
		header.push_back(((unsigned char*)(&m_iSequenceID))[i]);

	// | count |
	for (size_t i = 0; i < sizeof(uint16_t); i++)
		header.push_back(((unsigned char*)(&num_updates))[i]);

	if (peer == nullptr)
	{
		// This is a routine snapshot to broadcast to container peers.
		m_iSequenceID++;
		// Controller -> Broadcast ( header, buffer )
	}
	else
	{
		// This is a specific snapshot to resync a plsyer, don't do any tick logic.
		// Controller -> Send ( peer, header, buffer )
	}


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