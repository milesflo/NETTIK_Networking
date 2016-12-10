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
class NetVarListBase;
class VirtualInstance;
class IEntityManager;

//---------------------------------------------------
// NetObject: Base object for transmitting networked
// states to NetSystem. 
//---------------------------------------------------
class NetObject
{
public:
	using VariableList_t = std::vector<NetVar*>;
	using MapList_t      = std::vector<NetVarListBase*>;
	using NetID          = std::uint32_t;

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
	inline bool IsReplicated() const { return m_bIsReplicated; }

	// Replicated objects have been sent by the server and should
	// clean up when the server tells them to.
	void SetIsReplicated(bool value) { m_bIsReplicated = value; }

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

	//--------------------------------------
	// Called after the structure has been
	// fully prepared for network use.
	//--------------------------------------
	virtual void Initialize();

	virtual ~NetObject();

	//--------------------------------------
	// Network ID manipulation
	//--------------------------------------
	NetID GetNetID() const;
	void SetNetID(NetObject::NetID code);

	//--------------------------------------
	// Realm ID manipulation
	//--------------------------------------
	NetID GetRealmID() const;
	void SetRealmID(NetObject::NetID code);

	//--------------------------------------
	// Controller manipulation
	//--------------------------------------
	std::uint32_t GetController() const;
	void SetController(std::uint32_t controller);

protected:
	VariableList_t m_Vars;
	MapList_t      m_MapList;

private:
	bool m_bIsReplicated = false;
	bool m_bActive       = true;

	NetID          m_NetCode;
	std::uint32_t  m_RealmID;
	std::uint32_t  m_Controller = NET_CONTROLLER_NONE;
};

//--------------------------------------
// Controller manipulation
//--------------------------------------
inline std::uint32_t NetObject::GetController() const
{
	return m_Controller;
}
inline void NetObject::SetController(std::uint32_t controller)
{
	m_Controller = controller;
}

//--------------------------------------
// Network ID manipulation
//--------------------------------------
inline NetObject::NetID NetObject::GetNetID() const
{
	return m_NetCode;
}

inline void NetObject::SetNetID(NetObject::NetID code)
{
	m_NetCode = code;
}

//--------------------------------------
// Realm ID manipulation
//--------------------------------------
inline NetObject::NetID NetObject::GetRealmID() const
{
	return m_RealmID;
}

inline void NetObject::SetRealmID(NetObject::NetID code)
{
	m_RealmID = code;
}

//--------------------------------------
// Returns the list of map objects.
//--------------------------------------
inline NetObject::MapList_t& NetObject::GetMaps()
{
	return m_MapList;
}

//--------------------------------------
// Returns the list of variable objects.
//--------------------------------------
inline NetObject::VariableList_t& NetObject::GetVariables()
{
	return m_Vars;
}

//--------------------------------------
// Is the object controlled by this 
// controller.
//--------------------------------------
inline bool NetObject::IsNetworkLocal() const
{
	return m_Controller == NET_CONTROLLER_LOCAL;
}

//--------------------------------------
// Is the object controlled by a remote
// peer.
//--------------------------------------
inline bool NetObject::IsNetworkRemote() const
{
	return m_Controller != NET_CONTROLLER_LOCAL;
}