#pragma once
#include "CNetVarBase.h"
#include "CVector3.h"
#include "IDebug.h"
#define DEFINE_NetVector(name, reliable) \
	CNetVarVector   name = CNetVarVector(this, #name, reliable)

//! A three dimension network variable for representing
//  X,Y,Z.
class CNetVarVector : public CNetVarBase<NETTIK::CVector3>
{
public:
	
	CNetVarVector(NetObject* parent, const char* name, bool reliable) : CNetVarBase(parent, name, reliable)
	{

	}

	void Set(float x, float y, float z)
	{
		NETTIK::CVector3 compose(x, y, z);
		if (!SetGuard(compose))
			return;

		m_Data = compose;
	}
	virtual ~CNetVarVector() { }
};
