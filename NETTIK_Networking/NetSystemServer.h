//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "NetSystem.h"

class NetSystemServer : public NetSystem
{
public:
	size_t m_iPeerLimit;

public:

	//! Update routines.
	void ControllerUpdate(float elapsedTime);
		
	void HandleClientEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer);

	explicit NetSystemServer(uint32_t rate = 0);
	virtual ~NetSystemServer();

	bool InitializeHost();
	bool InitializeAddress(const char* hostname, uint16_t port);

	bool Listen(std::uint16_t port, size_t peerLimit, const char* psHostname);

};