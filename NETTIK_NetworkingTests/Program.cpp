#include "NETTIK_Networking.hpp"
#include "tests_server.h"
#include "tests_client.h"
#include <iostream>
#include <windows.h>
using namespace NETTIK;
using namespace std;

ITestHarness* g_Test = nullptr;

//
//IControllerClient* TestClient()
//{
//
//	IControllerClient* s_controller;
//	s_controller = new IControllerClient(0);
//
//	s_controller->on_enet(ENET_EVENT_TYPE_CONNECT, [] (ENetEvent& evtFrame) {
//		printf("client: connected!\n");
//	});
//	s_controller->on_enet(ENET_EVENT_TYPE_DISCONNECT, [] (ENetEvent& evtFrame) {
//		printf("client: disconnected!\n");
//	});
//
//	if (s_controller->Connect("127.0.0.1", 1337))
//		s_controller->Start();
//
//	return s_controller;
//}
//
//IControllerServer* TestServer()
//{
//
//	IControllerServer* s_controller;
//	s_controller = new IControllerServer(0);
//	s_controller->on_enet(ENET_EVENT_TYPE_CONNECT, [] (ENetEvent& evtFrame) {
//		printf("server: client connected.\n");
//	});
//	
//	s_controller->Listen(1337, 32);
//	s_controller->Start();
//
//	return s_controller;
//}

BOOL WINAPI CtrlHandler(DWORD type)
{
	switch (type)
	{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_BREAK_EVENT:
		{
			if (g_Test != nullptr)
			{
				delete(g_Test);
				g_Test = nullptr;
			}
			return (TRUE);
		}
		default:
			return FALSE;
	}

}

int main(int argc, char* argv[])
{
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		cout << "Console handler failed.";
		return 1;
	}

	if (argc <= 1)
	{
		cout << "Enter [c]lient or [s]erver into command line" << endl;
		return 0;
	}

	const char mode = argv[1][0];
	switch (mode)
	{
	case 's':
	{
		g_Test = new Tests_Server();
		g_Test->Run();
		break;
	}
	case 'c':
		g_Test = new Tests_Client();
		g_Test->Run();
		break;
	default:
		cout << "Unknown network mode." << endl;
		return 0;
	}

	return 0;
}