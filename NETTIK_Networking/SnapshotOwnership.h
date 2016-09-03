//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "SnapshotStream.h"
#include "INetworkCodes.hpp"
#include "NETIDReserved.h"
#include <cstddef>

struct SnapshotOwnershipData
{
	uint32_t  netid;
	bool      controlled;
};

