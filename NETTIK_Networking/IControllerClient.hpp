#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerClient : public IController
	{

	public:
		DEFINE_SERVER(true);

		IControllerClient(uint32_t rate);
		virtual ~IControllerClient();

		//! Creatse the client connection framework.
		bool InitializeHost();
		bool InitializeAddress(const char* hostname, uint16_t port);

		//! Connects to a specified hostname and port.
		bool Connect(const char* hostname, uint16_t port);

	};
}
