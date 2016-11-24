//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <wtypes.h>

class IThread
{
public:
	typedef void(*ThreadProcedure)(void* pData, bool& bThreadStatus);

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