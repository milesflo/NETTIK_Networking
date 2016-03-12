#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerServer : public IController
	{
	public:
		DEFINE_SERVER(true);

		size_t m_iPeerLimit;

	public:

		//! Update routines.
		void ControllerUpdate();

		IControllerServer();
		IControllerServer(uint32_t rate);
		virtual ~IControllerServer();

		bool InitializeHost();
		bool InitializeAddress(const char* hostname, uint16_t port);

		bool Listen(uint16_t port, size_t peerLimit);

	};
}
