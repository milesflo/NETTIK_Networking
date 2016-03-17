#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerClient : public IController
	{
	public:
		IControllerClient();
		IControllerClient(uint32_t rate);
		virtual ~IControllerClient();

		//! Update routines.
		void ControllerUpdate();

		//! Entity replication

		void EntAllocate(SnapshotEntList& frame);
		void EntDeallocate(SnapshotEntList& frame);
		void EntUpdate(SnapshotEntList& frame);

		void HandleEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer);

		//! Creatse the client connection framework.
		bool InitializeHost();
		bool InitializeAddress(const char* hostname, uint16_t port);

		//! Connects to a specified hostname and port.
		bool Connect(const char* hostname, uint16_t port);

	};
}
