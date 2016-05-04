#pragma once
#include <enet/enet.h>
#include <memory>

/* Internal Protocol Buffer suite */
#include <google/protobuf/stubs/common.h>

/* Important includes for constructing full interfaces */
#include "IDebug.h"
#include "INetworkCodes.hpp"
#include "INetworkPacketFactory.hpp"

/* Bring in the full interfaces */
#include "IController.hpp"
#include "IControllerClient.hpp"
#include "IControllerServer.hpp"

/* Some macros in here */
#include "CNetVarBase.h"
#include "CNetVarVector.h"
#include "CNetVarString.h"