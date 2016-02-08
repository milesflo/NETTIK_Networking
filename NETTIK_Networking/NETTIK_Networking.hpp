#pragma once
#include <enet/enet.h>
#include <memory>

/* Internal Protocol Buffer suite */
#include "NetworkStructures.pb.h"

/* Important includes for constructing full interfaces */
#include "IDebug.hpp"
#include "INetworkCodes.hpp"
#include "INetworkPacketFactory.hpp"

/* Bring in the full interfaces */
#include "IController.hpp"
#include "IControllerClient.hpp"
