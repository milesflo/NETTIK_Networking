//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "NetObject.h"
#include "NetVar.h"
#include "CEntities.h"

void NetObject::DestroyNetworkedEntity() const
{
	m_pManager->Remove( m_NetCode );
}

std::string NetObject::GetPeerHost()
{
	if (m_pPeer == nullptr)
		return "nullptr:0";

	char buffer[32] = { 0 };
	enet_address_get_host_ip(&m_pPeer->address, buffer, 32);

	return std::string(buffer) + std::to_string(m_pPeer->address.port);
}

NetObject::~NetObject()
{
	if (m_pManager != nullptr)
	{
		DestroyNetworkedEntity();
	}
}

void NetObject::TakeObjectSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& buffers, bool bReliableFlag, bool bForced)
{
	for (auto it = m_Vars.begin(); it != m_Vars.end(); ++it)
	{
		if (it->second->GetReliable() != bReliableFlag)
			continue;

		size_t max_size = it->second->TakeVariableSnapshot(buffers, bForced);
		if (max_size == 0)
			continue;

		if (max_size > max_value)
			max_value = max_size;


		num_updates++;
	}
}