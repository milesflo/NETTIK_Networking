//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "NetVar.h"
#include "Constraints.h"
#include "SnapshotEntList.h"
#include <cstddef>

#define NetworkVariable(type, name, reliable) \
	CNetVarBase< type > name = { this, #name, reliable }

//! A templated class for representing a fixed data type (non-pointer)
//  network variable.
template< class VarType >
class CNetVarBase : public NetVar
{
	static_assert(sizeof(VarType) < max_entvar_data, "Sizeof type templated to CNetVarBase is too large.");

private:
	bool        m_bChanged	= true;		// Set to true when the variable has differed.
	bool        m_bLocked	= false;	// Set to true when the variable cannot be changed (remotely).
	bool		m_bChangeForced = false;


protected:
	VarType     m_Data;

	bool SetGuard(VarType& data);

public:

	void* _get() { return static_cast<void*>( &m_Data ); }

	//! Lock and prevent remote changes (disposed).
	void Lock();

	//! Unlock and allow remote changes.
	void Unlock();

	//! Returns the mutable variable stored in the netvar.
	VarType& get();

	//! Invalidates the contents and resends to clients.
	void Invalidate();

	//! Takes a snapshot of the variable state.
	size_t TakeVariableSnapshot(SnapshotStream& buffers, bool bForced);


	//! Sets raw data and invalidates packet. Natural process is the function is called remotely.
	void Set(unsigned char* ptr, size_t size, ENetPeer* pWho);

	//! Sets the data with type safeness and invalidates the packet.
	void Set(VarType data);

	CNetVarBase(NetObject* parent, const char* name, bool reliable);

	virtual ~CNetVarBase();

};

template< class VarType >
inline void CNetVarBase<VarType>::Lock()
{
	m_bLocked = true;
}

template< class VarType >
inline void CNetVarBase<VarType>::Unlock()
{
	m_bLocked = false;
}

template< class VarType >
CNetVarBase<VarType>::~CNetVarBase()
{

}

template <class VarType >
void CNetVarBase<VarType>::Invalidate()
{
	m_bChanged = true;
	m_bChangeForced = true;
}

template< class VarType >
CNetVarBase<VarType>::CNetVarBase(NetObject* parent, const char* name, bool reliable) : NetVar(parent, name, reliable)
{
	unsigned char* pVarData;
	pVarData = reinterpret_cast<unsigned char*>(&m_Data);

	// For data safety, fill the variable with 00 bytes.
	for (size_t i = 0; i < sizeof(VarType); i++)
	{
		pVarData[ i ] = 0;
	}
}

template< class VarType >
inline VarType& CNetVarBase<VarType>::get()
{
	return m_Data;
}

template< class VarType >
size_t CNetVarBase<VarType>::TakeVariableSnapshot(SnapshotStream& buffers, bool bForced)
{
	// Variable hasn't changed and snapshot isn't forced.
	if (!bForced && !m_bChanged)
		return 0;

	// This is slow AF.
	// TODO: Pass character buffer and fill and update
	// an index of the current stream length.
	uint32_t code;
	code = NETID_Reserved::RTTI_Object::OBJECT_FRAME;

	SnapshotStream::Stream& buffer = buffers.create();

	SnapshotEntList generator;
	generator.set_frametype(FrameType::kFRAME_Data);
	generator.set_name(m_Name);
	generator.set_netid(m_pParent->GetNetID());
	generator.set_data(reinterpret_cast<unsigned char*>(&m_Data), sizeof(VarType));

	if (m_bChangeForced || bForced)
	{
		generator.set_forced(true);
	}
	else
	{
		generator.set_forced(false);
	}

	generator.write(buffer);

	m_bChanged = false;
	m_bChangeForced = false;
	return buffer.size();
}


template< class VarType >
void CNetVarBase<VarType>::Set(unsigned char* ptr, size_t size, ENetPeer* pWho)
{
	if (m_bLocked)
		return;

	if (size == 0)
		size = sizeof(VarType);

	unsigned char* data;
	data = reinterpret_cast<unsigned char*>(&m_Data);

	for (size_t i = 0; i < size; i++)
	{
		data[i] = ptr[i];
	}

	m_iChanges++;

	m_bChanged = true;
}

template< class VarType >
void CNetVarBase<VarType>::Set(VarType data)
{
	if (!SetGuard(data))
		return;

	m_Data = data;
	m_iChanges++;
	m_bChangeForced = true;
}

template< class VarType >
inline bool CNetVarBase<VarType>::SetGuard(VarType& data)
{
	if (m_pParent != nullptr && !m_pParent->IsActive())
		return false;

	if (m_Data == data)
		return false;

	m_bChanged = true;
	m_bChangeForced = true;

	return true;
}