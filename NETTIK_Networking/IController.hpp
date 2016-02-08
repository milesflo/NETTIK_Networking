#pragma once
#include <enet/enet.h>
#include <string>
#include <functional>
#include <google\protobuf\message.h>
#include <unordered_map>

#include "INetworkCodes.hpp"
#include "IDebug.hpp"
#include "IThread.hpp"

namespace NETTIK
{

	class IController
	{

	public:

		using CallbackFunction_f = 
			std::function<void(google::protobuf::Message* message, void* enetPeer)>;

		using EventFunction_f =
			std::function<void(ENetEvent& evtFrame)>;

		static IController* GetPeerSingleton();
		static void SetPeerSingleton(IController* peer);
		static void DeletePeerSingleton();

		std::vector<
			ENetPeer*
		> m_PeerList;

	protected:

		ENetAddress m_Address;
		ENetHost*   m_pHost;

		bool        m_bRunning = false;
		bool        m_bConnected = false;
		virtual void InitializeAddress() { }
		virtual void InitializeHost() = 0;

		ENetEvent   m_CurrentEvent;
		uint32_t    m_iNetworkRate;

		IThread*    m_pThread = nullptr;

	public:

		IController(uint32_t tickRate);

		virtual ~IController();

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

		void ProcessNetStack();

		void Start();

		void Run();

		//! Sends data to the ENET peer.
		void Send(std::string& data, ENetPeer* peer, uint32_t flags);

		//! Sends data to the first ENET peer.
		// Remember to set flag as reliable for important information.
		void Send(std::string& data, uint32_t flags = ENET_PACKET_FLAG_UNSEQUENCED);
		
		//! Processes a single data stream and emits
		// the appropriate function.
		void ProcessRecv(std::string& data);

	protected: 

		std::unordered_map <
			INetworkCodes::msg_t,

			std::pair <
				CallbackFunction_f,
				google::protobuf::Message*
			>

		> m_Callbacks;

		std::unordered_map <
			ENetEventType,
			EventFunction_f

		> m_EventCallbacks;

	public:

		template <class T>
		void ListenPacket(INetworkCodes::msg_t code, CallbackFunction_f callback)
		{
			m_Callbacks[code] = make_pair(code, T);
		}

		void ListenEvent(ENetEventType evt, EventFunction_f callback)
		{
			m_EventCallbacks[evt] = callback;
		}

	};

}