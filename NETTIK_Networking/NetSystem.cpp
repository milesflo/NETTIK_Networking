//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "CNetVarList.h"
#include "FrameTimer.h"

//! Global singleton for the ENET peer.
NetSystem* NetSystem::s_PeerSingleton = nullptr;

//------------------------------------
// Dumps the current network stats
// to the queue.
//------------------------------------
void NetSystem::PrintHostStatistics()
{
	if (!m_pHost)
	{
		return;
	}

	m_Dispatcher.Add(kMessageType_Warn, "MTU: " + std::to_string(m_pHost->mtu));
	m_Dispatcher.Add(kMessageType_Warn, "TSP: " + std::to_string(m_pHost->totalSentPackets));
	m_Dispatcher.Add(kMessageType_Warn, "TRP: " + std::to_string(m_pHost->totalReceivedPackets));
}

//--------------------------------------------------
// Processes a received entity update frame
// from a peer and implements changes if permissions
// are valid (or is most dominant).
//--------------------------------------------------
void NetSystem::ReadEntityUpdate(SnapshotEntList& frame, ENetPeer* owner)
{

	std::shared_ptr<NetObject> target = nullptr;
	uint32_t    queryID = frame.get_netid();

	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		target = it->second->FindObject(queryID);
		if (target != nullptr)
		{
			break;
		}
	}

	if (target == nullptr)
		return;

	// If owner is set, check the permissions for updating this entity.
	// This will happen when clients resync entities that aren't theirs, but
	// this could also be called by a malicious peer.
	if (owner != nullptr)
	{
		if (target->m_pPeer != owner)
		{
			return;
		}
	}

	NetObject::VariableList_t& vars = target->GetVariables();
	NetVar* pVar = nullptr;

	for (auto it = vars.begin(); it != vars.end(); ++it)
	{
		if (strcmp((*it)->GetName(), frame.get_name()) == 0)
		{
			pVar = (*it);
			break;
		}
	}

	if (pVar == nullptr)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "Tried to update null varname '%s'  with ID: %d", frame.get_name(), queryID);
		return;
	}

	if (target->IsNetworkLocal() && frame.get_forced() == false)
	{
	}
	else
	{
		pVar->Set(const_cast<unsigned char*>(frame.get_data()), 0, owner);
	}
}

//-----------------------------------------
// Returns if the controller is a server.
//-----------------------------------------
bool NetSystem::IsServer()
{
	return m_bServer;
}

//-----------------------------------------------
// Synchronously waits till the shutdown flag
// is true, then deletes the controller pointer.
//-----------------------------------------------
void NetSystem::Destroy()
{
	Stop();

	while (m_bShuttingDown)
	{
		std::this_thread::yield();
	}

}

//-----------------------------------------
// Deletes the static controller singleton
//-----------------------------------------
void NetSystem::DeletePeerSingleton()
{

	if (s_PeerSingleton == nullptr)
		NETTIK_EXCEPTION("Tried deleting invalid peer.");

	s_PeerSingleton = nullptr;
}

//-----------------------------------------
// Sets the static controller singleton
//-----------------------------------------
void NetSystem::SetPeerSingleton(NetSystem* peer)
{
	// Deleting should call DeletePeerSingleton.
	if (s_PeerSingleton != nullptr)
		delete(s_PeerSingleton);

	s_PeerSingleton = peer;
}

//
// IController member functions:
//
NetSystem::~NetSystem()
{
	// We don't need ENET anymore.
	enet_deinitialize();

	// Delete the global singleton (this!)
	// (doesn't actually delete the singleton,
	// just dereferences it)
	DeletePeerSingleton();

	std::unique_lock<std::recursive_mutex> guard(m_CallbackMutex);
	m_Callbacks.clear();
	m_EventCallbacks.clear();
	m_Instances.clear();
}

NetSystem::NetSystem(uint32_t tickRate) : m_iNetworkRate(tickRate)
{
	SetPeerSingleton(this);

	if (enet_initialize() != 0)
		NETTIK_EXCEPTION("ENET initialisation failed.");


	// Processes the network stack.
	m_pThread = new IThread([](void* pData, bool& bThreadStatus)
	{
		NetSystem* self = static_cast<NetSystem*>(pData);

		self->Run(bThreadStatus);
	}, this);

	// Process entity synch.
	m_pSyncThread = new IThread([](void* pData, bool& bThreadStatus)
	{
		NetSystem* self = static_cast<NetSystem*>(pData);

		// Track lap time for precise time stepping.
		CFrameTimer timer;
		timer.Start();

		while (self->IsRunning() && bThreadStatus)
		{
			// Sync the thread to the current network rate.
			SynchronousTimer syncTimer(self->GetNetworkRate());

			// Get the current lap time the thread executed on.
			self->Update(timer.GetLapTime());
		}

	}, this);

	// The controller needs to handle dynamic list updates
	// transmitted over the network.
	NetVarListBase::bind_controller( this );
}

//-----------------------------------------
// Allocates an instance to the passed
// instance name, then returns the new 
// object.
//-----------------------------------------
VirtualInstance* NetSystem::CreateInstance(std::string name)
{
	auto vinstance = std::make_unique<VirtualInstance>(name, this);
	m_Instances[name] = std::move(vinstance);

	return m_Instances[name].get();
}

//-----------------------------------------
// Deletes an instance from the controller
//-----------------------------------------
void NetSystem::DeleteInstance(std::string name)
{
	auto it = m_Instances.find(name);
	if (it != m_Instances.end())
		m_Instances.erase(it);
}
void NetSystem::DeleteInstance(VirtualInstance* instance)
{
	DeleteInstance(instance->GetName());
}

//-----------------------------------------
// Gets a single instance by its assigned
// instance name.
//-----------------------------------------
VirtualInstance* NetSystem::GetInstance(std::string name)
{
	auto it = m_Instances.find(name);
	if (it != m_Instances.end())
		return (*it).second.get();
	else
		return nullptr;
}

//-----------------------------------------
// Gets a list of instance pointers inside
// this controller.
//-----------------------------------------
std::unordered_map<std::string, VirtualInstance*> NetSystem::GetInstances()
{
	std::unordered_map<std::string, VirtualInstance*> instances_clone;
	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
		instances_clone[it->first] = it->second.get();
	return instances_clone;
}

//-----------------------------------------
// Starts the all network threads.
//-----------------------------------------
void NetSystem::Start()
{
	m_bRunning = true;

	if (m_pThread)
		m_pThread->Start();

	if (m_pSyncThread)
		m_pSyncThread->Start();
}

//-----------------------------------------
// Evaluates snapshot changes and relays
// changes to connected clients.
//-----------------------------------------
void NetSystem::SnapshotUpdate(float elapsedTime)
{
//	if (m_pHost->connectedPeers == 0)
//		return;

	SnapshotStream reliableStream;
	SnapshotStream unreliableStream;

	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		VirtualInstance* instance;
		instance = it->second.get();


		instance->DoSnapshot(reliableStream, true, false);
		instance->DoSnapshot(unreliableStream, false, false);

		if (reliableStream.modified())
		{
			if (IsServer())
				BroadcastStream(reliableStream, true);
			else
				SendStream(reliableStream, true);
		}

		if (unreliableStream.modified())
		{
			if (IsServer())
				BroadcastStream(unreliableStream, true);
			else
				SendStream(unreliableStream, true);
		}
	}

	for (auto it = m_Instances.begin(); it != m_Instances.end(); ++it)
		it->second->DoPostUpdate(elapsedTime);

	PostUpdate(elapsedTime);

	reliableStream.clear();
	unreliableStream.clear();
}

//-----------------------------------------
// Does a logical update of all network
// operations.
//-----------------------------------------
void NetSystem::Update(float elapsedTime)
{
	if (!m_bRunning)
		return;

	SnapshotUpdate(elapsedTime);
	ControllerUpdate(elapsedTime);
}

//-----------------------------------------
// Stops the network threads and
// deinitialises all ENET objects.
//-----------------------------------------
void NetSystem::Stop()
{
	if (!m_bRunning)
		return;

	m_bShuttingDown = true;

	if (m_pThread != nullptr)
	{
		delete(m_pThread);
		m_pThread = nullptr;
	}

	if (m_pSyncThread != nullptr)
	{
		delete(m_pSyncThread);
		m_pSyncThread = nullptr;
	}

	m_bRunning = false;
	m_bConnected = false;

//	if (!m_bReplicating)
//	{
		// Close all connections.
		for (auto it = m_PeerList.begin(); it != m_PeerList.end();)
		{
			enet_peer_disconnect_now(*it, DISCONNECT_REASONS::NETTIK_DISCONNECT_SHUTDOWN);
			it = m_PeerList.erase(it);
		}
//	}

	if (m_pHost != nullptr)
	{
		enet_host_destroy(m_pHost);
		m_pHost = nullptr;
	}

	m_bShuttingDown = false;
}

//-----------------------------------------
// Main thread entry for processing the
// controller's network stack.
//-----------------------------------------
void NetSystem::Run(bool& bThreadStatus)
{
	while (bThreadStatus && m_bRunning)
	{
		ProcessNetStack();
	}
}

//-----------------------------------------
// Broadcasts a SnapshotStream blob to
// all ENET peers attached to the host.
//-----------------------------------------
void NetSystem::BroadcastStream(SnapshotStream& stream, bool reliable)
{
	uint32_t flags;
	flags = reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;

	Broadcast(&stream.result()[0], stream.result().size(), flags, 0);
}

//-----------------------------------------
// Sends a SnapshotStream blob to an ENET
// peer.
//-----------------------------------------
void NetSystem::SendStream(SnapshotStream& stream, bool reliable, ENetPeer* peer)
{
	uint32_t flags;
	flags = reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;

	if (peer == nullptr)
		peer = GetFirstPeer();

	// This is a specific snapshot to resync a plsyer, don't do any tick logic.
	// Controller -> Send ( peer, header, buffer )
	Send(&stream.result()[0], stream.result().size(), peer, flags, reliable ? 1 : 0);
}

//-----------------------------------------
// Sends a data stream to an ENET peer.
//-----------------------------------------
void NetSystem::Send(const enet_uint8* data, size_t data_len, ENetPeer* peer, uint32_t flags, uint8_t channel)
{
	if (peer == nullptr)
		return;

	if (!m_bRunning)
		return;

	ENetPacket* packet;
	packet = enet_packet_create(data, data_len, flags);

	std::unique_lock<std::mutex> _(m_ENET_RW);

	// Packet pointer gets automatically deleted, future fyi: not a memory leak!
	enet_peer_send(peer, channel, packet);
}

//-----------------------------------------
// Broadcasts a data stream to all 
// ENET peers attached to the host.
//-----------------------------------------
void NetSystem::Broadcast(const enet_uint8* data, size_t data_len, uint32_t flags, uint8_t channel)
{
	if (!m_bRunning)
		return;
	
	ENetPacket* packet;
	packet = enet_packet_create(data, data_len, flags);

	std::unique_lock<std::mutex> _(m_ENET_RW);

	// Packet pointer gets automatically deleted, future fyi: not a memory leak!
	enet_host_broadcast(m_pHost, channel, packet);
}

//-----------------------------------------
// Sends a data stream to the first ENET
// peer.
//-----------------------------------------
void NetSystem::Send(const enet_uint8* data, size_t data_len, uint32_t flags, uint8_t channel)
{
	Send(data, data_len, GetFirstPeer(), flags, channel);
}

//-----------------------------------------
// Processes a single data stream, then
// fires the appropriate function.
//-----------------------------------------
void NetSystem::ProcessRecv(const enet_uint8* data, size_t data_length, ENetPeer* peer)
{
	if (!m_bRunning)
		return;

	// If data is too small for the message ID type, then
	// don't process the data. This could cause a memory violation by
	// reading too far over the `stream` pointer.
	if (data_length < sizeof(INetworkCodes::msg_t))
		NETTIK_EXCEPTION("Cannot parse data that has less than the code data type size (out of bounds prevention)");

	std::unique_lock<std::recursive_mutex> guard(m_CallbackMutex);
	INetworkCodes::msg_t code = *(INetworkCodes::msg_t*)(data);

	auto callbacks = m_Callbacks.find(code);
	if (callbacks != m_Callbacks.end())
	{
		for (auto it = callbacks->second.begin(); it != callbacks->second.end(); ++it)
		{
			(*it)(data, data_length, peer);
		}
		return;
	}

#ifdef _DEBUG
	CMessageDispatcher::Add(kMessageType_Warn, "Unhandled packet, code: %u, packet length: %d", code, data_length);
#endif
}

//------------------------------------
// Fires an event
//------------------------------------
void NetSystem::FireEvent(ENetEventType evt, ENetEvent& evtFrame)
{
	if (!m_bRunning)
		return;

	std::unique_lock<std::recursive_mutex> guard(m_CallbackMutex);
	auto evts = m_EventCallbacks.find(evt);

	if (evts != m_EventCallbacks.end())
	{		
		for (auto it = evts->second.begin(); it != evts->second.end(); ++it)
		{
			(*it)(evtFrame);
		}
	}
}

//-----------------------------------------------
// DEBUG: Enables controller dumping mode.
// All communication will be piped to console.
//-----------------------------------------------
void NetSystem::Dump()
{
	CMessageDispatcher::Add(kMessageType_Warn, "NetSystem is now in dump mode.");
	m_bDumpMode = true;
}

//------------------------------------
// Services the ENET host internally
//------------------------------------
int NetSystem::ServiceHostSafe(ENetHost* pHost, ENetEvent* pEvent, enet_uint32 flag)
{
	std::unique_lock<std::mutex> _(m_ENET_RW);
	return enet_host_service(pHost, pEvent, 0);
}

//-----------------------------------------
// Performs a single iteration over the 
// network stack.
//-----------------------------------------
void NetSystem::ProcessNetStack()
{
	SynchronousTimer timer( m_iNetworkRate );

	if (!m_bRunning)
		return;

	// Check m_bRunning again on each loop.

	while (m_pHost != nullptr && m_bRunning && ServiceHostSafe(m_pHost, &m_CurrentEvent, 0) > 0)
	{
		ENetEventType type = m_CurrentEvent.type;

		// Inform all listeners.
		FireEvent(type, m_CurrentEvent);

		// Process event...
		switch (type)
		{

		case ENET_EVENT_TYPE_CONNECT:
		{
			m_PeerList.push_back(m_CurrentEvent.peer);

			// Send current instance states to new player.
			for (std::pair<const std::string, std::unique_ptr<VirtualInstance>>& pInstancePair : m_Instances)
			{
				std::unique_ptr<VirtualInstance>& pInstance = pInstancePair.second;

				
			}

			m_bConnected = true;
		}
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			// Don't use this variable in a server container.
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
			// Send for processing.
			ProcessRecv(m_CurrentEvent.packet->data, m_CurrentEvent.packet->dataLength, m_CurrentEvent.peer);

			// Erase all packet information.
			enet_packet_destroy(m_CurrentEvent.packet);

			break;
		}
		}
	}

}