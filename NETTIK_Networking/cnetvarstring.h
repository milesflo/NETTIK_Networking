#pragma once
#include "CNetVarBase.h"
#include "CNetworkVector3.h"
#include "IDebug.h"
#define DEFINE_NetString(name, size) \
	CNetVarString<char [size]> name = CNetVarString<char [size]>(this, #name);

//! A three dimension network variable for representing
//  X,Y,Z.
template <typename DataArray>
class CNetVarString : public CNetVarBase<DataArray>
{
private:
	static const size_t sLength = sizeof(DataArray) / sizeof(char);

public:

	CNetVarString(NetObject* parent, const char* name) : CNetVarBase(parent, name, true)
	{

		for (size_t i = 0; i < sLength; i++)
		{
			m_Data[i] = 0;
		}
	}

	void Set(std::string& data)
	{
		if (data.size() + 1 > sLength)
			return;

		char* stream;
		stream = get();

		// Set all characters to null.
		for (size_t i = 0; i < sLength; ++i)
			stream[i] = 0;

		for (size_t i = 0; i < data.size(); i++)
			stream[i] = data[i];

		Invalidate();
	}
	virtual ~CNetVarString() { }
};
