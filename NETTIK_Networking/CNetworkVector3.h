//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <cmath>

namespace NETTIK
{
	class CNetworkVector3
	{
	public:
		CNetworkVector3() { x = 0.0f; y = 0.0f; z = 0.0f; }
		CNetworkVector3(const CNetworkVector3 & in) { x = in.x; y = in.y; z = in.z; }
		CNetworkVector3(float inX, float inY, float inZ) { x = inX; y = inY; z = inZ; }
		~CNetworkVector3() { }

		void	Set(float inX, float inY, float inZ) { x = inX; y = inY; z = inZ; }
		void	Get(float * outX, float * outY, float * outZ) { *outX = x; *outY = y; *outZ = z; }

		void	Normalize(void) { float mag = Magnitude(); x /= mag; y /= mag; z /= mag; }
		float	Magnitude(void) { return sqrt(x*x + y*y + z*z); }

		void	Scale(float scale) { x *= scale; y *= scale; z *= scale; }


		CNetworkVector3 &	operator+=(const CNetworkVector3 & rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		CNetworkVector3 &	operator-=(const CNetworkVector3 & rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		CNetworkVector3 &	operator*=(const CNetworkVector3 & rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		CNetworkVector3 &	operator/=(const CNetworkVector3 & rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

		bool operator==(const CNetworkVector3& vector) const;

		union
		{
			struct
			{
				float	x, y, z;
			};
			float	d[3];
		};
	};


	inline CNetworkVector3 operator+(const CNetworkVector3 & lhs, const CNetworkVector3 & rhs)
	{
		return CNetworkVector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	inline CNetworkVector3 operator-(const CNetworkVector3 & lhs, const CNetworkVector3 & rhs)
	{
		return CNetworkVector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	inline CNetworkVector3 operator*(const CNetworkVector3 & lhs, const CNetworkVector3 & rhs)
	{
		return CNetworkVector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	inline CNetworkVector3 operator/(const CNetworkVector3 & lhs, const CNetworkVector3 & rhs)
	{
		return CNetworkVector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

}