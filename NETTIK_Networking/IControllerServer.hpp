#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerServer : public IController
	{
	public:
		DEFINE_SERVER(true);

		size_t m_iPeerLimit;

	public:

		//! Broadcast SnapshotStream
		void BroadcastStream(SnapshotStream& stream, bool reliable)
		{
			uint32_t flags;
			flags = reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;

			Broadcast(&stream.result()[0], stream.result().size(), flags, 0);
		}

		void SendStream(SnapshotStream& stream, bool reliable, ENetPeer* peer)
		{
			uint32_t flags;
			flags = reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED;

			// This is a specific snapshot to resync a plsyer, don't do any tick logic.
			// Controller -> Send ( peer, header, buffer )
			Send(&stream.result()[0], stream.result().size(), peer, flags, 0);
		}



		//! Update routines.
		void ControllerUpdate();

		IControllerServer();
		IControllerServer(uint32_t rate);
		virtual ~IControllerServer();

		bool InitializeHost();
		bool InitializeAddress(const char* hostname, uint16_t port);

		bool Listen(uint16_t port, size_t peerLimit);

	};
}
