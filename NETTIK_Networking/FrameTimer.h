#pragma once
#include <Windows.h>
//--------------------------------------------------------------------
// CFrameTimer: Does a precise time diff between the current loop and 
// the previous loop to determine the current frame time.
//
// Taken from samples of CTimer.h by Laurent Noel, modified to
// allow frequency to take from member function internally.
//--------------------------------------------------------------------
class CFrameTimer
{
public:

	CFrameTimer();

	float GetLapTime();

	float GetTime();

	float GetFrequency();

	void Start();

	void Stop();

	void Reset();

private:

	bool m_bUseHighRes;
	bool m_bRunning;

	//----------------------------------
	// High resolution clock
	//----------------------------------
	LARGE_INTEGER m_HighResFreq;
	LARGE_INTEGER m_HighResStart;
	LARGE_INTEGER m_HighResLap;
	LARGE_INTEGER m_HighResStop;
	
	//----------------------------------
	// Low resolution clock
	//----------------------------------
	DWORD m_LowResStart;
	DWORD m_LowResLap;
	DWORD m_LowResStop;
};