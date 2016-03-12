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
	
	// | buffer max |
	for (size_t i = 0; i < sizeof(size_t); i++)
		header.push_back(((unsigned char*)(&max_value))[i]);

	// Padd all update frames to the max value.
	for (auto it = buffers.begin(); it != buffers.end(); ++it)
	{
		std::vector<unsigned char>* buffer;
		buffer = &(*it);

		if (buffer->size() < max_value)
		{
			uint32_t padsize;
			padsize = max_value -buffer->size();

			for (uint32_t i = 0; i < padsize; ++i)
				buffer->push_back(0);
		}

		// Copy contents into header.
		for (size_t i = 0; i < buffer->size(); ++i)
			header.push_back(buffer->at(i));
	}

	IController* controller;
	controller = IController::GetPeerSingleton();

	if (controller == nullptr)
		NETTIK_EXCEPTION("Tried snapshotting invalid network controller.");

	uint32_t flags;
	flags = bReliableFlag ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;

	if (peer == nullptr)
	{
		// This is a routine snapshot to broadcast to container peers.
		m_iSequenceID++;
		controller->Broadcast(&header[0], header.size(), flags, 0);
		printf("Broadcast.\n");
	}
	else
	{
		// This is a specific snapshot to resync a plsyer, don't do any tick logic.
		// Controller -> Send ( peer, header, buffer )
		controller->Send(&header[0], header.size(), peer, flags, 0);
		printf("Sending to peer.\n");
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