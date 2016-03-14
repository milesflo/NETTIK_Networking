#pragma once
#include <exception>
#include <string>
#include <windows.h>
#include <sstream>
#include <iostream>

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

		CNETTIK_Issue(const std::string& error, const std::string& file, unsigned int line)
		{
			std::stringstream stream;
			stream << file << std::endl << "Line: " << line << ": " << error;

			m_sErrorMessage = stream.str();
			std::cout << m_sErrorMessage << std::endl;

			ToMessage();

		}

		void ToMessage()
		{
			MessageBoxA(NULL, m_sErrorMessage.c_str(), "NETTIK Error", MB_OK);
		}

		virtual const char* what() const throw()
		{
			return m_sErrorMessage.c_str();
		}

		virtual ~CNETTIK_Issue() { }
	};

}