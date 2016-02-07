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
			kPacket_Disabled,
			kPacket_Dispatched
		};

		static void GenerateStream(std::string& out, google::protobuf::Message* msg, INetworkCodes::msg_t code);

		//! Serializes a packet and sends it to the singleton ENET peer (if it exists), 
		// drops the packet if not.
		static void DispatchPacket(google::protobuf::Message* msg, INetworkCodes::msg_t code, void* enetPeer);

	public:

		static INetworkCodes::msg_t GetCode(std::string& data)
		{
			INetworkCodes::msg_t result;
			for (uint32_t i = 0; i < sizeof(INetworkCodes::msg_t); i++)
				(&result)[i] = data.at(i);


			return result;
		}

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

			void AllocateDefaultPeer()
			{
				if (m_pPeer != nullptr)
					return;

				IController* controller;
				controller = IController::GetPeerSingleton();

				if (controller == nullptr)
					NETTIK_EXCEPTION("Failed to get global controller object, nullptr.");

				void* enetPeer = controller->GetFirstPeer();

				if (enetPeer == nullptr)
					NETTIK_EXCEPTION("Failed to get first peer from controller object, it is nullptr.");

				m_pPeer = enetPeer;
			}

		public:
			
			//! Forces a dispatch of the packet, regardless of 
			// it's status. Sets the status flag to `dispatched`.
			inline
			void _ForceDispatch()
			{
				AllocateDefaultPeer();
				DispatchPacket(this, m_iCode, m_pPeer);

				m_Status = PacketStatus::kPacket_Dispatched;
			}

			inline
			void _Read(std::string& data)
			{
				_PreventAutoDispatch();
				data.erase(0, sizeof(INetworkCodes::msg_t));
				ParseFromString(data);
			}

			inline
			void _SetChannel(uint16_t channel)
			{
				m_iChannel = channel;
			}

			//! Silently dispatches a packet, doesn't change the 
			// status of the packet.
			inline
			void _SilentDispatch()
			{
				AllocateDefaultPeer();
				DispatchPacket(this, m_iCode, m_pPeer);
			}

			//! Prevents RAII dispatching (ie. using a singleton-like
			// object)
			inline
			void _PreventAutoDispatch()
			{
				m_Status = PacketStatus::kPacket_Disabled;
			}

			inline
			void _GenerateToString(std::string& out)
			{
				_PreventAutoDispatch();
				GenerateStream(out, this, m_iCode);
			}

			//! Constructs a packet with the peer being the first
			// item in the internal connection stack. Useful for client -> server as
			// the server is the only connected peer.
			IPacket(INetworkCodes::msg_t code) : m_iCode(code), T()
			{
				m_Status = PacketStatus::kPacket_Pending;
			}

			//! Constructs a packet when no packet code is supplied.
			// Usually the case when the packet is incoming.
			IPacket() : T()
			{
				m_Status = PacketStatus::kPacket_Disabled;
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