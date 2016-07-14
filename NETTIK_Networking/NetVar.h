//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <vector>
#include <enet/enet.h>
#include "SnapshotStream.h"

class NetObject;

class NetVar
{
protected:
	NetObject*  m_pParent;
	const char* m_Name;

	bool        m_Reliable = true;

public:

	//-----------------------------------------
	// Processes internal variable snapshot.
	//-----------------------------------------
	virtual size_t TakeVariableSnapshot(SnapshotStream& buffers, bool bForced) = 0;

	//--------------------------
	// Internal blob copy.
	//--------------------------
	virtual void Set( unsigned char* ptr, size_t size, ENetPeer* pWho ) = 0;

	inline const char*  GetName()     const { return m_Name; }
	inline NetObject*   GetParent()   const { return m_pParent; }
	inline bool const   GetReliable() const { return m_Reliable; }

	NetVar(NetObject* parent, const char* name, bool reliable);

	virtual ~NetVar();
};
