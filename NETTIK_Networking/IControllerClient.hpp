#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerClient : public IController
	{

	public:

		virtual void InitializeHost()
		{
			m_pHost = enet_host_create(NULL, 1, 0, 0, 0);

			if (m_pHost == NULL)
				NETTIK_EXCEPTION("Failed creating client host.");

		}


		IControllerClient() : IController()
		{

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
