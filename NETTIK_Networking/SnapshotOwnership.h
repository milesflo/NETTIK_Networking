#pragma once
#include "SnapshotStream.h"
#include "INetworkCodes.hpp"
#include "IDebug.h"
#include "NETIDReserved.h"
#include <enet\types.h>
#include <cstddef>

struct SnapshotOwnershipData
{
	uint32_t  netid;
	bool      controlled;
};

NETTIK::static_info<sizeof(SnapshotOwnershipData)> info;
