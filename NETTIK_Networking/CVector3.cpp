//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "CVector3.h"

/*-----------------------------------------------------------------------------------------
	Length operations
-----------------------------------------------------------------------------------------*/

// Reduce vector to unit length - member function
void gen_net::CVector3::Normalise()
{
	float lengthSq = x*x + y*y + z*z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( lengthSq == 0.0f )
	{
		x = y = z = 0.0f;
	}
	else
	{
		float invLength = 1.0f / sqrtf( lengthSq );
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}
}


// Return unit length vector in the same direction as given one
gen_net::CVector3 Normalise( const gen_net::CVector3& v )
{
	float lengthSq = v.x*v.x + v.y*v.y + v.z*v.z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if ( lengthSq == 0.0f )
	{
		return gen_net::CVector3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		float invLength = 1.0f / sqrtf( lengthSq );
		return gen_net::CVector3(v.x * invLength, v.y * invLength, v.z * invLength);
	}
}


/*-----------------------------------------------------------------------------------------
	Point related functions
-----------------------------------------------------------------------------------------*/

// Return distance from this point to another - member function
float gen_net::CVector3::DistanceTo( const gen_net::CVector3& p )
{
	float distX = p.x - x;
	float distY = p.y - y;
	float distZ = p.z - z;
	return sqrtf( distX*distX + distY*distY + distZ*distZ );
}


// Return squared distance from this point to another - member function
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceToSquared(...) ) to calculate 1 / distance more efficiently
float gen_net::CVector3::DistanceToSquared( const gen_net::CVector3& p )
{
	float distX = p.x - x;
	float distY = p.y - y;
	float distZ = p.z - z;
	return distX*distX + distY*distY + distZ*distZ;
}


// Return distance from one point to another - non-member version
float Distance
(
	const gen_net::CVector3& p1,
	const gen_net::CVector3& p2
)
{
	float distX = p1.x - p2.x;
	float distY = p1.y - p2.y;
	float distZ = p1.z - p2.z;
	return sqrtf( distX*distX + distY*distY + distZ*distZ );
}

// Return squared distance from one point to another - non-member version
// More efficient than Distance when exact length is not required (e.g. for comparisons)
// Use InvSqrt( DistanceSquared(...) ) to calculate 1 / distance more efficiently
float DistanceSquared
(
	const gen_net::CVector3& p1,
	const gen_net::CVector3& p2
)
{
	float distX = p1.x - p2.x;
	float distY = p1.y - p2.y;
	float distZ = p1.z - p2.z;
	return distX*distX + distY*distY + distZ*distZ;
}