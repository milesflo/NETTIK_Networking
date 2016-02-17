#include "IThread.hpp"
#include <cstdio>

IThread::IThread(ThreadProcedure proc, void* pData)
{
	m_fProcedure = proc;
	m_pPassedData = pData;
}

IThread::~IThread()
{
	if (m_hThread)
	{
		if (m_bRunning)
			TerminateThread(m_hThread, 0);

		CloseHandle(m_hThread);
	}
}

void IThread::Start()
{
	if (m_hThread || m_bRunning)
		return;

	m_bRunning = true;

	m_hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_staticProc,
		static_cast<IThread*>(this), 0, 0);
}

uint32_t IThread::_staticProc(void* pData)
{
	IThread* self;
	self = static_cast<IThread*>(pData);

	if (self->m_fProcedure)
		self->m_fProcedure(
			self->m_pPassedData
		);

	self->m_bRunning = false;

	return 0;

}