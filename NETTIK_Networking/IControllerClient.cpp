#include "IControllerClient.hpp"
#include "SnapshotHeader.h"
#include "SnapshotEntList.h"
using namespace std::placeholders;
using namespace NETTIK;

IControllerClient::~IControllerClient()
{

}

IControllerClient::IControllerClient(uint32_t rate) : IController(rate)
{
	if (!InitializeHost())
		NETTIK_EXCEPTION("Failed establishing host, network protcol error.");

	m_bReplicating = true;
}

IControllerClient::IControllerClient() : IController(0)
{
	if (!InitializeHost())
		NETTIK_EXCEPTION("Failed establishing host, network protcol error.");

	m_bReplicating = true;
}


bool IControllerClient::InitializeHost()
{
	m_pHost = enet_host_create(NULL, 1, 0, 0, 0);

	if (m_pHost == NULL)
		return false;

	return true;
}

bool IControllerClient::InitializeAddress(const char* hostname, uint16_t port)
{
	enet_address_set_host(&m_Address, hostname);
	m_Address.port = port;
	return true;
}

void IControllerClient::ControllerUpdate()
{
	PostUpdate();
}

void IControllerClient::HandleEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer)
{
	SnapshotHeader header;
	header.read(data, data_len);
	header.display();

	size_t expected_size = header.count() * header.max_size();
	size_t header_size = data_len - header.size();

	if (header_size != expected_size)
	{
		printf("warning: dropped snapshot, provided size '%d' when expected '%d'\n", header_size, expected_size);
		return;
	}


	enet_uint8* partition;
	partition = (enet_uint8*)(data);
	partition += header.size();

	// partition now at the first frame.
	for (size_t i = 0; i < header.count(); i++)
	{
		SnapshotEntList frame;
		frame.read(partition, header.max_size());

		partition += header.max_size();
	}
}

void IControllerClient::HandleEntNew(const enet_uint8* data, size_t data_len, ENetPeer* peer)
{

}

void IControllerClient::HandleEntDel(const enet_uint8* data, size_t data_len, ENetPeer* peer)
{

}

bool IControllerClient::Connect(const char* hostname, uint16_t port)
{
	if (!InitializeAddress(hostname, port))
		return false;

	ENetPeer* peer = enet_host_connect(m_pHost, &m_Address, 0, 0);

	if (peer == nullptr)
		NETTIK_EXCEPTION("Client peer failed.");

	on_enet(ENET_EVENT_TYPE_DISCONNECT, [this](ENetEvent& evtFrame) {
		this->Stop();
	});

	on(NETID_Reserved::RTTI_Object::SNAPSHOT,   std::bind(&IControllerClient::HandleEntSnapshot, this, _1, _2, _3));
	on(NETID_Reserved::RTTI_Object::OBJECT_NEW, std::bind(&IControllerClient::HandleEntNew, this, _1, _2, _3));
	on(NETID_Reserved::RTTI_Object::OBJECT_DEL, std::bind(&IControllerClient::HandleEntDel, this, _1, _2, _3));

	if (enet_host_service(m_pHost, &m_CurrentEvent, 5000) > 0 &&
		m_CurrentEvent.type == ENET_EVENT_TYPE_CONNECT)
	{
		m_PeerList.push_back(peer);

		// Inform listeners because this service call
		// was done outside of the main procedure.
		FireEvent(ENET_EVENT_TYPE_CONNECT, m_CurrentEvent);
		m_bConnected = true;
		return true;
	}
	else
	{
		enet_peer_reset(peer);
		return false;
	}

}
