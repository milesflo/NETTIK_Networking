//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <exception>
#include <string>
#include <sstream>

//! Compile linked runtime error with file/line information appended.
#define NETTIK_EXCEPTION(msg) \
	throw NETTIK::CNETTIK_Issue(msg, __FILE__, __LINE__)

namespace NETTIK
{

	//! Used for VS Intellisense "mouse over" value debugging.
	template <size_t S> class static_info
	{

	};
	// ie. static_info<sizeof(int)> debug; -> "4/8"

	//! Custom exception for NETTIK specific application errors.
	class CNETTIK_Issue : public std::exception
	{

	protected:
		std::string  m_sErrorMessage;

	public:

		CNETTIK_Issue(const std::string& error, const std::string& file, unsigned int line);

		void ToMessage();

		virtual const char* what() const;

		virtual ~CNETTIK_Issue();
	};

}