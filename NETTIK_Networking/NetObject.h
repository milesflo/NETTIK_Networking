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
class CNetVarList;
class VirtualInstance;
class IEntityManager;

class NetObject
{
#ifdef _DEBUG
private:
	// If compiled in debug mode, every network object is padded by 
	// 1MB to simulate alloc/free at a larger scale. 
	unsigned char m_pDebugBuffer[ 1024 * 1024 ];
#endif

public:

	using VariableList_t = std::vector<NetVar*>;
	using MapList_t      = std::vector<CNetVarList*>;
	
	uint32_t          m_NetCode;
	uint32_t          m_RealmID;
	uint32_t          m_Controller = NET_CONTROLLER_NONE;

	//---------------------------------------------------
	// Takes a variable snapshot and creates a new entry
	// to a passes SnapshotStream. Setting bForced will
	// make a full snapshot of all variables, regardless
	// of delta compression. max_value will update to the
	// largest updated element (for future padding).
	//---------------------------------------------------
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

	//--------------------------------------
	// Called on each network synch tick.
	//--------------------------------------
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

	//--------------------------------------
	// Flags all variables to be transmitted
	// over all associated peers.
	//--------------------------------------
	virtual void InvalidateVars();

	//--------------------------------------
	// Returns the list of variable objects.
	//--------------------------------------
	VariableList_t& GetVariables();

	//--------------------------------------
	// Returns the list of map objects.
	//--------------------------------------
	MapList_t& GetMaps();

	void UpdateListDelta();

	void SendLists(ENetPeer* pRemotePeer);

	virtual ~NetObject();

private:
	bool m_bIsServer  = true;
	bool m_bActive    = true;

protected:
	VariableList_t m_Vars;
	MapList_t      m_MapList;
};

inline NetObject::MapList_t& NetObject::GetMaps()
{
	return m_MapList;
}

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