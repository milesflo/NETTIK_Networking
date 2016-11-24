#pragma once
//------------------------------------
// Inline unicode string formatter.
//------------------------------------
// Thanks to GWEN.
// https://github.com/garrynewman/GWEN/blob/a0f0a2be6b87fae889c6a49183cd7b8132f6e554/gwen/src/Utility.cpp#L37
#ifdef _MSC_VER
	#pragma warning(disable:4267) // conversion from 'size_t' to 'int', possible loss of data
#endif

#ifdef __MINGW32__
	#undef vswprintf
	#define vswprintf _vsnwprintf
#endif

#ifdef _MSC_VER
	#define GWEN_FNULL "NUL"
	#define va_copy(d,s) ((d) = (s))
#else
	#define GWEN_FNULL "/dev/null"
#endif

//-----------------------------------------------------------
// Formats an ASCII string. Third parameter denotes the last
// parameter in the current calling context to start the
// va_args list from. 
//-----------------------------------------------------------
#define FormatAString(strOut, fmt, _start)                  \
	va_list s;                                              \
	int len = 0;                                            \
	                                                        \
	va_start(s, _start);                                    \
	                                                        \
	                                                        \
	{                                                       \
		FILE* fnull;                                        \
		fopen_s(&fnull, GWEN_FNULL, "wb");                  \
		va_list c;                                          \
		va_copy(c, s);                                      \
		if (fnull)                                          \
		{                                                   \
			len = vfprintf_s(fnull, fmt, c);                \
		}                                                   \
		va_end(c);                                          \
		fclose(fnull);                                      \
	}                                                       \
	                                                        \
	                                                        \
	if (len > 0)                                            \
	{                                                       \
		strOut.resize(len + 1);                             \
		va_list c;                                          \
		va_copy(c, s);                                      \
		len = vsprintf_s(&strOut[0], strOut.size(), fmt, c);\
		va_end(c);                                          \
		strOut.resize(len);                                 \
	}                                                       \
	                                                        \
	va_end(s);                                  

//-----------------------------------------------------------
// Formats a UTF-8 string. Third parameter denotes the last
// parameter in the current calling context to start the
// va_args list from. 
//-----------------------------------------------------------
#define FormatWString(strOut, fmt, _start)                  \
	va_list s;                                              \
	int len = 0;                                            \
	                                                        \
	va_start(s, _start);                                    \
	                                                        \
	                                                        \
	{                                                       \
		FILE* fnull;                                        \
		fopen_s(&fnull, GWEN_FNULL, "wb");                  \
		va_list c;                                          \
		va_copy(c, s);                                      \
		if (fnull)                                          \
		{                                                   \
			len = vfwprintf(fnull, fmt, c);                 \
		}                                                   \
		va_end(c);                                          \
		fclose(fnull);                                      \
	}                                                       \
	                                                        \
	                                                        \
	if (len > 0)                                            \
	{                                                       \
		strOut.resize(len + 1);                             \
		va_list c;                                          \
		va_copy(c, s);                                      \
		len = vswprintf(&strOut[0], strOut.size(), fmt, c); \
		va_end(c);                                          \
		strOut.resize(len);                                 \
	}                                                       \
	                                                        \
	va_end(s);                                             