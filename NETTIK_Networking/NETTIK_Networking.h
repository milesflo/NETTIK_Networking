//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <enet\types.h>
#include <enet\enet.h>

/* Internal Protocol Buffer suite */
#include <google\protobuf\message.h>
#include <google\protobuf\stubs\common.h>

/* Important includes for constructing full interfaces */
#include "LoggingUtil.h"
#include "INetworkCodes.hpp"
#include "INetworkPacketFactory.hpp"
#include "IThread.h"
#include "SynchronousTimer.h"

// Network interfaces.
#include "ControllerFlags.h"
#include "NetSystem.h"
#include "NetSystemServer.h"
#include "NetSystemClient.h"

#include "IEntityManager.h"
#include "EntityMessages.pb.h"
#include "VirtualInstance.h"
#include "CEntities.h"

// Networking components
#include "NetVar.h"
#include "NetObject.h"

#include "CNetVarBase.h"
#include "CNetVarVector.h"
#include "CNetVarString.h"

#include "NetVarListBase.h"
#include "CNetVarList.h"

#include "SnapshotHeader.h"
#include "SnapshotEntList.h"
#include "SnapshotStream.h"
#include "NETRPC.h"

#include "LockableVector.h"

// Network entity specific objects
#include "Constraints.h"
#include "NETIDReserved.h"
#include "ReplicationInfo.h"
#include "ControllerFlags.h"