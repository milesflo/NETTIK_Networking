#pragma once
#include <wtypes.h>
#include <inttypes.h>

class IThread
{

public:

	typedef void(*ThreadProcedure)(void* pData);

protected:

	void*  m_pPassedData = nullptr;
	bool   m_bRunning = false;
	HANDLE m_hThread = 0;
	ThreadProcedure m_fProcedure;
	
	static uint32_t WINAPI _staticProc(
		void* pData
	);

public:

	IThread(ThreadProcedure proc, void* pData);

	void Start();

	~IThread();
};