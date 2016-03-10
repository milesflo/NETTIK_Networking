#pragma once
#include "NETTIK_Networking.hpp"
#include "ITestHarness.h"

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

class Tests_Server : public ITestHarness
{
private:
	NETTIK::IControllerServer* m_pController = nullptr;

public:

	Tests_Server()
	{
		m_pController = new Server();
	}

	void Run()
	{
		if (!m_pController)
			return Failed("m_pController nullptr");

		if (!m_pController->Listen(1337, 32))
			return Failed("Binding to hostname or port failed.");
		
		m_pController->Start();

		while (m_pController->IsRunning())
		{
			// filler
		}

		Success();
	}

	~Tests_Server()
	{
		delete(m_pController);
	}
};