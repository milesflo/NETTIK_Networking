//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "CNetVarBase.h"
#include "CNetworkVector3.h"
//#define DEFINE_NetVector(name, reliable) \
//	CNetVarVector   name = CNetVarVector(this, #name, reliable)

//-----------------------------------------------
// Constructs a three dimension array to process
// floating point positional data.
//-----------------------------------------------
#define NetworkVector(name, reliable) \
	CNetVarVector   name = CNetVarVector( this, #name, reliable )

class CNetVarVector : public CNetVarBase< NETTIK::CNetworkVector3 >
{
public:
	
	CNetVarVector(NetObject* parent, const char* name, bool reliable) : CNetVarBase(parent, name, reliable)
	{

	}

	void Set(float x, float y, float z)
	{
		NETTIK::CNetworkVector3 compose(x, y, z);
		if (!SetGuard(compose))
			return;

		m_Data = compose;
	}
	
	virtual ~CNetVarVector()
	{
	
	}
};
