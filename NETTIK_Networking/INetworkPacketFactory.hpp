#pragma once
#include <google\protobuf\wire_format.h>

#include "INetworkCodes.hpp"
#include "IController.hpp"

#include <string>
#include "IDebug.hpp"

namespace NETTIK
{
	class IPacketFactory
	{

	protected:

		enum class PacketStatus : uint8_t
		{
			kPacket_Pending = 0,
			kPacket_InlineDisabled,
			kPacket_Dispatched
		};

		//! Serializes a packet and sends it to the singleton ENET peer (if it exists), 
		// drops the packet if not.
		static void DispatchPacket(google::protobuf::Message* msg, INetworkCodes::msg_t code, void* enetPeer);

	public:

		//! The packet wrapper class for dispatching data to
		// an ENET peer.
		template <class T>
		class IPacket : public T
		{
			
		private:

			INetworkCodes::msg_t  m_iCode;
			PacketStatus          m_Status;
			void*                 m_pPeer     = nullptr;
			uint16_t              m_iChannel  = 1;

		public:
			
			//! Forces a dispatch of the packet, regardless of 
			// it's status. Sets the status flag to `dispatched`.
			__forceinline 
			void _ForceDispatch()
			{
				DispatchPacket(this, m_iCode, m_pPeer);
				m_Status = PacketStatus::kPacket_Dispatched;
			}

			__forceinline
			void _SetChannel(uint16_t channel)
			{
				m_iChannel = channel;
			}

			//! Silently dispatches a packet, doesn't change the 
			// status of the packet.
			__forceinline
			void _SilentDispatch()
			{
				DispatchPacket(this, m_iCode, m_pPeer);
			}

			//! Prevents RAII dispatching (ie. using a singleton-like
			// object)
			void _PreventAutoDispatch()
			{
				m_Status = PacketStatus::kPacket_InlineDisabled;
			}

			//! Constructs a packet with the peer being the first
			// item in the internal connection stack. Useful for client -> server as
			// the server is the only connected peer.
			IPacket(INetworkCodes::msg_t code) : m_iCode(code)
			{
				IController* controller;
				controller = IController::GetPeerSingleton();

				if (controller == nullptr)
					NETTIK_EXCEPTION("Failed to get global controller object, nullptr.");

				void* enetPeer = controller->GetFirstPeer();

				if (enetPeer == nullptr)
					NETTIK_EXCEPTION("Failed to get first peer from controller object, it is nullptr.");

				m_pPeer = enetPeer;
			}

			//! Constructs a packet with a desired peer to send to.
			IPacket(INetworkCodes::msg_t code, void* enetPeer) : m_iCode(code), m_pPeer(enetPeer)
			{

			}

			//! RAII supported packet dispatching, it's cleaner
			// to force a dispatch but this lets the front-end developer be lazy.
			// This also allocates to the stack, so be careful to overuse this.
			~IPacket()
			{
				if (m_Status == PacketStatus::kPacket_Pending)
					_ForceDispatch();
			}
		};


	};

};