#pragma once
#include <inttypes.h>

namespace NETTIK
{

	class INetworkCodes
	{

	public:

		//! Don't change the message code data type at this type.
		//  CHANGE AT YOUR OWN RISK.
		typedef uint8_t  msg_t;

		enum NetworkCodesShared : msg_t
		{
			SharedNull = 0,
			SharedPing = 1,

			_MaxShared = SharedPing,
		};

		enum NetworkCodesClient : msg_t
		{
			ClientNull = _MaxShared + 1,

			_MaxCodesClient = ClientNull
		};

		enum NetworkCodesServer : msg_t
		{
			ServerNull = _MaxCodesClient + 1,

			_MaxCodesServer = ServerNull
		};


	};
};