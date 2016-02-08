#pragma once
#include "IController.hpp"

namespace NETTIK
{

	class IControllerClient : public IController
	{

	public:

		bool InitializeHost()
		{
			m_pHost = enet_host_create(NULL, 1, 0, 0, 0);

			if (m_pHost == NULL)
				return false;

			return true;
		}

		bool InitializeAddress(const char* hostname, uint16_t port)
		{
			enet_address_set_host(&m_Address, hostname);
			m_Address.port = port;
			return true;
		}

		IControllerClient(uint32_t rate) : IController(rate)
		{
			if (!InitializeHost())
				NETTIK_EXCEPTION("Failed establishing host, network protcol error.");
		}

		bool Connect(const char* hostname, uint16_t port)
		{
			if (!InitializeAddress(hostname, port))
				return false;

			ENetPeer* peer = enet_host_connect(m_pHost, &m_Address, 0, 0);

			if (peer == nullptr)
				NETTIK_EXCEPTION("Client peer failed.");
			
			on_enet(ENET_EVENT_TYPE_DISCONNECT, [this](ENetEvent& evtFrame) {
				this->Stop();
			});

			if (enet_host_service(m_pHost, &m_CurrentEvent, 5000) > 0 &&
				m_CurrentEvent.type == ENET_EVENT_TYPE_CONNECT)
			{
				m_PeerList.push_back(peer);

				// Inform listeners because this service call
				// was done outside of the main procedure.
				FireEvent(ENET_EVENT_TYPE_CONNECT, m_CurrentEvent);
				return true;
			}
			else
			{
				enet_peer_reset(peer);
				return false;
			}

		}

		~IControllerClient()
		{
		}
	};
}
