//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "CNetVarBase.h"
#include "CVector3.h"

//-----------------------------------------------
// Constructs a three dimension array to process
// floating point positional data.
//-----------------------------------------------
#define NetworkVector(name, reliable) \
	CNetVarVector   name = CNetVarVector( this, #name, reliable )

class CNetVarVector : public CNetVarBase< gen_net::CVector3 >
{
public:
	
	CNetVarVector(NetObject* parent, const char* name, bool reliable);

	void Set(float x, float y, float z);
};
