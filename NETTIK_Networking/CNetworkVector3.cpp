//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "CNetworkVector3.h"

namespace NETTIK
{

	bool CNetworkVector3::operator==(const CNetworkVector3& vector) const
	{
		if (this == &vector)
			return true;

		if (x == vector.x && y == vector.y && z == vector.z)
			return true;

		return false;
	}

}