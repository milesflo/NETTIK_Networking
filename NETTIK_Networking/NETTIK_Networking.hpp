#pragma once
#include <memory>

/* Internal Protocol Buffer suite */
#include "NetworkStructures.pb.h"

/* Important includes for constructing full interfaces */
#include "IDebug.hpp"
#include "INetworkCodes.hpp"
#include "INetworkPacketFactory.hpp"

/* Bring in the full interfaces */
#include "IController.hpp"

namespace NETTIK
{
	typedef std::unique_ptr<IControllerClient> IControllerClient_t;
	typedef std::unique_ptr<IControllerServer> IControllerServer_t;
}