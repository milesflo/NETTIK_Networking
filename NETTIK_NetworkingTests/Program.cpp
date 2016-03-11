#include "NETTIK_Networking.hpp"
#include "CPlayerRealm.h"
#include <iostream>
#include <windows.h>
using namespace NETTIK;
using namespace std;

IController* service;

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

	CNetVar(int, m_Health, true);
	CNetVarVector3(m_Pos, false);

	char _debug_memory[1024 * 56];

	void Update()
	{

	}

	CPlayer() : NetObject()
	{
		m_Health.Set(100);
		m_Pos.Set(0.0f, 0.0f, 0.0f);
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
		VirtualInstance* instance = CreateInstance("world");
		instance->CreateEntityManager<CPlayer>("players");

		on_enet(ENET_EVENT_TYPE_DISCONNECT, [this, instance](ENetEvent& frame)
		{
			printf("Client disconnected.\n");
			CPlayer* player;
			player = m_Realm.GetPlayer(frame.peer);
			if (player)
			{
				instance->GetEntityManager<CPlayer>("players")->SERVER_Remove(player->m_NetCode);
				m_Realm.Remove(player->m_RealmID);
				
				printf("Realm player left.\n");
			}
			else
				printf("Player object missing.\n");
		});

		on_enet(ENET_EVENT_TYPE_CONNECT, [this, instance](ENetEvent& frame)
		{
			printf("Client connected.\n");

			CPlayer* player;
			player = m_Realm.Add(frame.peer);

			instance->GetEntityManager<CPlayer>("players")->SERVER_Add(player);
			printf("Realm player added.");
		});
	}
	virtual ~Server()
	{

	}
};

bool StartClient()
{
	Client* client = new Client();
	service = client;
	if (!client->Connect("127.0.0.1", 1337))
		return false;

	client->Start();

	while (client->IsRunning())
	{
		client->Update();
	}

	delete(client);
	return true;
}

bool StartServer()
{
	Server* server = new Server();
	service = server;

	if (!server->Listen(1337, 32))
		return false;

	server->Start();

	while (server->IsRunning())
	{
		server->Update();
	}

	delete(server);
	return true;
}

int main(int argc, char* argv[])
{
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		cout << "Console handler failed.";
		return 1;
	}

	char mode = 's';
	if (argc > 1)
		mode = argv[1][0];

	switch (mode)
	{
	case 's':
		assert(StartServer() && "Server failed.");
		break;
	case 'c':
		assert(StartClient() && "Client failed.");
		break;
	default:
		cout << "Unknown network mode." << endl;
		return 0;
	}

	return 0;
}