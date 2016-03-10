#pragma once
#include <enet/enet.h>
#include <unordered_map>
#include <inttypes.h>
#include <mutex>
#define DEFINE_NETOBJECT(typeName) \
	std::string GetNetObject_Name() const { return typeName; }

class NetVar;
class VirtualInstance;

class NetObject
{
public:
	uint32_t          m_NetCode;
	std::unordered_map<std::string, NetVar*> m_Vars;
	bool              m_Active = true;
	// These are set by DEFINE_NETOBJECT.
	virtual std::string   GetNetObject_Name() const = 0;

	// Snapshotting
	// - size_t result is the LARGEST update element (for padding)
	// TODO: Pass reliable flag for generating reliable/unsequenced snapshots.
	void TakeObjectSnapshot(size_t& max_value, uint16_t& num_update, std::vector<std::vector<unsigned char>>& buffers, bool bForced = false);

	// Accessors for ENET peer if this has one assigned.
	ENetPeer* m_pPeer = nullptr;
	VirtualInstance* m_pInstance = nullptr;
	std::recursive_mutex m_Mutex;

public:
	virtual ~NetObject() { };
};

