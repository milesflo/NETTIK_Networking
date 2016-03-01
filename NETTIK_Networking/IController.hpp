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
#include "IDebug.hpp"
#include "IThread.hpp"
#include "INetworkPacketFactory.hpp"
#include "CEntities.h"

namespace NETTIK
{

	class IController
	{

	public:

		enum DISCONNECT_REASONS : NETTIK::INetworkCodes::msg_t
		{
			NETTIK_DISCONNECT_SHUTDOWN = 0xFFFF,
			NETTIK_DISCONNECT_KICK = 0xFFFE,
			NETTIK_DISCONNECT_BAN = 0xFFFD
		};

		using CallbackFunction_f =
			std::function<void(enet_uint8* data, size_t data_length, ENetPeer* enetPeer)>;

		using EventFunction_f =
			std::function<void(ENetEvent& evtFrame)>;

		static IController* GetPeerSingleton();
		static void SetPeerSingleton(IController* peer);
		static void DeletePeerSingleton();

		std::vector<ENetPeer*> m_PeerList;
		std::unordered_map<std::string, CEntities*> m_EntManagers;

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

		CEntities* CreateEntityManager(std::string name)
		{
			CEntities* mgr;
			mgr = new CEntities;
			mgr->SetName(name);

			m_EntManagers.insert(make_pair(name, mgr));
			return mgr;
		}

		void DestroyEntityManager(std::string name)
		{
			auto it = m_EntManagers.find(name);
			if (it != m_EntManagers.end())
				m_EntManagers.erase(it);
		}

		void DestroyEntityManager(CEntities* mgr)
		{
			DestroyEntityManager(mgr->GetName());
		}

	public:

		IController(uint32_t tickRate);
		virtual ~IController();

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
			if (m_PeerList.size() == 0)
				return nullptr;

			return m_PeerList.front(); /* todo: implement*/
		}

		//! Performs a single iteration over the network stack.
		void ProcessNetStack();

		//! Starts the network stack thread.
		void Start();

		//! Stops the network stack thread and deinitialises ENET.
		void Stop();

		//! Main thread entry for processing network stack.
		void Run(bool& bThreadStatus);

		//! Sends data to the ENET peer.
		void Send(enet_uint8* data, size_t data_len, ENetPeer* peer, uint32_t flags, uint8_t channel);

		//! Sends data to the first ENET peer.
		// Remember to set flag as reliable for important information.
		void Send(enet_uint8* data, size_t data_len, uint32_t flags = ENET_PACKET_FLAG_UNSEQUENCED, uint8_t channel = 0);

		//! Processes a single data stream and emits
		// the appropriate function.
		void ProcessRecv(enet_uint8* data, size_t data_length, ENetPeer* peer);

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

		void FireEvent(ENetEventType evt, ENetEvent& evtFrame);

		// TODO: add "once", "off"
		// SEE: EventEmitters in JS
	};

}