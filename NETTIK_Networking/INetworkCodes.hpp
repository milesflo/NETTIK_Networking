//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <cinttypes>

class INetworkCodes
{
public:
	//----------------------------------------
	// The cross application data type for
	// network RPC codes. Take data alignment
	// into consideration.
	//----------------------------------------
	typedef std::uint16_t  msg_t;
	#define MSG_CAST( data ) static_cast<msg_t>( data )

	//----------------------------------------
	// Codes for internal RPC operations for
	// NETTIK. In unsinged form to allow 
	// the app to build on the network codes
	// without overloading these accidentally.
	//----------------------------------------
	enum NetworkCodesShared : msg_t
	{
		//----------------------------------------
		// Call when allocating a key/value
		// pair into a networked list.
		//----------------------------------------
		internal_evt_list_add    = MSG_CAST( -1 ),

		//----------------------------------------
		// Call when there has been data changed
		// to a list element.
		//----------------------------------------
		internal_evt_list_data   = MSG_CAST( -2 ),

		//----------------------------------------
		// Call when deallocating a key/value
		// pair into a networked list.
		//----------------------------------------
		internal_evt_list_remove = MSG_CAST( -3 )
	};

	#undef MSG_CAST
};