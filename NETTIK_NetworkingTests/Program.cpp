#include "NETTIK_Networking.hpp"
#include "CPlayerRealm.h"
#include <iostream>
#include <windows.h>
using namespace NETTIK;
using namespace std;

IController* service;

const uint32_t serviceRate = 60;

BOOL WINAPI CtrlHandler(DWORD type)
{
	switch (type)
	{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_BREAK_EVENT:
			service->Stop();
	}

	return TRUE;

}

class CPlayer : public NetObject
{
public:
	DEFINE_NETOBJECT("Player");

	DEFINE_NetVar(char, m_Char, true);
	DEFINE_NetVar(uint32_t, m_Int, false);

	DEFINE_NetVector(m_Pos, false);

	void Update()
	{
		m_Pos.Set(1, 2, 3);
		// Debugging 
		if (m_NetCode == 0 && IsServer())
			m_Int.Set(static_cast<uint32_t>(rand() % UINT32_MAX));

//		if (!IsServer())
//			printf("client value %s %d (%d)\n", m_Int.GetName(), m_Int.get(), m_NetCode);

	}

	CPlayer() : NetObject()
	{
		if (IsServer())
		{
			m_Char.Set(0xff);
			m_Int.Set(123);
		}
	}

	virtual ~CPlayer()
	{

	}
};

class Client : public IControllerClient
{
public:
	void PostUpdate()
	{
		 
	}

	Client() : IControllerClient(0)
	{

	}

	virtual ~Client()
	{

	}
};

class Server : public IControllerServer
{
private:
	CPlayerRealm<CPlayer> m_Realm;
public:
	void PostUpdate()
	{
		m_Realm.ProcessRealmDeletes();
	}

	Server() : IControllerServer(0)
	{
		on_enet(ENET_EVENT_TYPE_DISCONNECT, [this](ENetEvent& frame)
		{
			printf("Client disconnected.\n");
			CPlayer* player;
			player = m_Realm.GetPlayer(frame.peer);
			if (player)
			{
				GetInstance("world")->GetEntityManager<CPlayer>("players")->Remove(player->m_NetCode);
				m_Realm.Remove(player->m_RealmID);
				
				printf("Realm player left.\n");
			}
			else
				printf("Player object missing.\n");
		});

		on_enet(ENET_EVENT_TYPE_CONNECT, [this](ENetEvent& frame)
		{
			printf("Client connected.\n");

			CPlayer* player;
			player = m_Realm.Add(frame.peer);

			GetInstance("world")->GetEntityManager<CPlayer>("players")->Add(player);
			printf("Realm player added.\n");
		});
	}
	virtual ~Server()
	{

	}
};

void OnNewPlayer(CPlayer* player)
{
	printf("create.\n");
}

void OnDeletePlayer(CPlayer* player)
{
	printf("delete.\n");
}

void SetupSharedResources(IController* controller)
{
	VirtualInstance* instance = controller->CreateInstance("world");
	instance->CreateEntityManager<CPlayer>("players", OnNewPlayer, OnDeletePlayer);
}

bool StartClient()
{
	Client* client = new Client();
	service = client;
	SetupSharedResources(client);

	cout << "Attempting to bind address..." << endl;
	if (!client->Connect("127.0.0.1", 1337))
		return false;

	cout << "Starting thread..." << endl;
	client->Start();

	cout << "Running process..." << endl;

	while (client->IsRunning())
	{
		IController::Timer timer(serviceRate);
		client->Update();
	}

	client->Destroy();
	return true;
}

bool StartServer()
{
	Server* server = new Server();
	service = server;
	SetupSharedResources(server);

	cout << "Attempting to bind address..." << endl;
	if (!server->Listen(1337, 32))
		return false;

	cout << "Starting thread..." << endl;
	server->Start();

	cout << "Running process..." << endl;
	while (server->IsRunning())
	{
		IController::Timer timer(serviceRate);
		server->Update();
	}
	server->Destroy();
	return true;
}

int main(int argc, char* argv[])
{
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		cout << "Console handler failed.";
		return 1;
	}

	char mode = 'c';
	if (argc > 1)
		mode = argv[1][0];

	bool result;

	switch (mode)
	{
	case 's':
		cout << "Running server..." << endl;
		result = StartServer();
		assert(result && "Server failed.");
		break;
	case 'c':
		cout << "Running client..." << endl;
		result = StartClient();
		assert(result && "Client failed.");
		break;
	default:
		cout << "Unknown network mode." << endl;
		return 0;
	}

	cout << "Application finished." << endl;
	return 0;
}