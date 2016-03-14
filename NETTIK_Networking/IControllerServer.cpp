#include "IControllerServer.hpp"
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

}

IControllerServer::IControllerServer() : IController(0)
{
	m_bReplicating = false;
}

bool IControllerServer::Listen(uint16_t port, size_t peerLimit)
{
	m_iPeerLimit = peerLimit;

	if (!InitializeAddress(0, port) || !InitializeHost())
		return false;

	m_bConnected = true;
	return true;
}

void IControllerServer::ControllerUpdate()
{

	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		VirtualInstance* instance;
		instance = it->second.get();

		SnapshotStream reliableStream;
		SnapshotStream unreliableStream;

		instance->DoSnapshot(reliableStream, true, false);
		instance->DoSnapshot(unreliableStream, false, false);

		if (reliableStream.modified())
			BroadcastStream(reliableStream, true);

		if (unreliableStream.modified())
			BroadcastStream(unreliableStream, false);


	}

	PostUpdate();

	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
		it->second->DoPostUpdate();

}
IControllerServer::~IControllerServer()
{

}