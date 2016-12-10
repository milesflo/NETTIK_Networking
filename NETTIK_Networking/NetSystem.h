//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "SnapshotEntList.h"
#include "IThread.h"
#include "NetObject.h"
#include "MessageDispatcher.h"
#include <functional>
#include <unordered_map>
#include <mutex>

class VirtualInstance;

//------------------------------------------------------------------------
// NetSystem: Core network controller class that manages low-level
// communication to evaluate and maintain logical objects. 
//------------------------------------------------------------------------
class NetSystem
{
public:
	//--------------------------
	// Controller typedefs
	//--------------------------
	using CallbackFunction_f = std::function<void(const enet_uint8* data, size_t data_length, ENetPeer* enetPeer)>;
	using EventFunction_f    = std::function<void(ENetEvent& evtFrame)>;

	enum DISCONNECT_REASONS : INetworkCodes::msg_t
	{
		NETTIK_DISCONNECT_SHUTDOWN = 0xFFFF,
		NETTIK_DISCONNECT_KICK     = 0xFFFE,
		NETTIK_DISCONNECT_BAN      = 0xFFFD
	};

	//-----------------------------------------------
	// DEBUG: Enables controller dumping mode.
	// All communication will be piped to console.
	//-----------------------------------------------
	void Dump();

	//-----------------------------------------------
	// Synchronously waits till the shutdown flag
	// is true, then deletes the controller pointer.
	//-----------------------------------------------
	void Destroy();

	//--------------------------------------------------
	// Processes a received entity update frame
	// from a peer and implements changes if permissions
	// are valid (or is most dominant).
	//--------------------------------------------------
	void ReadEntityUpdate(SnapshotEntList& frame, ENetPeer* owner = nullptr);

	//-----------------------------------------
	// Performs a controller specific post 
	// update.
	//-----------------------------------------
	virtual void PostUpdate(float elapsedTime) = 0;

	//-----------------------------------------
	// Performs a controller specific update.
	//-----------------------------------------
	virtual void ControllerUpdate(float elapsedTime) = 0;

	//-----------------------------------------
	// Returns if the controller is a server.
	//-----------------------------------------
	bool IsServer();

	//-----------------------------------------
	// Does a logical update of all network
	// operations.
	//-----------------------------------------
	void Update(float elapsedTime);

	//-----------------------------------------
	// Evaluates snapshot changes and relays
	// changes to connected clients.
	//-----------------------------------------
	void SnapshotUpdate(float elapsedTime);

	//-----------------------------------------
	// Performs a single iteration over the 
	// network stack.
	//-----------------------------------------
	void ProcessNetStack();

	//-----------------------------------------
	// Starts the all network threads.
	//-----------------------------------------
	void Start();

	//-----------------------------------------
	// Stops the network threads and
	// deinitialises all ENET objects.
	//-----------------------------------------
	void Stop();

	//-----------------------------------------
	// Main thread entry for processing the
	// controller's network stack.
	//-----------------------------------------
	void Run(bool& bThreadStatus);

	//-----------------------------------------
	// Broadcasts a data stream to all 
	// ENET peers attached to the host.
	//-----------------------------------------
	void Broadcast(const enet_uint8* data, size_t data_len, uint32_t flags, uint8_t channel);

	//-----------------------------------------
	// Sends a data stream to an ENET peer.
	//-----------------------------------------
	void Send(const enet_uint8* data, size_t data_len, ENetPeer* peer, uint32_t flags, uint8_t channel);

	//-----------------------------------------
	// Broadcasts a SnapshotStream blob to
	// all ENET peers attached to the host.
	//-----------------------------------------
	void BroadcastStream(SnapshotStream& stream, bool reliable);

	//-----------------------------------------
	// Sends a SnapshotStream blob to an ENET
	// peer.
	//-----------------------------------------
	void SendStream(SnapshotStream& stream, bool reliable, ENetPeer* peer = nullptr);

	//-----------------------------------------
	// Sends a data stream to the first ENET
	// peer.
	//-----------------------------------------
	void Send(const enet_uint8* data, size_t data_len, uint32_t flags = ENET_PACKET_FLAG_UNSEQUENCED, uint8_t channel = 0);

	//-----------------------------------------
	// Processes a single data stream, then
	// fires the appropriate function.
	//-----------------------------------------
	void ProcessRecv(const enet_uint8* data, size_t data_length, ENetPeer* peer);

	//-----------------------------------------
	// Allocates an instance to the passed
	// instance name, then returns the new 
	// object.
	//-----------------------------------------
	VirtualInstance* CreateInstance(std::string name);

	//-----------------------------------------
	// Gets a single instance by its assigned
	// instance name.
	//-----------------------------------------
	VirtualInstance* GetInstance(std::string name);

	//-----------------------------------------
	// Gets a list of instance pointers inside
	// this controller.
	//-----------------------------------------
	std::unordered_map<std::string, VirtualInstance*> GetInstances();

	//-----------------------------------------
	// Deletes an instance from the controller
	//-----------------------------------------
	void DeleteInstance( std::string name );
	void DeleteInstance( VirtualInstance* instance );

	//-----------------------------------------
	// DEPRECATED: Gets the static controller 
	// singleton. [[ Use GetSingleton<T>( ) ]]
	//-----------------------------------------
	static __declspec(deprecated("Please use GetSingleton<T>()")) NetSystem* GetPeerSingleton();

	//------------------------------------
	// Gets the static controller singleton 
	// casted to a template class
	//------------------------------------
	template <class T>
	static T* GetSingleton();

	//-----------------------------------------
	// Gets the static controller singleton
	//-----------------------------------------
	static NetSystem* GetSingleton();

	//-----------------------------------------
	// Sets the static controller singleton
	//-----------------------------------------
	static void SetPeerSingleton(NetSystem* peer);

	//-----------------------------------------
	// Deletes the static controller singleton
	//-----------------------------------------
	static void DeletePeerSingleton();

	//------------------------------------
	// Binds a callback to a network code
	//------------------------------------
	void on(INetworkCodes::msg_t code, CallbackFunction_f callback);

	//------------------------------------
	// Binds a callback to an ENET event
	// This call might come from a remote
	// thread.
	//------------------------------------
	void on_enet(ENetEventType evt, EventFunction_f callback);

	//------------------------------------
	// Fires an event
	//------------------------------------
	void FireEvent(ENetEventType evt, ENetEvent& evtFrame);

	//--------------------------------------------------
	// Utility to convert ENetAddress into a printable 
	// format.
	//--------------------------------------------------
	static std::string GetIPAddress(ENetAddress& addr);

	//----------------------------------------------
	// Returns the ENET host used by the controller
	//----------------------------------------------
	ENetHost* GetHost() const;

	//-------------------------------------------
	// Returns the first peer in the controller
	//-------------------------------------------
	ENetPeer* GetFirstPeer();

	//-----------------------------------------
	// Gets the controller network rate in mis
	//-----------------------------------------
	uint32_t GetNetworkRate(void) const;

	//-----------------------------------------
	// Sets the controller network rate in ms
	//-----------------------------------------
	void SetNetworkRate(uint32_t netRate);

	//------------------------------------
	// Returns the running state flag
	//------------------------------------
	bool IsRunning(void) const;

	//------------------------------------
	// Returns the active message queue.
	//------------------------------------
	CMessageDispatcher& GetQueue();

	//------------------------------------
	// Dumps the current network stats
	// to the queue.
	//------------------------------------
	void PrintHostStatistics();

	//---------------------------------------
	// Accessors for the amount of channels
	// the system will use.
	//---------------------------------------
	void SetChannelCount(size_t iChannels);
	size_t GetChannelCount() const;

protected:
	//--------------------------------
	// Singleton controller
	//--------------------------------
	static NetSystem* s_PeerSingleton;
	
	//--------------------------------
	// Internal process threads.
	//--------------------------------
	IThread*    m_pThread        = nullptr;
	IThread*    m_pSyncThread    = nullptr;

	//--------------------------------
	// Status flags
	//--------------------------------
	bool        m_bShuttingDown  = false; // Flagged when the controller is in the process of shutting down
	bool        m_bRunning       = false; // Is the system running
	bool        m_bConnected    = false;  // Is the system connected (if on client)
	bool        m_bReplicating  = false;  // Is the system replicating entities

	//--------------------------------
	// ENET Structures
	//--------------------------------
	ENetAddress m_Address;
	ENetHost*   m_pHost;
	ENetEvent   m_CurrentEvent;
	size_t      m_iChannelCount = 2;

	//-----------------------------------------------------
	// Initialises the ENET address data. 
	//-----------------------------------------------------
	virtual bool InitializeAddress(const char* hostname, uint16_t port) = 0;

	//-----------------------------------------------------
	// Initialises and establishes the implementation
	// host.
	//-----------------------------------------------------
	virtual bool InitializeHost() = 0;

	//-----------------------------------------------------
	// System based details
	//-----------------------------------------------------
	uint32_t    m_iNetworkRate; // How many ticks per second the service will aim to network at.
	bool        m_bServer;      // Flagged true when the system is 
	bool        m_bDumpMode = false;

	NetSystem( uint32_t tickRate );
	virtual ~NetSystem();

	//------------------------------------
	// Callbacks
	//------------------------------------
	std::recursive_mutex m_CallbackMutex; // Guard access to callbacks from remote threads.
	std::unordered_map < INetworkCodes::msg_t, std::vector<CallbackFunction_f> > m_Callbacks;
	std::unordered_map < ENetEventType, std::vector<EventFunction_f > >          m_EventCallbacks;

	// Read/write mutex for the network stack.
	std::mutex m_ENET_RW;

	//------------------------------------
	// Internal components
	//------------------------------------
	std::vector<ENetPeer*> m_PeerList;
	std::unordered_map<std::string, std::unique_ptr<VirtualInstance>> m_Instances;
	CMessageDispatcher m_Dispatcher;
	//------------------------------------
	// Services the ENET host internally
	//------------------------------------
	int ServiceHostSafe(ENetHost *, ENetEvent *, enet_uint32);
};


//---------------------------------------
// Accessors for the amount of channels
// the system will use.
//---------------------------------------
inline void NetSystem::SetChannelCount(size_t iChannels)
{
	m_iChannelCount = iChannels;
}

inline size_t NetSystem::GetChannelCount() const
{
	return m_iChannelCount;
}

//------------------------------------
// Returns the active message queue.
//------------------------------------
inline CMessageDispatcher& NetSystem::GetQueue()
{
	return m_Dispatcher;
}

//--------------------------------------------------
// Utility to convert ENetAddress into a printable 
// format.
//--------------------------------------------------
inline std::string NetSystem::GetIPAddress(ENetAddress& addr)
{
	char buffer[20];

	if (enet_address_get_host_ip(&addr, buffer, 20) < 0)
	{
		return "?.?.?.?";
	}

	return std::string(buffer);
}

//----------------------------------------------
// Returns the ENET host used by the controller
//----------------------------------------------
inline ENetHost* NetSystem::GetHost() const
{
	return m_pHost;
}

//------------------------------------
// Binds a callback to a network code
//------------------------------------
inline void NetSystem::on(INetworkCodes::msg_t code, CallbackFunction_f callback)
{
	std::unique_lock<std::recursive_mutex> guard(m_CallbackMutex);
	m_Callbacks[code].push_back(callback);
}

//------------------------------------
// Binds a callback to an ENET event.
// This call might come from a remote
// thread.
//------------------------------------
inline void NetSystem::on_enet(ENetEventType evt, EventFunction_f callback)
{
	std::unique_lock<std::recursive_mutex> guard(m_CallbackMutex);
	m_EventCallbacks[evt].push_back(callback);
}

//-------------------------------------------
// Returns the first peer in the controller
//-------------------------------------------
inline ENetPeer* NetSystem::GetFirstPeer()
{
	if (m_PeerList.size() == 0)
		return nullptr;

	return m_PeerList.front();
}

//-----------------------------------------
// Gets the controller network rate in mis
//-----------------------------------------
inline uint32_t NetSystem::GetNetworkRate(void) const
{
	return m_iNetworkRate;
}

//-----------------------------------------
// Sets the controller network rate in ms
//-----------------------------------------
inline void NetSystem::SetNetworkRate(uint32_t netRate)
{
	m_iNetworkRate = netRate;
}

//------------------------------------
// Returns the running state flag
//------------------------------------
inline bool NetSystem::IsRunning(void) const
{
	return m_bRunning;
}

//------------------------------------
// Gets the static controller singleton 
// casted to a template class
//------------------------------------
template <class T>
inline T* NetSystem::GetSingleton()
{
	return dynamic_cast< T* >( s_PeerSingleton );
}
inline NetSystem* NetSystem::GetSingleton()
{
	return ( s_PeerSingleton );
}