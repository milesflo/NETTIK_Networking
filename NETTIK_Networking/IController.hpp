#pragma once
#include <enet\enet.h>
#include <google\protobuf\message.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "INetworkCodes.hpp"
#include "IDebug.hpp"
#include "IThread.hpp"
#include "IAtomObject.hpp"


namespace NETTIK
{

	class IController
	{

	public:

		using CallbackFunction_f = 
			std::function<void(std::string& data, ENetPeer* enetPeer)>;

		using EventFunction_f =
			std::function<void(ENetEvent& evtFrame)>;

		static IController* GetPeerSingleton();
		static void SetPeerSingleton(IController* peer);
		static void DeletePeerSingleton();

		std::vector<
			ENetPeer*
		> m_PeerList;

		std::vector<
			std::unique_ptr<IAtomObject>
		> m_AtomObjects;

		std::mutex m_ObjectListMutex;

	protected:

		ENetAddress m_Address;
		ENetHost*   m_pHost;

		bool        m_bRunning = false;
		bool        m_bConnected = false;
		bool        m_bReplicating;

		//! Initialises the ENET address data, this
		// differs per implementation.
		virtual bool InitializeAddress(const char* hostname, uint16_t port) = 0;

		//! Initialises and establishes the implementation
		// host.
		virtual bool InitializeHost() = 0;

		ENetEvent   m_CurrentEvent;
		uint32_t    m_iNetworkRate;

		IThread*    m_pThread = nullptr;

	public:

		IController(uint32_t tickRate);
		virtual ~IController();

		//! Inserts an object into the sync list.
		void AddObject(std::unique_ptr<IAtomObject> obj);

		//! Erases an object from the sync list.
		void RemoveObject(IAtomObject* obj);

		//! Performs a post update, handled by server/client.
		virtual void PostUpdate() = 0;

		void Update();

		inline uint32_t GetNetworkRate(void) const
		{
			return m_iNetworkRate;
		}

		inline void SetNetworkRate(uint32_t netRate)
		{
			m_iNetworkRate = netRate;
		}

		inline bool IsRunning(void) const
		{
			return m_bRunning;
		}

		//! Useful for clients as the first peer will be
		// the server.
		inline ENetPeer* GetFirstPeer()
		{
			return m_PeerList.front(); /* todo: implement*/
		}

		//! Performs a single iteration over the network stack.
		void ProcessNetStack();

		//! Starts the network stack thread.
		void Start();

		//! Stops the network stack thread and deinitialises ENET.
		void Stop();

		//! Main thread entry for processing network stack.
		void Run();

		//! Sends data to the ENET peer.
		void Send(std::string& data, ENetPeer* peer, uint32_t flags, uint8_t channel);

		//! Sends data to the first ENET peer.
		// Remember to set flag as reliable for important information.
		void Send(std::string& data, uint32_t flags = ENET_PACKET_FLAG_UNSEQUENCED, uint8_t channel = 0);
		
		//! Processes a single data stream and emits
		// the appropriate function.
		void ProcessRecv(std::string& data, ENetPeer* peer);

	protected: 

		std::unordered_map <
			INetworkCodes::msg_t,
			std::vector<CallbackFunction_f>

		> m_Callbacks;

		std::unordered_map <
			ENetEventType,
			std::vector<EventFunction_f>

		> m_EventCallbacks;

	public:

		inline void on(INetworkCodes::msg_t code, CallbackFunction_f callback)
		{
			m_Callbacks[code].push_back(callback);
		}

		inline void on_enet(ENetEventType evt, EventFunction_f callback)
		{
			m_EventCallbacks[evt].push_back(callback);
		}

		inline void FireEvent(ENetEventType evt, ENetEvent& evtFrame);

		// TODO: add "once", "off"
		// SEE: EventEmitters in JS
	};

}