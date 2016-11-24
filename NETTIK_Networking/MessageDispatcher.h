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


	//--------------------------------------------------------
	// Adds a new message to the message queue.
	//--------------------------------------------------------
	void Add(EDispatcherMessageType type, std::string msg);
	static void Add(EDispatcherMessageType type, const char* psFormat, ...);

	//--------------------------------------------------------
	// Gets the longest waiting message in the queue.
	//--------------------------------------------------------
	Message* GetMessage();

	//--------------------------------------------------------
	// Pops the front of the queue.
	//--------------------------------------------------------
	void PopMessageQueue();

protected:
	std::recursive_mutex  m_MessagesMutex;
	std::deque<Message>  m_Messages;
};