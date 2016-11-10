//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerServer : public IController
	{
	public:
		size_t m_iPeerLimit;

	public:

		//! Update routines.
		void ControllerUpdate(float elapsedTime);
		
		void HandleClientEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer);

		explicit IControllerServer(uint32_t rate = 0);
		virtual ~IControllerServer();

		bool InitializeHost();
		bool InitializeAddress(const char* hostname, uint16_t port);

		bool Listen(size_t peerLimit, uint16_t port, const char* hostname);

	};
}
