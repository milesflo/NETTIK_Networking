#pragma once
#include <vector>
#include "SnapshotStream.h"

class NetObject;
class NetVar
{
protected:
	NetObject* m_pParent;
	const char* m_Name;

	bool        m_Reliable;

public:
	virtual size_t TakeVariableSnapshot(SnapshotStream& buffers, bool bForced) = 0;

	//! Internal setting of anonymous data.
	virtual void Set(unsigned char* ptr, size_t size) = 0;

	inline const char*  GetName()     const { return m_Name; }
	inline NetObject*   GetParent()   const { return m_pParent; }
	inline bool const   GetReliable() const { return m_Reliable; }

	NetVar(NetObject* parent, const char* name, bool reliable);

	virtual ~NetVar();
};
