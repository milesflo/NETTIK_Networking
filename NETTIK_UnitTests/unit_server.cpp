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
