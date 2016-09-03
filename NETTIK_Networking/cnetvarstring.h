//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "CNetVarBase.h"

//-----------------------------------------------
// A fixed size string blob.
//-----------------------------------------------
#define NetworkString(name, size) \
	CNetVarString< char, std::string, size >     name   = CNetVarString< char, std::string, size >( this, #name )

//-----------------------------------------------
// A fixed size string blob that supports
// unicode content.
//-----------------------------------------------
#define NetworkUnicodeString(name, size) \
	CNetVarString< wchar_t, std::wstring, size > name   = CNetVarString<  wchar_t, std::wstring, size >( this, #name );

//-----------------------------------------------
// Represents an array that stores and manipulates
// a sequence of char-like objects.
//-----------------------------------------------
template < typename T, typename S, size_t N >
class CNetVarString : public CNetVarBase< T[N] >
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

		T* stream = get();

		// Set all characters to NULL (+0x00).
		for (size_t i = 0; i < N; ++i)
		{
			stream[i] = 0;
		}

		// Copy the string data into the storage.
		for (size_t i = 0; i < data.size(); ++i)
		{
			stream[i] = data[i];
		}

		// Mark variable as changed.
		Invalidate();
	}

	virtual ~CNetVarString()
	{

	}
};
