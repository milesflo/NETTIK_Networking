#include "NETTIK_Networking.hpp"
#include "ProtoData.pb.h"
#include <iostream>
using namespace NETTIK;
using namespace std;

IControllerClient* TestClient()
{

	IControllerClient* s_controller;
	s_controller = new IControllerClient(0);

	s_controller->on_enet(ENET_EVENT_TYPE_CONNECT, [] (ENetEvent& evtFrame) {
		printf("client: connected!\n");
	});
	s_controller->on_enet(ENET_EVENT_TYPE_DISCONNECT, [] (ENetEvent& evtFrame) {
		printf("client: disconnected!\n");
	});

	if (s_controller->Connect("127.0.0.1", 1337))
		s_controller->Start();

	return s_controller;
}

IControllerServer* TestServer()
{

	IControllerServer* s_controller;
	s_controller = new IControllerServer(0);
	s_controller->on_enet(ENET_EVENT_TYPE_CONNECT, [] (ENetEvent& evtFrame) {
		printf("server: client connected.\n");
	});
	
	s_controller->Listen(1337, 32);
	s_controller->Start();

	return s_controller;
}

void main()
{
	char mode;
	mode = cin.get();
	
	IController* controller;
	switch (mode)
	{
	case 's':
	{
		controller = TestServer();
		break;
	}
	case 'c':
	{
		controller = TestClient();
		break;
	}
	default:
		printf("unknown network type\n");
		return;
	}

	
	while (controller->IsRunning())
	{
		// duhh
	}

	delete(controller);

	cout << "finished";
	cin.get();
}