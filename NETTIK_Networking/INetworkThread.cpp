//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "INetworkThread.h"
#include <cstdio>

INetworkThread::INetworkThread(ThreadProcedure proc, void* pData)
{
	m_fProcedure = proc;
	m_pPassedData = pData;
}

INetworkThread::~INetworkThread()
{
	if (m_hThread)
	{
		if (m_bRunning)
			TerminateThread(m_hThread, 0);
		m_bRunning = false;

//		CloseHandle(m_hThread);
	}
}

void INetworkThread::Start()
{
	if (m_hThread || m_bRunning)
		return;

	m_bRunning = true;

	m_hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_staticProc,
		static_cast<INetworkThread*>(this), 0, 0);
}

uint32_t INetworkThread::_staticProc(void* pData)
{
	INetworkThread* self = static_cast<INetworkThread*>(pData);

	if (self->m_fProcedure)
		self->m_fProcedure(
			self->m_pPassedData, self->m_bRunning
		);

	self->m_bRunning = false;

	return 0;

}