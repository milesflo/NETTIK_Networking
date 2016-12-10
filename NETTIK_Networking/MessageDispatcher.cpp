#include "MessageDispatcher.h"
#include "NetSystem.h"
#include "FormatInline.h"

void CMessageDispatcher::Add(EDispatcherMessageType type, std::string msg)
{
	std::lock_guard<std::recursive_mutex> guard(m_MessagesMutex);
	m_Messages.push_back({ type, msg });
}

void CMessageDispatcher::Add(EDispatcherMessageType type, const char* psFormat, ...)
{
	std::string msg;
	FormatAString(msg, psFormat, psFormat);

	NetSystem* pSystem = NetSystem::GetSingleton();
	if (pSystem)
	{
		pSystem->GetQueue().Add(type, msg);
	}
	else
	{
		std::cout << "[NETF] " << msg << std::endl;
	}
}

CMessageDispatcher::Message* CMessageDispatcher::GetNextMessage()
{
	std::lock_guard<std::recursive_mutex> guard(m_MessagesMutex);

	if (m_Messages.empty())
		return nullptr;

	return &(m_Messages.front());
}

void CMessageDispatcher::PopMessageQueue()
{
	std::lock_guard<std::recursive_mutex> guard(m_MessagesMutex);
	m_Messages.pop_front();
}