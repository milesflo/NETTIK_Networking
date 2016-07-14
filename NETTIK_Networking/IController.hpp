//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <enet\enet.h>
#include <google\protobuf\message.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

#include "INetworkCodes.hpp"
#include "IDebug.h"
#include "IThread.h"
#include "INetworkPacketFactory.hpp"
#include "VirtualInstance.h"

namespace NETTIK
{

	//------------------------------------------------------------------------
	// Core network controller class that manages low-level communication
	// to evaluate and maintain logical objects. 
	//------------------------------------------------------------------------
	class IController
	{

	public:
		//--------------------------
		// Controller typedefs
		//--------------------------
		using CallbackFunction_f =
			std::function<void(const enet_uint8* data, size_t data_length, ENetPeer* enetPeer)>;

		using EventFunction_f =
			std::function<void(ENetEvent& evtFrame)>;

		enum DISCONNECT_REASONS : NETTIK::INetworkCodes::msg_t
		{
			NETTIK_DISCONNECT_SHUTDOWN = 0xFFFF,
			NETTIK_DISCONNECT_KICK     = 0xFFFE,
			NETTIK_DISCONNECT_BAN      = 0xFFFD
		};

		std::vector<ENetPeer*> m_PeerList;
		std::unordered_map<std::string, VirtualInstance_ptr> m_Instances;

	private:

		//----------------------
		// Global controller
		//----------------------
		static IController* s_PeerSingleton;
		
		//--------------------
		// Multithreading
		//--------------------
		IThread*    m_pThread        = nullptr;
		IThread*    m_pSyncThread    = nullptr;

		//--------------------
		// Semaphores
		//--------------------
		bool        m_bShuttingDown  = false;
		bool        m_bRunning       = false;

	protected:

		ENetAddress m_Address;
		ENetHost*   m_pHost;

		bool        m_bConnected    = false;
		bool        m_bReplicating  = false;

		//! Initialises the ENET address data, this
		// differs per implementation.
		virtual bool InitializeAddress(const char* hostname, uint16_t port) = 0;

		//! Initialises and establishes the implementation
		// host.
		virtual bool InitializeHost() = 0;

		ENetEvent   m_CurrentEvent;

		uint32_t    m_iNetworkRate;
		uint32_t    m_iFPS;

		bool        m_bServer;
		bool        m_bDumpMode = false;


		IController( uint32_t tickRate );
		virtual ~IController();

	public:

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

	protected:

		std::unordered_map < INetworkCodes::msg_t, std::vector<CallbackFunction_f> > m_Callbacks;
		std::unordered_map < ENetEventType, std::vector<EventFunction_f > >          m_EventCallbacks;

		std::mutex m_ENET_RW;

	public:

		//-----------------------------------------
		// DEPRECATED: Gets the static controller 
		// singleton. [[ Use GetSingleton<T>( ) ]]
		//-----------------------------------------
		static __declspec(deprecated("Please use GetSingleton<T>()")) IController* GetPeerSingleton();

		//------------------------------------
		// Gets the static controller singleton 
		// casted to a template class
		//------------------------------------
		template <class T>
		static T* GetSingleton();

		//-----------------------------------------
		// Gets the static controller singleton
		//-----------------------------------------
		static IController* GetSingleton();

		//-----------------------------------------
		// Sets the static controller singleton
		//-----------------------------------------
		static void SetPeerSingleton(IController* peer);

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
		static std::string IController::GetIPAddress(ENetAddress& addr);

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

	private:
		//------------------------------------
		// Services the ENET host internally
		//------------------------------------
		int ServiceHostSafe(ENetHost *, ENetEvent *, enet_uint32);

	};

}

inline std::string NETTIK::IController::GetIPAddress(ENetAddress& addr)
{
	char buffer[20];

	if (enet_address_get_host_ip(&addr, buffer, 20) < 0)
	{
		return "?.?.?.?";
	}

	return std::string(buffer);
}

inline ENetHost* NETTIK::IController::GetHost() const
{
	return m_pHost;
}

inline void NETTIK::IController::on(INetworkCodes::msg_t code, CallbackFunction_f callback)
{
	m_Callbacks[code].push_back(callback);
}

inline void NETTIK::IController::on_enet(ENetEventType evt, EventFunction_f callback)
{
	m_EventCallbacks[evt].push_back(callback);
}

inline ENetPeer* NETTIK::IController::GetFirstPeer()
{
	if (m_PeerList.size() == 0)
		return nullptr;

	return m_PeerList.front();
}

inline uint32_t NETTIK::IController::GetNetworkRate(void) const
{
	return m_iNetworkRate;
}

inline void NETTIK::IController::SetNetworkRate(uint32_t netRate)
{
	m_iNetworkRate = netRate;
}

inline bool NETTIK::IController::IsRunning(void) const
{
	return m_bRunning;
}

template <class T>
inline T* NETTIK::IController::GetSingleton()
{
	return dynamic_cast< T* >( s_PeerSingleton );
}

inline NETTIK::IController* NETTIK::IController::GetSingleton()
{
	return ( s_PeerSingleton );
}