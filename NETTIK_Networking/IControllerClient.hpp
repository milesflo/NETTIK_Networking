#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerClient : public IController
	{

	public:

		IControllerClient(uint32_t rate);
		~IControllerClient();

		//! Dummy function for post updating.
//		virtual void PostUpdate();

		//! Creatse the client connection framework.
		bool InitializeHost();
		bool InitializeAddress(const char* hostname, uint16_t port);

		//! Connects to a specified hostname and port.
		bool Connect(const char* hostname, uint16_t port);

	};
}
