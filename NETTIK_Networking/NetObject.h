//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <enet/enet.h>
#include <vector>
#include <inttypes.h>
#include <mutex>
#include <string>

#include "SnapshotStream.h"
#include "ReplicationInfo.h"

class NetVar;
class VirtualInstance;
class IEntityManager;

class NetObject
{
private:

	// If compiled in debug mode, every network object is padded by 
	// 1MB to simulate alloc/free at a larger scale. 
#ifdef _DEBUG
	unsigned char m_pDebugBuffer[ 1024 * 1024 ];
#endif

public:

	using VariableList_t = std::vector<NetVar*>;

	uint32_t          m_NetCode;
	uint32_t          m_RealmID;
	uint32_t          m_Controller = NET_CONTROLLER_NONE;

	// Snapshotting
	// - size_t result is the LARGEST update element (for padding)
	// TODO: Pass reliable flag for generating reliable/unsequenced snapshots.
	void TakeObjectSnapshot
	(
		size_t&				max_value,
		uint16_t&			num_update,
		SnapshotStream&		buffers,
		bool				bReliableFlag,

		bool		bForced			= false
	);

	inline bool IsActive() const { return m_bActive; }
	inline bool IsServer() const { return m_bIsServer; }

	void SetIsServer(bool value) { m_bIsServer = value; }

	// Accessors for ENET peer if this has one assigned.
	ENetPeer*        m_pPeer      = nullptr;
	VirtualInstance* m_pInstance  = nullptr;
	IEntityManager*  m_pManager   = nullptr;

	virtual void NetworkUpdate(ReplicationInfo& repinfo) = 0;

public:

	std::recursive_mutex m_Mutex;

	//--------------------------------------
	// Calculates the peer's host in a 
	// printable format.
	//--------------------------------------
	std::string NetObject::GetPeerHost();

	//--------------------------------------
	// Is the object controlled by this 
	// controller.
	//--------------------------------------
	bool IsNetworkLocal() const;

	//--------------------------------------
	// Is the object controlled by a remote
	// peer.
	//--------------------------------------
	bool IsNetworkRemote() const;

	//--------------------------------------
	// Informs the parent manager to erase
	// this object from its list.
	//--------------------------------------
	void DestroyNetworkedEntity() const;

	virtual void InvalidateVars();

	VariableList_t& GetVariables();

	virtual ~NetObject();

private:
	bool m_bIsServer  = true;
	bool m_bActive    = true;

protected:
	VariableList_t m_Vars;
};

inline NetObject::VariableList_t& NetObject::GetVariables()
{
	return m_Vars;
}

inline bool NetObject::IsNetworkLocal() const
{
	return m_Controller == NET_CONTROLLER_LOCAL;
}

inline bool NetObject::IsNetworkRemote() const
{
	return m_Controller != NET_CONTROLLER_LOCAL;
}