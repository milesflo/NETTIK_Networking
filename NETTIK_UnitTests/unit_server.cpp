#include <gtest\gtest.h>
#include "NETTIK_Networking.hpp"

class CServer : public NETTIK::IControllerServer
{
public:
	void PostUpdate(float elapsedTime);

	void ControllerUpdate(float elapsedTime);

	CServer();
	virtual ~CServer();
};

CServer::CServer() : NETTIK::IControllerServer(33)
{

}

CServer::~CServer()
{

}

void CServer::PostUpdate(float elapsedTime)
{

}

void CServer::ControllerUpdate(float elaspedTime)
{

}

TEST(ControllerServer, Initialization)
{
	
	std::unique_ptr<CServer> server;
	server.reset(new CServer());

	ASSERT_NE(server.get(), nullptr);
	ASSERT_NE(server->Listen(1337, 32), false);

	server->Start();

	ASSERT_NE(server->IsRunning(), false);

	server->Destroy();

}