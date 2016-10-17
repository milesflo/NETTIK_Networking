#include "LoggingUtil.h"
#include <windows.h>

NETTIK::CNETTIK_Issue::CNETTIK_Issue(const std::string& error, const std::string& file, unsigned int line)
{
	std::stringstream stream;
	stream << file << std::endl << "Line: " << line << ": " << error;

	m_sErrorMessage = stream.str();

	// Try to dispatch exception into the global IController singleton. If this
	// can't be done, pump to cerr stream.
	IController* pNetworkController = IController::GetSingleton();

	if (pNetworkController)
	{
		pNetworkController->GetQueue().Add(kMessageType_Error, m_sErrorMessage);
	}
	else
	{
		std::cerr << m_sErrorMessage << std::endl;
	}

	ToMessage();
}


void NETTIK::CNETTIK_Issue::ToMessage()
{
	MessageBoxA(NULL, m_sErrorMessage.c_str(), "NETTIK Error", MB_OK);
}

const char* NETTIK::CNETTIK_Issue::what() const throw()
{
	return m_sErrorMessage.c_str();
}

NETTIK::CNETTIK_Issue::~CNETTIK_Issue()
{
}