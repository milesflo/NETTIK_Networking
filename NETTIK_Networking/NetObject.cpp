//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "CEntities.h"

//--------------------------------------
// Called after the structure has been
// fully prepared for network use.
//--------------------------------------
void NetObject::Initialize()
{
	CMessageDispatcher::Add(kMessageType_Print, "NetObject::Initialize()");
}

//--------------------------------------
// Informs the parent manager to erase
// this object from its list.
//--------------------------------------
void NetObject::DestroyNetworkedEntity() const
{
	m_pManager->FreeEntity( const_cast<NetObject*>(this) );
}

//--------------------------------------
// Flags all variables to be transmitted
// over all associated peers.
//--------------------------------------
void NetObject::InvalidateVars()
{
	for (auto it = m_Vars.begin(); it != m_Vars.end(); ++it)
	{
		(*it)->InvalidateChanges();
	}

	for (auto it = m_MapList.begin(); it != m_MapList.end(); ++it)
	{
		(*it)->SendContents(m_pManager);
	}
}

void NetObject::UpdateListDelta()
{
	for (auto it = m_MapList.begin(); it != m_MapList.end(); ++it)
	{
		(*it)->SendDelta(m_pManager);
		(*it)->Flush();
		(*it)->think();
	}
}

void NetObject::SendLists(ENetPeer* pRemotePeer)
{
	for (auto it = m_MapList.begin(); it != m_MapList.end(); ++it)
	{
		(*it)->SendContents(pRemotePeer);
	}
}

//--------------------------------------
// Calculates the peer's host in a 
// printable format.
//--------------------------------------
std::string NetObject::GetPeerHost()
{
	if (m_pPeer == nullptr)
		return "nullptr:0";

	char buffer[32] = { 0 };
	enet_address_get_host_ip(&m_pPeer->address, buffer, 32);

	return std::string( buffer ) + ":" + std::to_string( m_pPeer->address.port );
}

NetObject::~NetObject()
{
	if (m_pManager != nullptr)
	{
		DestroyNetworkedEntity();
		return;
	}
	CMessageDispatcher::Add(kMessageType_Warn, "Deleted object");

	CMessageDispatcher::Add(kMessageType_Error, "Failed releasing object from network controlled, m_pManager is NULL.");
}

// profile: high cpu usage (reporting 60%)
void NetObject::TakeObjectSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& buffers, bool bReliableFlag, bool bForced)
{
	for (auto it = m_Vars.begin(); it != m_Vars.end(); ++it)
	{
		NetVar* pVar;
		pVar = (*it);
		if (pVar->GetReliable() != bReliableFlag)
			continue;

		size_t max_size = pVar->TakeVariableSnapshot(buffers, bForced);
		if (max_size == 0)
			continue;

		if (max_size > max_value)
			max_value = max_size;

		num_updates++;
	}
}