//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "NetVarBufferBase.h"
#include "CQuaternion.h"

//--------------------------------------------------
// Constructs a buffered vector for interpolation.
//--------------------------------------------------
#define NetworkBufferedQuat(name, reliable) \
	CNetVarBufferedQuat name = CNetVarBufferedQuat( this, #name, reliable )

class CNetVarBufferedQuat : public NetVarBufferBase<gen_net::CQuaternion>
{
public:
	CNetVarBufferedQuat(NetObject* parent, const char* name, bool reliable);

	//--------------------------------------------------
	// Calculates the interpolated value of the angles
	// contents 
	//--------------------------------------------------
	gen_net::CQuaternion GetInterpolated(std::chrono::milliseconds dtms);

	//--------------------------------------------------
	// Standard vector manipulator.
	//--------------------------------------------------
	void Set(float w, float x, float y, float z);

	void Rotate(float w, float x, float y, float z);

protected:

};