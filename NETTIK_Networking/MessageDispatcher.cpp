#include "MessageDispatcher.h"


void CMessageDispatcher::Add(EDispatcherMessageType type, std::string msg)
{
	std::lock_guard<std::recursive_mutex> guard( m_MessagesMutex );
	m_Messages.push_back({ type, msg });
}

CMessageDispatcher::Message* CMessageDispatcher::GetMessage()
{
	std::lock_guard<std::recursive_mutex> guard( m_MessagesMutex );
	
	if ( m_Messages.empty() )
		return nullptr;

	return &(m_Messages.front());
}

void CMessageDispatcher::PopMessageQueue()
{
	std::lock_guard<std::recursive_mutex> guard( m_MessagesMutex );
	m_Messages.pop_front();
}