#pragma once
#include "NETTIK_Networking.hpp"
#include "ITestHarness.h"
#include "tests_shared.h"

//NETTIK::IController* controller = nullptr;

class Server : public NETTIK::IControllerServer
{
public:
	Server() : NETTIK::IControllerServer(0)
	{
		
	}

	void PostUpdate()
	{

	}
};

class Tests_Server : public TestHarness<NETTIK::IControllerServer>
{
private:

public:

	Tests_Server()
	{
		SetObjectPointer<NETTIK::IControllerServer>(new Server());
	}

	void Run()
	{
		NETTIK::IControllerServer* server;
		server = GetObjectPointer<NETTIK::IControllerServer>();

		if (!server)
			return Failed("m_pController nullptr");

		if (!server->Listen(1337, 32))
			return Failed("Binding to hostname or port failed.");

		server->on_enet(ENET_EVENT_TYPE_CONNECT, [](ENetEvent& frame) {
			std::cout << "client connected!" << std::endl;
		});

		CEntity* test = new CEntity;

		server->CreateInstance("game")->CreateEntityManager<CEntity>("ents")->SERVER_Add(test);
		server->Start();

		while (server->IsRunning())
		{
			server->Update();
		}
		
		Success();
	}

	~Tests_Server()
	{
		printf("dELETE.\n");
		delete(m_Object);
	}
};