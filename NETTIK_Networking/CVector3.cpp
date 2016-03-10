#include "CVector3.h"

namespace NETTIK
{

	bool CVector3::operator==(const CVector3& vector) const
	{
		if (this == &vector)
			return true;

		if (x == vector.x && y == vector.y && z == vector.z)
			return true;

		return false;
	}

}