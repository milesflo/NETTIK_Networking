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
	m_bServer = false;
}

IControllerClient::IControllerClient() : IController(60)
{
	if (!InitializeHost())
		NETTIK_EXCEPTION("Failed establishing host, network protcol error.");

	m_bReplicating = true;
	m_bServer = false;
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
	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
		it->second->DoPostUpdate();

	PostUpdate();
}

void IControllerClient::EntAllocate(SnapshotEntList& frame)
{
	const char* manager_name = frame.get_name();
	const uint32_t netID = frame.get_netid();
	const uint32_t controller = frame.get_controller();
	const char* instance_name = reinterpret_cast<const char*>(frame.get_data());

	VirtualInstance* instance;
	instance = GetInstance(instance_name);

	if (instance == nullptr)
		NETTIK_EXCEPTION("Invalid instance field passed from server.");

	IEntityManager* manager;
	manager = instance->GetManager(manager_name);

	if (manager == nullptr)
		NETTIK_EXCEPTION("Invalid manager field passed from server.");

	if (manager->GetByNetID(netID) == nullptr)
	{
		NetObject* instance = manager->AddLocal(netID, controller);

		if (controller == NET_CONTROLLER_LOCAL)
			m_ControlledObjects[netID] = instance;

	}

}

void IControllerClient::EntDeallocate(SnapshotEntList& frame)
{
	const char* manager_name = frame.get_name();
	const uint32_t netID = frame.get_netid();
	const uint32_t controller = frame.get_controller();
	const char* instance_name = reinterpret_cast<const char*>(frame.get_data());

	VirtualInstance* instance;
	instance = GetInstance(instance_name);

	if (instance == nullptr)
		NETTIK_EXCEPTION("Invalid instance field passed from server.");

	IEntityManager* manager;
	manager = instance->GetManager(manager_name);

	if (manager == nullptr)
		NETTIK_EXCEPTION("Invalid manager field passed from server.");

	NetObject* object;
	object = manager->GetByNetID(netID);
	if (object == nullptr)
		return;

	manager->RemoveLocal(netID);

	if (object->m_Controller == NET_CONTROLLER_LOCAL)
	{
		auto object = m_ControlledObjects.find(netID);

		if (object != m_ControlledObjects.end())
			m_ControlledObjects.erase(object);
	}

}

void IControllerClient::HandleEntOwnership(const enet_uint8* data, size_t data_len, ENetPeer* peer)
{
	SnapshotHeader header;
	header.read(data, data_len);

	enet_uint8* partition;
	partition = (enet_uint8*)(data);
	partition += header.size();

	size_t expected_size = (sizeof(uint32_t) * header.count());
	size_t header_size = data_len - header.size();

	if (expected_size != header_size)
	{
		printf("warning: dropped ownership list, provided size '%d' when expected '%d'\n", header_size, expected_size);
		return;
	}

	for (size_t i = 0; i < header.count(); i++)
	{
		uint32_t netid = static_cast<uint32_t>(*partition);

		printf("I now own %d\n", netid);
		partition += sizeof(uint32_t);
	}
}

void IControllerClient::HandleEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer)
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

		switch (frame.get_frametype())
		{
		case kFRAME_Data:
			ReadEntityUpdate(frame);
			break;
		case kFRAME_Alloc:
			EntAllocate(frame);
			break;
		case kFRAME_Dealloc:
			EntDeallocate(frame);
			break;
		default:
			NETTIK_EXCEPTION("Unknown frame type sent, possible stack corruption.");
		}

		partition += header.max_size();
	}
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

	on(NETID_Reserved::RTTI_Object::OBJECT_FRAME,   std::bind(&IControllerClient::HandleEntSnapshot, this, _1, _2, _3));
	on(NETID_Reserved::RTTI_Object::CLIENT_OWNERSHIP, std::bind(&IControllerClient::HandleEntOwnership, this, _1, _2, _3));
//	on(NETID_Reserved::RTTI_Object::OBJECT_NEW, std::bind(&IControllerClient::HandleEntNew, this, _1, _2, _3));
//	on(NETID_Reserved::RTTI_Object::OBJECT_DEL, std::bind(&IControllerClient::HandleEntDel, this, _1, _2, _3));

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
