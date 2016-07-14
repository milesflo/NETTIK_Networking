//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <chrono>

//------------------------------------------------
// A timer class that on destruction calculates
// how much of the frametime to delay by.
// It takes real frame time, and takes it away
// from the desired amount of time to wait.
//
// It gives a steady flow of execution.
//------------------------------------------------
class SynchronousTimer
{
private:
	uint32_t m_iRate;
	std::chrono::steady_clock::time_point m_begin;

public:
	// In milliseconds, how much time a normal frame should be.
	SynchronousTimer(uint32_t iRate);

	~SynchronousTimer();
};
