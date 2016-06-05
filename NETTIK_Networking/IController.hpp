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

	class IController
	{

	public:

		class Timer
		{
		private:
			bool m_bDelegated = false;

			uint32_t m_rate;
			std::chrono::steady_clock::time_point m_begin;

		public:
			Timer(uint32_t rate);
			~Timer();
		};

		enum DISCONNECT_REASONS : NETTIK::INetworkCodes::msg_t
		{
			NETTIK_DISCONNECT_SHUTDOWN = 0xFFFF,
			NETTIK_DISCONNECT_KICK = 0xFFFE,
			NETTIK_DISCONNECT_BAN = 0xFFFD
		};

		using CallbackFunction_f =
			std::function<void(const enet_uint8* data, size_t data_length, ENetPeer* enetPeer)>;

		using EventFunction_f =
			std::function<void(ENetEvent& evtFrame)>;

		static IController* GetPeerSingleton();
		static void SetPeerSingleton(IController* peer);
		static void DeletePeerSingleton();

		std::vector<ENetPeer*> m_PeerList;
		std::unordered_map<std::string, VirtualInstance_ptr> m_Instances;

	protected:

		ENetAddress m_Address;
		ENetHost*   m_pHost;

		bool        m_bShuttingDown = false;
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
		uint32_t    m_iFPS;

		IThread*    m_pThread = nullptr;
		IThread*    m_pSyncThread = nullptr;

		bool        m_bServer;
		bool        m_bDumpMode = false;

	public:

		void Dump();

		void ReadEntityUpdate(SnapshotEntList& frame, ENetPeer* owner = nullptr);

		//! Returns the single host used by this controller.
		ENetHost* GetHost() const;

		IController(uint32_t tickRate);
		virtual ~IController();

		void Destroy();

		//! Performs a post update, handled by server/client.
		virtual void PostUpdate(float elapsedTime) = 0;

		//! Performs a controller type update.
		virtual void ControllerUpdate(float elapsedTime) = 0;

		//! Returns if the controller is a server or not.
		bool IsServer();

		//! Performs a tick, calls PostUpdate.
		void Update(float elapsedTime);

		void SnapshotUpdate(float elapsedTime);

		//! Returns rate that messages get processed.
		uint32_t GetNetworkRate(void) const;

		//! Sets the rate that messages get processed.
		void SetNetworkRate(uint32_t netRate);

		//! Returns if the controller is running or not.
		bool IsRunning(void) const;

		//! Useful for clients as the first peer will be
		// the server.
		ENetPeer* GetFirstPeer();

		static std::string GetIPAddress(ENetAddress& addr);

		//! Performs a single iteration over the network stack.
		void ProcessNetStack();

		//! Starts the network stack thread.
		void Start();

		//! Stops the network stack thread and deinitialises ENET.
		void Stop();

		//! Main thread entry for processing network stack.
		void Run(bool& bThreadStatus);

		//! Broadcasts data to all connected ENET peers.
		void Broadcast(const enet_uint8* data, size_t data_len, uint32_t flags, uint8_t channel);

		//! Sends data to the ENET peer.
		void Send(const enet_uint8* data, size_t data_len, ENetPeer* peer, uint32_t flags, uint8_t channel);

		//! Broadcast SnapshotStream
		void BroadcastStream(SnapshotStream& stream, bool reliable);

		//! Sends SnapshotStream
		void SendStream(SnapshotStream& stream, bool reliable, ENetPeer* peer = nullptr);

		//! Sends data to the first ENET peer.
		// Remember to set flag as reliable for important information.
		void Send(const enet_uint8* data, size_t data_len, uint32_t flags = ENET_PACKET_FLAG_UNSEQUENCED, uint8_t channel = 0);

		//! Processes a single data stream and emits
		// the appropriate function.
		void ProcessRecv(const enet_uint8* data, size_t data_length, ENetPeer* peer);

		VirtualInstance* CreateInstance(std::string name);
		VirtualInstance* GetInstance(std::string name);

		std::unordered_map<std::string, VirtualInstance*> GetInstances();

		void DeleteInstance(std::string name);
		void DeleteInstance(VirtualInstance* instance);

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

		void on(INetworkCodes::msg_t code, CallbackFunction_f callback);

		void on_enet(ENetEventType evt, EventFunction_f callback);

		void FireEvent(ENetEventType evt, ENetEvent& evtFrame);

	};

	inline ENetHost* IController::GetHost() const
	{
		return m_pHost;
	}

	inline void IController::on(INetworkCodes::msg_t code, CallbackFunction_f callback)
	{
		m_Callbacks[code].push_back(callback);
	}

	inline void IController::on_enet(ENetEventType evt, EventFunction_f callback)
	{
		m_EventCallbacks[evt].push_back(callback);
	}

	inline ENetPeer* IController::GetFirstPeer()
	{
		if (m_PeerList.size() == 0)
			return nullptr;

		return m_PeerList.front(); /* todo: implement*/
	}

	inline uint32_t IController::GetNetworkRate(void) const
	{
		return m_iNetworkRate;
	}

	inline void IController::SetNetworkRate(uint32_t netRate)
	{
		m_iNetworkRate = netRate;
	}

	inline bool IController::IsRunning(void) const
	{
		return m_bRunning;
	}
}
