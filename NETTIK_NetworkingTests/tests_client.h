#pragma once
#include "NETTIK_Networking.hpp"
#include "ITestHarness.h"
#include "tests_shared.h"

//NETTIK::IController* controller = nullptr;

class Client : public NETTIK::IControllerClient
{
public:
	Client() : NETTIK::IControllerClient(0)
	{

	}

	void PostUpdate()
	{

	}
};

class Tests_Client : public TestHarness<NETTIK::IControllerServer>
{
private:

public:

	Tests_Client()
	{
		SetObjectPointer<NETTIK::IControllerClient>(new Client());
	}

	void Run()
	{
		NETTIK::IControllerClient* client;
		client = GetObjectPointer<NETTIK::IControllerClient>();

		if (!client)
			return Failed("m_pController nullptr");
		
		if (!client->Connect("127.0.0.1", 1337))
			return Failed("Binding to hostname or port failed.");

		client->on_enet(ENET_EVENT_TYPE_CONNECT, [](ENetEvent& frame) {
			std::cout << "connected!" << std::endl;
		});

		client->Start();

		while (client->IsRunning())
		{
			client->Update();
		}

		Success();
	}

	~Tests_Client()
	{
		delete(m_Object);
	}
};