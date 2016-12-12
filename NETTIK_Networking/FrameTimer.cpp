#include "FrameTimer.h"

void CFrameTimer::Start()
{
	if (!m_bRunning)
	{
		m_bRunning = true;

		if (m_bUseHighRes)
		{
			LARGE_INTEGER newHighResTime;
			QueryPerformanceCounter( &newHighResTime );
			m_HighResStart.QuadPart += (newHighResTime.QuadPart - m_HighResStop.QuadPart);
			m_HighResLap.QuadPart += (newHighResTime.QuadPart - m_HighResStop.QuadPart);
		}
		else
		{
			DWORD newLowResTime = timeGetTime();
			m_LowResStart += (newLowResTime - m_LowResStop);
			m_LowResLap += (newLowResTime - m_LowResStop);
		}
	}
}

void CFrameTimer::Stop()
{
	if (m_bUseHighRes)
	{
		QueryPerformanceCounter( &m_HighResStop );
	}
	else
	{
		m_LowResStop = timeGetTime();
	}
}

float CFrameTimer::GetFrequency()
{
	if (m_bUseHighRes)
	{
		return static_cast<float>(m_HighResFreq.QuadPart);
	}
	else
	{
		return 1000.0f;
	}
}

float CFrameTimer::GetTime()
{
	float fTime;

	if (m_bUseHighRes)
	{
		LARGE_INTEGER newHighResTime;
		if (m_bRunning)
		{
			QueryPerformanceCounter( &newHighResTime );
		}
		else
		{
			newHighResTime = m_HighResStop;
		}

		double dTime = static_cast<double>(newHighResTime.QuadPart - m_HighResStart.QuadPart) / GetFrequency();
		fTime = static_cast<float>(dTime);
	}
	else
	{
		DWORD newLowResTime;
		if (m_bRunning)
		{
			newLowResTime = timeGetTime();
		}
		else
		{
			newLowResTime = m_LowResStop;
		}

		fTime = static_cast<float>(newLowResTime - m_LowResStart) / GetFrequency();
	}

	return fTime;
}

float CFrameTimer::GetLapTime()
{
	float fTime;
	
	if (m_bUseHighRes)
	{
		LARGE_INTEGER newHighResTime;
		if (m_bRunning)
		{
			QueryPerformanceCounter( &newHighResTime );
		}
		else
		{
			newHighResTime = m_HighResStop;
		}

		double dTime = static_cast<double>(newHighResTime.QuadPart - m_HighResLap.QuadPart) / GetFrequency();
		fTime = static_cast<float>(dTime);
		m_HighResLap = newHighResTime;
	}
	else
	{
		DWORD newLowResTime;
		if (m_bRunning)
		{
			newLowResTime = timeGetTime();
		}
		else
		{
			newLowResTime = m_LowResStop;
		}
		fTime = static_cast<float>(newLowResTime - m_LowResLap) / GetFrequency();
	}

	return fTime;
}

void CFrameTimer::Reset()
{
	if (m_bUseHighRes)
	{
		QueryPerformanceCounter( &m_HighResStart );
		m_HighResLap = m_HighResStart;
		m_HighResStop = m_HighResStart;
	}
	else
	{
		m_LowResStart = timeGetTime();
		m_LowResLap = m_LowResStart;
		m_LowResStop = m_LowResStart;
	}
}

CFrameTimer::CFrameTimer()
{
	m_bUseHighRes = QueryPerformanceFrequency( &m_HighResFreq ) != 0;

	Reset();
	m_bRunning = true;
}