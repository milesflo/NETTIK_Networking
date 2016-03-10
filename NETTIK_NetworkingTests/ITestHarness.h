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
private:
	bool m_bResultLocked = false;

public:
	ITestResult result;

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

	//! Returns test results.
	virtual void Run() = 0;

	virtual ~ITestHarness()
	{

	}
};