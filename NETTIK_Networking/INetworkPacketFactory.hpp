#pragma once
#include <string>
#include <enet\enet.h>
#include <google\protobuf\wire_format.h>

#include "INetworkCodes.hpp"

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

		//! Runs protobuf serialisation and allocated message code to
		// binary header.
		static size_t GenerateStream(
			std::string& stream,
			google::protobuf::Message*  msg,
			INetworkCodes::msg_t        code
			);

		//! Serializes a packet and sends it to the singleton ENET peer (if it exists), 
		// drops the packet if not.
		static void DispatchPacket(
			google::protobuf::Message*  msg,
			INetworkCodes::msg_t        code,
			ENetPeer*   enetPeer,
			uint32_t    flags,
			uint8_t     channel
			);

		static ENetPeer* GetFirstPeer();

	public:

		class INetworkPacket
		{
		public:
			virtual ~INetworkPacket() { };
		};

		//! Reads a network code from a packet buffer.
		static INetworkCodes::msg_t GetCode(
			std::string& data
			);

		//! The packet wrapper class for dispatching data to
		// an ENET peer.
		template <class T>
		class NetPacket : public T, INetworkPacket
		{

		private:

			INetworkCodes::msg_t  m_iCode;
			PacketStatus          m_Status;
			uint8_t               m_iChannel = 0;
			uint32_t              m_Flags = 0;
			std::vector<ENetPeer*> m_pPeerList;

			//! Finds the default peer if pPeer is still
			// a null pointer. Throws an exception if the
			// controller is invalid or a peer cannot be found.

			void AllocateDefaultPeer()
			{
				if (m_pPeerList.size() != 0)
					return;

				m_pPeerList.push_back(GetFirstPeer());
			}

		public:

			inline void _SetCode(INetworkCodes::msg_t code)
			{
				m_iCode = code;
			}

			inline void _SetFlags(uint32_t flags)
			{
				m_Flags = flags;
			}

			inline void _FlagAsReliable()
			{
				m_Flags |= ENET_PACKET_FLAG_RELIABLE;
			}

			inline void _FlagAsUnsequenced()
			{
				m_Flags |= ENET_PACKET_FLAG_UNSEQUENCED;
			}

			inline void _PeerAdd(ENetPeer* peer)
			{    
				m_pPeerList.push_back(peer);
			}

			inline void _PeerSet(ENetPeer* peer)
			{
				m_pPeerList.clear();
				_Peeradd(peer);
			}

			//! Assigns the packet channel to use.
			inline void _SetChannel(uint8_t channel)
			{
				m_iChannel = channel;
			}


			//! Forces a dispatch of the packet, regardless of 
			// it's status. Sets the status flag to `dispatched`.
			inline void _ForceDispatch()
			{
				AllocateDefaultPeer();

#ifdef _DEBUG
				if (m_pPeerList.size() == 0)
				{
					printf("warning: PeerList.Size == 0 from %s\n", __FUNCTION__);
				}
#endif
				for (uint32_t i = 0; i < m_pPeerList.size(); i++)
					DispatchPacket(this, m_iCode, m_pPeerList.at(i), m_Flags, m_iChannel);

				m_Status = PacketStatus::kPacket_Dispatched;
			}

			//! Reads a data stream and removes the network code
			// from the buffer.
			inline void _Read(enet_uint8* data, size_t data_length)
			{
				_PreventAutoDispatch();

				enet_uint8* stream_data;
				stream_data = data + (sizeof(INetworkCodes::msg_t));

				size_t stream_len;
				stream_len = data_length - sizeof(INetworkCodes::msg_t);

				ParseFromArray(stream_data, stream_len);
			}

			//! Silently dispatches a packet, doesn't change the 
			// status of the packet.
			inline void _SilentDispatch()
			{
				AllocateDefaultPeer();
				DispatchPacket(this, m_iCode, m_pPeer, m_Flags, m_iChannel);
			}

			//! Prevents RAII dispatching (ie. using a singleton-like
			// object)
			inline void _PreventAutoDispatch()
			{
				m_Status = PacketStatus::kPacket_Disabled;
			}

			inline void _GenerateToString(std::string& out)
			{
				_PreventAutoDispatch();
				GenerateStream(out, this, m_iCode);
			}

			//! Constructs a packet with the peer being the first
			// item in the internal connection stack. Useful for client -> server as
			// the server is the only connected peer.
			NetPacket(INetworkCodes::msg_t code) : m_iCode(code), T(), INetworkPacket()
			{
				m_Status = PacketStatus::kPacket_Pending;
			}

			//! Constructs a packet when no packet code is supplied.
			// Usually the case when the packet is incoming.
			NetPacket() : T()
			{
				m_Status = PacketStatus::kPacket_Disabled;
			}
			NetPacket(enet_uint8* data, size_t data_len) : T()
			{
				_Read(data, data_len);
			}

			//! Constructs a packet with a desired peer to send to.
			//IPacket(INetworkCodes::msg_t code, ENetPeer* enetPeer) : m_iCode(code), m_pPeer(enetPeer)
			//{

			//}

			//! RAII supported packet dispatching, it's cleaner
			// to force a dispatch but this lets the front-end developer be lazy.
			// This also allocates to the stack, so be careful to overuse this.
			~NetPacket()
			{
				if (m_Status == PacketStatus::kPacket_Pending)
					_ForceDispatch();
			}
		};

	};

};