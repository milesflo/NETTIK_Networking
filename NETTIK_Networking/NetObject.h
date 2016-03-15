#pragma once
#include <enet/enet.h>
#include <unordered_map>
#include <inttypes.h>
#include <mutex>

#include "SnapshotStream.h"
#define DEFINE_NETOBJECT(typeName) \
	std::string GetNetObject_Name() const { return typeName; }

class NetVar;
class VirtualInstance;
class IEntityManager;

class NetObject
{
private:
	bool m_bIsServer = true;
public:
	uint32_t          m_NetCode;
	uint32_t          m_RealmID;

	std::unordered_map<std::string, NetVar*> m_Vars;
	bool              m_Active = true;
	// These are set by DEFINE_NETOBJECT.
	virtual std::string   GetNetObject_Name() const = 0;

	// Snapshotting
	// - size_t result is the LARGEST update element (for padding)
	// TODO: Pass reliable flag for generating reliable/unsequenced snapshots.
	void TakeObjectSnapshot(size_t& max_value, uint16_t& num_update, SnapshotStream& buffers, bool bReliableFlag, bool bForced = false);

	inline bool IsServer() const { return m_bIsServer; }
	void SetIsServer(bool value) { m_bIsServer = value; }

	// Accessors for ENET peer if this has one assigned.
	ENetPeer*        m_pPeer = nullptr;
	VirtualInstance* m_pInstance = nullptr;
	IEntityManager*  m_pManager = nullptr;

	virtual void Update() = 0;

public:
	std::recursive_mutex m_Mutex;

	void DestroyNetworkedEntity();

	virtual ~NetObject()
	{
	};
};

