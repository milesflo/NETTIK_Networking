//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "CNetVarBase.h"
#include "CVector3.h"
#include <chrono>

//--------------------------------------------------
// Constructs a buffered vector for interpolation.
//--------------------------------------------------
#define NetworkBufferedVector(name, reliable) \
	CNetVarBufferedVector   name = CNetVarBufferedVector( this, #name, reliable )

class CNetVarBufferedVector : public CNetVarBase< CVector3 >
{
public:
	CNetVarBufferedVector(NetObject* parent, const char* name, bool reliable);

	//--------------------------------------------------
	// Overloads the binary set to buffer invalidation to allow
	// interpolation between each snapshot.
	//--------------------------------------------------
	void Set(unsigned char* ptr, size_t size, ENetPeer* pWho);

	//--------------------------------------------------
	// Returns 1 always. Overrides the GetChanges 
	// call in net variables. Buffered vectors always
	// have changes to process.
	//--------------------------------------------------
	size_t GetChanges();

	//--------------------------------------------------
	// Returns the true most updated vector received
	// from the server.
	//--------------------------------------------------
	CVector3 GetLastVector();

	//--------------------------------------------------
	// Calculates the interpolated value of the vectors
	// contents 
	//--------------------------------------------------
	CVector3 GetInterpolated(std::chrono::milliseconds dtms);

	//--------------------------------------------------
	// Standard vector manipulator.
	//--------------------------------------------------
	void Set(float x, float y, float z);

protected:

	//--------------------------------------------------
	// Internal helper function for returning the 
	// current system time.
	//--------------------------------------------------
	std::chrono::milliseconds GetTime();

	//--------------------------------------------------
	// Internal helper function for wrapping a value
	// and supporting negative wrapping.
	//--------------------------------------------------
	template <class T>
	T mod_wrap(T a, T b)
	{
		return (a % b + b) % b;
	}

	// Maximum amount of distance to interpolate to, anything
	// higher will be skipped.
	static const float  kMaxInterpolationDistance;

	std::chrono::milliseconds m_fElaspedTime;       // Elasped time to move interpolated value by.
	std::chrono::milliseconds m_fTimeFrame;         // The average time in milliseconds the vector expects an update to its contents (total frametime)
	std::chrono::milliseconds m_fTimeFramePrevious; // Timestamp of previous update function.
	
	// Interpolation buffers.
	CVector3 m_VectorBuffer[4];
	size_t   m_ActivePositionIndex = 0;

	bool m_bCalculatedFrameTime = false;
	bool m_bCalculatedPreviousFrameTime = false;
};
