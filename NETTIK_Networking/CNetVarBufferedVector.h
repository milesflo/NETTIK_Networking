//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "NetVarBufferBase.h"
#include "CVector3.h"

//--------------------------------------------------
// Constructs a buffered vector for interpolation.
//--------------------------------------------------
#define NetworkBufferedVector(name, reliable) \
	CNetVarBufferedVector   name = CNetVarBufferedVector( this, #name, reliable )

class CNetVarBufferedVector : public NetVarBufferBase<gen_net::CVector3>
{
public:
	CNetVarBufferedVector(NetObject* parent, const char* name, bool reliable);

	//--------------------------------------------------
	// Calculates the interpolated value of the vectors
	// contents 
	//--------------------------------------------------
	gen_net::CVector3 GetInterpolated(std::chrono::milliseconds dtms);

	//--------------------------------------------------
	// Standard vector manipulator.
	//--------------------------------------------------
	void Set(float x, float y, float z);

protected:
	// Maximum amount of distance to interpolate to, anything
	// higher will be skipped.
	static const float  kMaxInterpolationDistance;

};
