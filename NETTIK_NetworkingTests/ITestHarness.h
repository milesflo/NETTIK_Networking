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

protected:
	void* m_Object;

public:
	ITestResult result;

	virtual void Run() = 0;

	template <typename _PtrInstance>
	inline _PtrInstance* GetObjectPointer() { return static_cast<_PtrInstance*>(m_Object); }

	template <typename _PtrInstance>
	inline void SetObjectPointer(_PtrInstance* object) { m_Object = static_cast<void*>(object); }

	virtual ~ITestHarness() { }
};

template <typename _PtrInstance>
class TestHarness : public ITestHarness
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

	virtual ~TestHarness()
	{
	}
};