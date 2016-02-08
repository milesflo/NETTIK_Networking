#include "NETTIK_Networking.hpp"
#include <iostream>
using namespace NETTIK;
using namespace std;

enum NetClientCodes
{

};

enum NetServerCodes
{

};

void main()
{

	
	IControllerClient* s_controller;
	s_controller = new IControllerClient(0);
	s_controller->ListenEvent(ENET_EVENT_TYPE_CONNECT, [] (ENetEvent& evtFrame)
	{

		printf("connected!\n");

	});
	s_controller->ListenEvent(ENET_EVENT_TYPE_DISCONNECT, [] (ENetEvent& evtFrame)
	{

		printf("disconnected!\n");

	});
	s_controller->Start();
	s_controller->Connect("127.0.0.1", 1337);

	while (s_controller->IsRunning())
	{

	}

	delete( s_controller );

	cout << "finished";
	cin.get();
}