#pragma once
#include <cinttypes>
#include "ControllerFlags.h"
#include <enet\enet.h>

class ReplicationInfo
{
private:
	bool m_bBroadcast;
	bool m_bReplicate;
public:

	ReplicationInfo() = default;

	explicit ReplicationInfo(bool server, ENetPeer* pPeer, uint32_t controller)
	{
		m_bReplicate = (server && pPeer != nullptr) || (!server && controller != NET_CONTROLLER_LOCAL);
		m_bBroadcast = (server && pPeer == nullptr) || (!server && controller == NET_CONTROLLER_LOCAL);
	}


	bool ShouldBroadcast();
	bool ShouldReplicate();
};

inline bool ReplicationInfo::ShouldBroadcast()
{
	return m_bBroadcast;
}

inline bool ReplicationInfo::ShouldReplicate()
{
	return m_bReplicate;
}
