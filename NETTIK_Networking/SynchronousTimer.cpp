//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include <thread>
#include "SynchronousTimer.h"

SynchronousTimer::SynchronousTimer(uint32_t iRate) : m_iRate(iRate)
{
	m_begin = std::chrono::steady_clock::now();
}

SynchronousTimer::~SynchronousTimer()
{
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto loop_time     = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_begin);
	auto desired_time  = std::chrono::milliseconds(1000 / m_iRate);

	// Delay the thread's execution to resynch from the last frame updates.
	std::this_thread::sleep_for(desired_time - loop_time);
}
