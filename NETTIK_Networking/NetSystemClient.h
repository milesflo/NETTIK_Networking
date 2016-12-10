//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "NetSystem.h"
#include <unordered_map>

//------------------------------------------------------------------------
// NetSystemClient: A core network controller that implements client 
// connection and entity synch.
//------------------------------------------------------------------------
class NetSystemClient : public NetSystem
{
public:
	NetSystemClient();

	NetSystemClient(uint32_t rate);

	virtual ~NetSystemClient();


	//--------------------------------------------------
	// Updates underlying instances.
	//--------------------------------------------------
	void ControllerUpdate(float elapsedTime);

	//--------------------------------------------------
	// Clientside entity management.
	//--------------------------------------------------
	void EntAllocate(SnapshotEntList& frame);
	void EntDeallocate(SnapshotEntList& frame);


	//--------------------------------------------------
	// Clientside ownership and snapshotting.
	//--------------------------------------------------
	void HandleEntOwnership(const enet_uint8* data, size_t data_len, ENetPeer* peer);
	void HandleEntSnapshot(const enet_uint8* data, size_t data_len, ENetPeer* peer);


	//--------------------------------------------------
	// Initialises the ENET client connection 
	// request.
	//--------------------------------------------------
	bool InitializeHost();
	bool InitializeAddress(const char* hostname, uint16_t port);
	bool Connect(const char* hostname, uint16_t port);

private:
	std::unordered_map<NetObject::NetID, std::shared_ptr<NetObject>> m_ControlledObjects;
};
