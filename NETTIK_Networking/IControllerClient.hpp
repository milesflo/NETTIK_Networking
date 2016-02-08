#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerClient : public IController
	{

	public:

		void InitializeHost()
		{
			m_pHost = enet_host_create(NULL, 1, 0, 0, 0);

			if (m_pHost == NULL)
				NETTIK_EXCEPTION("Failed creating client host.");

		}


		IControllerClient(uint32_t rate) : IController(rate)
		{
			InitializeAddress();
			InitializeHost();
		}

		void Connect(const char* hostname, uint16_t port)
		{
			enet_address_set_host(&m_Address, hostname);
			m_Address.port = port;

			enet_host_connect(m_pHost, &m_Address, 0, 0);
			printf("connecting...\n");
		}

		virtual ~IControllerClient()
		{

		}

		void Send(std::string& data, void* peer)
		{

		}

		void Recv(std::string& data, void* peer)
		{

		}

	};
}
