//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <string>

struct ITestResult
{
	std::string what;
	bool        success;

	ITestResult() { }
	ITestResult(bool success) : success(success) { }
	ITestResult(bool success, std::string what) : success(success), what(what) { }
	ITestResult(std::string what) : success(false), what(what) { }
};

class ITestHarness
{
protected:
	bool m_bResultLocked = false;

public:
	ITestResult result;
	virtual void Test() = 0;
	virtual void StopTest() = 0;
	virtual ~ITestHarness() { }
};

template <typename T>
class TestHarness : public ITestHarness, public T
{
public:

	void Failed(std::string what)
	{
		if (m_bResultLocked)
			return;

		result.success = false;
		result.what = what;
		
		m_bResultLocked = true;
	}

	void Success()
	{
		if (m_bResultLocked)
			return;

		result.success = true;
		result.what = "ok (test success)";

		m_bResultLocked = true;
	}

	TestHarness() : ITestHarness(), T()
	{

	}

	virtual ~TestHarness()
	{
		printf("~harness\n");
	}
};