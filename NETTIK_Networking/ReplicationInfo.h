//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once

class ReplicationInfo
{
private:
	bool  m_bBroadcast;
	bool  m_bReplicate;
	float m_ElapsedTime;

public:

	ReplicationInfo() = default;

	explicit ReplicationInfo(bool server, ENetPeer* pPeer, uint32_t controller, float elapsedTime)
	{
		m_bReplicate = (server && pPeer != nullptr) || (!server && controller != NET_CONTROLLER_LOCAL);
		m_bBroadcast = (server && pPeer == nullptr) || (!server && controller == NET_CONTROLLER_LOCAL);
		m_ElapsedTime = elapsedTime;
	}

	float ElapsedTime();
	bool ShouldBroadcast();
	bool ShouldReplicate();
};

inline float ReplicationInfo::ElapsedTime()
{
	return m_ElapsedTime;
}

inline bool ReplicationInfo::ShouldBroadcast()
{
	return m_bBroadcast;
}

inline bool ReplicationInfo::ShouldReplicate()
{
	return m_bReplicate;
}
