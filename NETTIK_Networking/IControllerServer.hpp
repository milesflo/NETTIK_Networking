#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerServer : public IController
	{

	private:

		size_t m_iPeerLimit;

	public:

		

		bool InitializeHost()
		{
			m_pHost = enet_host_create(&m_Address, m_iPeerLimit, 0, 0, 0);

			if (m_pHost == NULL)
				return false;

			return true;
		}

		bool InitializeAddress(const char* hostname, uint16_t port)
		{
			m_Address.host = ENET_HOST_ANY;
			m_Address.port = port;
			return true;
		}

		IControllerServer(uint32_t rate) : IController(rate)
		{
			m_bReplicating = false;
		}

		bool Listen(uint16_t port, size_t peerLimit)
		{
			m_iPeerLimit = peerLimit;

			if (!InitializeAddress(0, port) || !InitializeHost())
				return false;

			return true;
		}

		~IControllerServer()
		{
		}

	};
}
