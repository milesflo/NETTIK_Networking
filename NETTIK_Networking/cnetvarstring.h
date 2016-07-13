#pragma once
#include <string>
#include "CNetVarBase.h"
#include "CNetworkVector3.h"
#include "IDebug.h"
#define DEFINE_NetString(name, size) \
	CNetVarString<char, std::string, size> name = CNetVarString<char, std::string, size>(this, #name);

#define DEFINE_NetStringWide(name, size) \
	CNetVarString<wchar_t, std::wstring, size> name = CNetVarString<wchar_t, std::wstring, size>(this, #name);

//! Represents an array with restricted data to represent a string.
template <typename T, typename S, size_t N>
class CNetVarString : public CNetVarBase<T[N]>
{
public:

	CNetVarString(NetObject* parent, const char* name) : CNetVarBase(parent, name, true)
	{

		for (size_t i = 0; i < N; i++)
		{
			m_Data[i] = 0;
		}
	}

	void Set(S& data)
	{
		if (data.size() + 1 > N)
		{
			data = data.substr(0, N - 1);
		}

		T* stream;
		stream = get();

		// Set all characters to null.
		for (size_t i = 0; i < N; ++i)
			stream[i] = 0;

		for (size_t i = 0; i < data.size(); i++)
			stream[i] = data[i];

		Invalidate();
	}

	virtual ~CNetVarString() { }
};
