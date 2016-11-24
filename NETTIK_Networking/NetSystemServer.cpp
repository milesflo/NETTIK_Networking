//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "NetSystemServer.h"
using namespace std::placeholders;

bool NetSystemServer::InitializeHost()
{
	m_pHost = enet_host_create(&m_Address, m_iPeerLimit, m_iChannelCount, 0, 0);

	if (m_pHost == NULL)
		return false;

	return true;
}

bool NetSystemServer::InitializeAddress(const char* hostname, uint16_t port)
{
	if (hostname == nullptr)
	{
		m_Address.host = ENET_HOST_ANY;
	}
	else
	{
		m_Address.host = ENET_HOST_ANY;
	}

	m_Address.port = port;
	return true;
}

NetSystemServer::NetSystemServer(uint32_t rate) : NetSystem(rate)
{
	m_bReplicating = false;
	m_bServer = true;


	on(NETID_Reserved::RTTI_Object::OBJECT_FRAME, std::bind(&NetSystemServer::HandleClientEntSnapshot, this, _1, _2, _3));

}

bool NetSystemServer::Listen(std::uint16_t port, size_t peerLimit, const char* psHostname)
{
	m_iPeerLimit = peerLimit;

	if (!InitializeAddress(psHostname, port) || !InitializeHost())
		return false;

	m_bConnected = true;
	return true;
}


void NetSystemServer::HandleClientEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer)
{
	SnapshotHeader header;
	header.read(data, data_len);

	size_t expected_size = header.count() * header.max_size();
	size_t header_size = data_len - header.size();

	if (header_size != expected_size)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "Dropped snapshot, provided size '%d' when expected '%d'", header_size, expected_size);
		return;
	}


	enet_uint8* partition;
	partition = (enet_uint8*)(data);
	partition += header.size();

	// partition now at the first frame.
	for (size_t i = 0; i < header.count(); i++)
	{
		SnapshotEntList frame;
		frame.read_data(partition, header.max_size());

		// Permissions are handled by the second parameter
		if (frame.get_frametype() == kFRAME_Data)
		{
			ReadEntityUpdate(frame, peer);
		}
		partition += header.max_size();
	}
}


void NetSystemServer::ControllerUpdate(float elapsedTime)
{
	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
		it->second->DoPostUpdate(elapsedTime);

	PostUpdate(elapsedTime);
}

NetSystemServer::~NetSystemServer()
{

}