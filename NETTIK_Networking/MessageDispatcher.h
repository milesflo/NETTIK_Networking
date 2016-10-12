#pragma once
#include <deque>
#include <mutex>

enum EDispatcherMessageType
{
	kMessageType_Print,
	kMessageType_Warn,
	kMessageType_Error
};

class CMessageDispatcher
{
public:

	struct Message
	{
		EDispatcherMessageType type;
		std::string  data;
	};

	void Add(EDispatcherMessageType type, std::string msg);

	Message* GetMessage();

	void PopMessageQueue();

protected:
	std::recursive_mutex  m_MessagesMutex;
	std::deque<Message>  m_Messages;
};