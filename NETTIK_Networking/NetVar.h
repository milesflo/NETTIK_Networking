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
public:

	//-----------------------------------------
	// Returns the amount of data updates
	// performed on this variable.
	//-----------------------------------------
	size_t GetChanges() const;

	//-----------------------------------------
	// Resets changes to zero.
	//-----------------------------------------
	void ClearChanges();

	//-----------------------------------------
	// Increments the changes value to provoke
	// a scripted update.
	//-----------------------------------------
	void InvalidateChanges();

	//-----------------------------------------
	// Processes internal variable snapshot.
	//-----------------------------------------
	virtual size_t TakeVariableSnapshot( SnapshotStream& buffers, bool bForced ) = 0;

	//--------------------------
	// Internal blob copy.
	//--------------------------
	virtual void Set( unsigned char* ptr, size_t size, ENetPeer* pWho ) = 0;

	//------------------------------
	// Implementation to return
	// the data stored in the var
	//------------------------------
	virtual void* _get() = 0;

	inline const char*  GetName()     const { return m_Name; }
	inline NetObject*   GetParent()   const { return m_pParent; }
	inline bool const   GetReliable() const { return m_Reliable; }

	NetVar(NetObject* parent, const char* name, bool reliable);

	virtual ~NetVar();

protected:
	size_t      m_iChanges = 0;		// Number of changes performed to this variable.
	NetObject*  m_pParent;			// Parent object.
	const char* m_Name;				// Name of variable.

	bool        m_Reliable = true;	// Should be transported via reliable channels?

};

inline void NetVar::ClearChanges()
{
	m_iChanges = 0;
}

inline size_t NetVar::GetChanges() const
{
	return m_iChanges;
}

inline void NetVar::InvalidateChanges()
{
	++ m_iChanges;
}

