#pragma once
#include <cmath>

namespace NETTIK
{
	class CVector3
	{
	public:
		CVector3() { x = 0.0f; y = 0.0f; z = 0.0f; }
		CVector3(const CVector3 & in) { x = in.x; y = in.y; z = in.z; }
		CVector3(float inX, float inY, float inZ) { x = inX; y = inY; z = inZ; }
		~CVector3() { }

		void	Set(float inX, float inY, float inZ) { x = inX; y = inY; z = inZ; }
		void	Get(float * outX, float * outY, float * outZ) { *outX = x; *outY = y; *outZ = z; }

		void	Normalize(void) { float mag = Magnitude(); x /= mag; y /= mag; z /= mag; }
		float	Magnitude(void) { return sqrt(x*x + y*y + z*z); }

		void	Scale(float scale) { x *= scale; y *= scale; z *= scale; }


		CVector3 &	operator+=(const CVector3 & rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		CVector3 &	operator-=(const CVector3 & rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		CVector3 &	operator*=(const CVector3 & rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		CVector3 &	operator/=(const CVector3 & rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

		bool operator==(const CVector3& vector) const;

		union
		{
			struct
			{
				float	x, y, z;
			};
			float	d[3];
		};
	};


	inline CVector3 operator+(const CVector3 & lhs, const CVector3 & rhs)
	{
		return CVector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	inline CVector3 operator-(const CVector3 & lhs, const CVector3 & rhs)
	{
		return CVector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	inline CVector3 operator*(const CVector3 & lhs, const CVector3 & rhs)
	{
		return CVector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	inline CVector3 operator/(const CVector3 & lhs, const CVector3 & rhs)
	{
		return CVector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

}