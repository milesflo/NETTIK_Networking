#include "IControllerServer.hpp"
using namespace std::placeholders;
using namespace NETTIK;

bool IControllerServer::InitializeHost()
{
	m_pHost = enet_host_create(&m_Address, m_iPeerLimit, 0, 0, 0);

	if (m_pHost == NULL)
		return false;

	return true;
}


bool IControllerServer::InitializeAddress(const char* hostname, uint16_t port)
{
	m_Address.host = ENET_HOST_ANY;
	m_Address.port = port;
	return true;
}

IControllerServer::IControllerServer(uint32_t rate) : IController(rate)
{
	m_bReplicating = false;
	m_bServer = true;


	on(NETID_Reserved::RTTI_Object::OBJECT_FRAME, std::bind(&IControllerServer::HandleClientEntSnapshot, this, _1, _2, _3));

}

bool IControllerServer::Listen(uint16_t port, size_t peerLimit)
{
	m_iPeerLimit = peerLimit;

	if (!InitializeAddress(0, port) || !InitializeHost())
		return false;

	m_bConnected = true;
	return true;
}


void IControllerServer::HandleClientEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer)
{
	SnapshotHeader header;
	header.read(data, data_len);

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
		frame.read_data(partition, header.max_size());

		// Permissions are handled by the second parameter
		if (frame.get_frametype() == kFRAME_Data)
		{
			ReadEntityUpdate(frame, peer);
		}
		partition += header.max_size();
	}
}


void IControllerServer::ControllerUpdate(float elapsedTime)
{
	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
		it->second->DoPostUpdate(elapsedTime);

	PostUpdate(elapsedTime);
}

IControllerServer::~IControllerServer()
{

}