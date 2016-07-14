//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "IController.hpp"
#include <unordered_map>

namespace NETTIK
{

	class IControllerClient : public IController
	{
	private:
		std::unordered_map<uint32_t, NetObject*> m_ControlledObjects;
	public:
		IControllerClient();
		IControllerClient(uint32_t rate);
		virtual ~IControllerClient();

		//! Update routines.
		void ControllerUpdate(float elapsedTime);

		//! Entity replication

		void EntAllocate(SnapshotEntList& frame);
		void EntDeallocate(SnapshotEntList& frame);

		void HandleEntOwnership(const enet_uint8* data, size_t data_len, ENetPeer* peer);
		void HandleEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer);

		//! Creatse the client connection framework.
		bool InitializeHost();
		bool InitializeAddress(const char* hostname, uint16_t port);

		//! Connects to a specified hostname and port.
		bool Connect(const char* hostname, uint16_t port);

	};
}
