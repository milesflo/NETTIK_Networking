#include "IController.hpp"
#include "IDebug.hpp"
using NETTIK::IController;

//! Global singleton for the ENET peer.
static IController* g_PeerSingleton = nullptr;

IController* IController::GetPeerSingleton()
{
	return g_PeerSingleton;
}

void IController::DeletePeerSingleton()
{

	if (g_PeerSingleton == nullptr)
		NETTIK_EXCEPTION("Tried deleting invalid peer.");

	g_PeerSingleton = nullptr;
}

void IController::SetPeerSingleton(IController* peer)
{
	// Deleting should call DeletePeerSingleton.
	if (g_PeerSingleton != nullptr)
		delete(g_PeerSingleton);

	g_PeerSingleton = peer;
}

// IController member functions:

IController::IController(uint32_t tickRate) : m_iNetworkRate(tickRate)
{
	SetPeerSingleton(this);

	if (enet_initialize() != 0)
		NETTIK_EXCEPTION("ENET initialisation failed.");


	m_pThread = new IThread([](void* pData) {

		IController* self;
		self = static_cast<IController*>(pData);

		self->Run();

	}, this);
}

IController::~IController()
{
	m_bRunning = false;

	if (m_pThread != nullptr)
		delete(m_pThread);

	DeletePeerSingleton();

	if (m_pHost != NULL)
		enet_host_destroy(m_pHost);

	enet_deinitialize();
	printf("IController destruct.\n");
}

void IController::Start()
{
	m_bRunning = true;

	if (m_pThread)
		m_pThread->Start();
}

void IController::Run()
{
	while (m_bRunning)
	{
		ProcessNetStack();
	}
}

void IController::Send(std::string& data, ENetPeer* peer, uint32_t flags)
{
	ENetPacket* packet;
	packet = enet_packet_create(data.c_str(), data.size() + 1, flags);
}

void IController::Send(std::string& data, uint32_t flags)
{
	Send(data, GetFirstPeer(), flags);
}

void IController::ProcessRecv(std::string& data)
{
	const char* stream = data.c_str();

	// If data is too small for the message ID type, then
	// don't process the data. This could cause a memory violation by
	// reading too far over the `stream` pointer.
	if (data.size() < sizeof(INetworkCodes::msg_t))
		NETTIK_EXCEPTION("Cannot parse data that has less than the code data type size (out of bounds prevention)");

	INetworkCodes::msg_t code;
	code = (INetworkCodes::msg_t)(*stream);

	printf("debug: (id = %d, content: %s)\n", code, stream);
	// todo: lookup code in the unordered_map and execute function with
	// parsed packet.
}

void IController::ProcessNetStack()
{
	if (m_pHost == nullptr)
		NETTIK_EXCEPTION("Tried to process network stack on NULL host pointer.");

	while (enet_host_service(m_pHost, &m_CurrentEvent, m_iNetworkRate) > 0)
	{
		ENetEventType type = m_CurrentEvent.type;

		//! Inform all listeners.
		if (m_EventCallbacks.find(type) != m_EventCallbacks.end())
			m_EventCallbacks[type](m_CurrentEvent);

		//! Process event...
		switch (type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			m_PeerList.push_back(m_CurrentEvent.peer);
			m_bConnected = true;
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			//! Don't use this variable in a server container.
			// I don't like this, but it kinda does the job.
			// Juts ignore bConnected if working with the server.
			auto it = std::find(m_PeerList.begin(), m_PeerList.end(), m_CurrentEvent.peer);

			if (it != m_PeerList.end())
				m_PeerList.erase(it);

			m_bConnected = false;

			break;
		}

		case ENET_EVENT_TYPE_RECEIVE:
		{
			printf("incoming packet..\n");

			const char* data;
			data = (const char*)m_CurrentEvent.packet->data;

			// Send for processing.
			ProcessRecv(std::string(data));

			// Erase all packet information.
			enet_packet_destroy(m_CurrentEvent.packet);

			break;
		}
		}
	}
}