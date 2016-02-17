#include "IController.hpp"
#include "IDebug.hpp"
using namespace NETTIK;

//! Global singleton for the ENET peer.
static IController* s_PeerSingleton = nullptr;

//! Issue message for when an unhandled packet is processed.
const char* s_issueUnhandledPacket = "Unhandled packet, code: %d, packet length: %d\n";

IController* IController::GetPeerSingleton()
{
	return s_PeerSingleton;
}

void IController::DeletePeerSingleton()
{

	if (s_PeerSingleton == nullptr)
		NETTIK_EXCEPTION("Tried deleting invalid peer.");

	s_PeerSingleton = nullptr;
}

void IController::SetPeerSingleton(IController* peer)
{
	// Deleting should call DeletePeerSingleton.
	if (s_PeerSingleton != nullptr)
		delete(s_PeerSingleton);

	s_PeerSingleton = peer;
}

//
// IController member functions:
//
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
	// Flag the controller as stopped.
	Stop();

	// We don't need ENET anymore.
	enet_deinitialize();

	// Delete the global singleton (this!)
	// (doesn't actually delete the singleton,
	// just dereferences it)
	DeletePeerSingleton();
}

void IController::Start()
{
	m_bRunning = true;

	if (m_pThread)
		m_pThread->Start();
}

void IController::AddObject(std::unique_ptr<IAtomObject> obj)
{
	// When adding objects, make sure there aren't any loops ongoing.
	std::lock_guard<std::mutex> _(m_ObjectListMutex);

	obj->SetAllocated(true);
	m_AtomObjects.push_back(std::move(obj));
}

void IController::RemoveObject(IAtomObject* obj)
{
	// When removing objects, make sure there aren't any loops ongoing too.
	std::lock_guard<std::mutex> _(m_ObjectListMutex);

	for (auto it = m_AtomObjects.begin(); it != m_AtomObjects.end();)
	{
		if (it->get() == obj) {
			(*it)->SetAllocated(false);
			m_AtomObjects.erase(it);
			break;
		}
	}
}


void IController::Update()
{
	// Same with updating, don't loop if another thread has modified the
	// object list.
	std::lock_guard<std::mutex> _(m_ObjectListMutex);

	for (auto it = m_AtomObjects.begin(); it != m_AtomObjects.end(); ++it)
	{
		(*it)->Update(m_bReplicating);
	}

	PostUpdate();
}

void IController::Stop()
{
	m_bRunning = false;
	m_bConnected = false;

	if (m_pThread != nullptr)
	{
		delete(m_pThread);
		m_pThread = nullptr;
	}

	if (m_pHost != nullptr)
	{
		enet_host_destroy(m_pHost);
		m_pHost = nullptr;
	}

}

void IController::Run()
{
	while (m_bRunning)
	{
		ProcessNetStack();
	}
}

void IController::Send(std::string& data, ENetPeer* peer, uint32_t flags, uint8_t channel)
{
	ENetPacket* packet;
	packet = enet_packet_create(data.c_str(), data.size() + 1, flags);

	// Packet pointer gets automatically
	// deleted, future fyi: not a memory leak!
	enet_peer_send(peer, channel, packet);
}

void IController::Send(std::string& data, uint32_t flags, uint8_t channel)
{
	Send(data, GetFirstPeer(), flags, channel);
}

void IController::ProcessRecv(std::string& data, ENetPeer* peer)
{
	const char* stream = data.c_str();

	// If data is too small for the message ID type, then
	// don't process the data. This could cause a memory violation by
	// reading too far over the `stream` pointer.
	if (data.size() < sizeof(INetworkCodes::msg_t))
		NETTIK_EXCEPTION("Cannot parse data that has less than the code data type size (out of bounds prevention)");

	INetworkCodes::msg_t code;
	code = (INetworkCodes::msg_t)(*stream);

	// todo: lookup code in the unordered_map and execute function with
	// parsed packet.

	auto it = m_Callbacks.find(code);
	if (it != m_Callbacks.end())
	{
		it->second(data, peer);
		return;
	}

#ifdef _DEBUG
	printf(s_issueUnhandledPacket, code, data.size());
#else
	NETTIK_EXCEPTION(s_issueUnhandledPacket, code, data.size());
#endif
}

void IController::FireEvent(ENetEventType evt, ENetEvent& evtFrame)
{
	if (m_EventCallbacks.find(evt) != m_EventCallbacks.end())
		m_EventCallbacks[evt](m_CurrentEvent);
}

void IController::ProcessNetStack()
{
	if (m_pHost == nullptr)
		NETTIK_EXCEPTION("Tried to process network stack on NULL host pointer.");

	while (enet_host_service(m_pHost, &m_CurrentEvent, m_iNetworkRate) > 0)
	{
		ENetEventType type = m_CurrentEvent.type;

		//! Inform all listeners.
		FireEvent(type, m_CurrentEvent);

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
			const char* data;
			data = (const char*)m_CurrentEvent.packet->data;

			// Send for processing.
			ProcessRecv(std::string(data), m_CurrentEvent.peer);

			// Erase all packet information.
			enet_packet_destroy(m_CurrentEvent.packet);

			break;
		}
		}
	}
}