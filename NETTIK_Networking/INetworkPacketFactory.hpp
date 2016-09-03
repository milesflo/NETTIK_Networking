//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <string>
#include <enet\enet.h>
#include <google\protobuf\wire_format.h>
#include "INetworkCodes.hpp"

#define DEFINE_GETSET(Type, MemberName, FaceName) \
	Type Get##FaceName() const { \
		return MemberName; \
	} \
	void Set##FaceName(Type value) { \
		MemberName = value; \
	}\

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

		static size_t GenerateRaw(
			std::string& stream,
			const enet_uint8* buffer,
			const size_t buffer_len,
			INetworkCodes::msg_t code
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

		static void DispatchRaw(
			const enet_uint8* buffer,
			const size_t buffer_len,
			INetworkCodes::msg_t code,
			ENetPeer* enetPeer,
			uint32_t flags,
			uint8_t channel
			);

		static ENetPeer* GetFirstPeer();

	public:

		//! Reads a network code from a packet buffer.
		static INetworkCodes::msg_t GetCode(
			std::string& data
			);

		class CBasePacket
		{
		protected:
			INetworkCodes::msg_t  m_iCode;
			PacketStatus          m_Status;
			uint8_t               m_iChannel = 0;
			uint32_t              m_Flags = 0;
			std::vector<ENetPeer*> m_pPeerList;

		public:

			DEFINE_GETSET(INetworkCodes::msg_t, m_iCode, Code);
			DEFINE_GETSET(uint32_t, m_Flags, Flags);
			DEFINE_GETSET(uint8_t, m_iChannel, Channel);

			//! Finds the default peer if pPeer is still
			// a null pointer. Throws an exception if the
			// controller is invalid or a peer cannot be found.
			void AllocateDefaultPeer();

			void SetReliable()
			{
				m_Flags |= ENET_PACKET_FLAG_RELIABLE;
			}

			void SetUnsequenced()
			{
				m_Flags |= ENET_PACKET_FLAG_UNSEQUENCED;
			}

			void AddPeer(ENetPeer* peer)
			{
				m_pPeerList.push_back(peer);
			}

			void SetPeer(ENetPeer* peer)
			{
				m_pPeerList.clear();
				AddPeer(peer);
			}

			CBasePacket() { }
			virtual ~CBasePacket() { };
		};

		class CRawPacket : public CBasePacket
		{
			CRawPacket() { }
		};

		//! The packet wrapper class for dispatching data to
		// an ENET peer.
		template <class T>
		class CProtoPacket : public T, public CBasePacket
		{
		public:

			//! Forces a dispatch of the packet, regardless of 
			// it's status. Sets the status flag to `dispatched`.
			void Dispatch()
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
			void Read(const enet_uint8* data, size_t data_length)
			{
				Disable();

				enet_uint8* stream_data;
				stream_data = const_cast<enet_uint8*>(data) + (sizeof(INetworkCodes::msg_t));

				size_t stream_len;
				stream_len = data_length - sizeof(INetworkCodes::msg_t);

				ParseFromArray(stream_data, stream_len);
			}

			//! Silently dispatches a packet, doesn't change the 
			// status of the packet.
			void DispatchSilent()
			{
				AllocateDefaultPeer();
				DispatchPacket(this, m_iCode, m_pPeer, m_Flags, m_iChannel);
			}

			//! Prevents RAII dispatching (ie. using a singleton-like
			// object)
			void Disable()
			{
				m_Status = PacketStatus::kPacket_Disabled;
			}

			void GenerateToString(std::string& out)
			{
				Disable();
				GenerateStream(out, this, m_iCode);
			}

			//! Constructs a packet with the peer being the first
			// item in the internal connection stack. Useful for client -> server as
			// the server is the only connected peer.
			CProtoPacket(INetworkCodes::msg_t code) : T(), CBasePacket()
			{
				SetCode(code);
				m_Status = PacketStatus::kPacket_Pending;
			}

			//! Constructs a packet when no packet code is supplied.
			// Usually the case when the packet is incoming.
			CProtoPacket() : T(), CBasePacket()
			{
				m_Status = PacketStatus::kPacket_Disabled;
			}
			CProtoPacket(const enet_uint8* data, size_t data_len) : T(), CBasePacket()
			{
				Read(data, data_len);
			}

			//! Constructs a packet with a desired peer to send to.
			//IPacket(INetworkCodes::msg_t code, ENetPeer* enetPeer) : m_iCode(code), m_pPeer(enetPeer)
			//{

			//}

			//! RAII supported packet dispatching, it's cleaner
			// to force a dispatch but this lets the front-end developer be lazy.
			// This also allocates to the stack, so be careful to overuse this.
			~CProtoPacket()
			{
				if (m_Status == PacketStatus::kPacket_Pending)
					Dispatch();
			}
		};

	};

};