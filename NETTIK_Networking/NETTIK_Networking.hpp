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

/* Bring in the full interfaces */
#include "ControllerFlags.h"
#include "IController.hpp"
#include "IControllerClient.hpp"
#include "IControllerServer.hpp"

/* Some macros in here */
#include "NetVar.h"
#include "NetObject.h"

#include "CNetVarBase.h"
#include "CNetVarVector.h"
#include "CNetVarString.h"
#include "CNetVarList.h"

#include "SnapshotHeader.h"
#include "SnapshotEntList.h"
#include "SnapshotStream.h"
