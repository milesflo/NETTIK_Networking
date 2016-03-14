#pragma once
#include "SnapshotStream.h"
#include "INetworkCodes.hpp"
#include "IDebug.hpp"
#include "SnapshotHeader.h"
#include "Constraints.h"

#include <enet\types.h>

struct SnapshotEntListDataEntry
{
	uint32_t netID = 0;
	char name[max_entvar_name] = { 0 };
	
	// Data buffer.
	unsigned char data[max_entvar_data] = { 0 };
};
static_assert(sizeof(SnapshotEntListDataEntry) == 68U + 128U, "SnapshotEntListDataEntry size not correct.");

inline void* add_ptr(void* in, size_t offset)
{
	unsigned char* data = reinterpret_cast<unsigned char*>(in);
	data += offset;

	return data;
}

//                        | netid          | ' ', '\0'                   | sizeof(datatype)
//const size_t vardat_min = sizeof(uint32_t) + sizeof(char) + sizeof(char);

class SnapshotEntList
{
private:
	SnapshotEntListDataEntry m_data;
public:

	void read(const enet_uint8* stream, size_t stream_len)
	{
		// If the stream is less than a single character with a netcode and a byte, or
		// greater than the max data entry size, drop.
		if (stream_len < sizeof(uint32_t) + sizeof(char) + sizeof(char) ||
			stream_len > sizeof(SnapshotEntListDataEntry))
			NETTIK_EXCEPTION("Buffer underflow / overflow.");

		// header | [data entry]
		// | network id |

		m_data.netID = (uint32_t)(*stream);

		char* current_ptr;
		current_ptr = (char*)add_ptr((void*)stream, 4);

		for (uint32_t i = 0; i < max_entvar_name; i++)
		{
			if (*current_ptr == 0)
				break;

			m_data.name[i] = *current_ptr++;
		}

		printf("NETID: %d\n", m_data.netID);
		printf("NAME: %s\n", m_data.name);
	}

	void read(SnapshotStream::Stream& data)
	{

		// TODO: Some checks.

		enet_uint8* stream;
		stream = &data[0];

		read(stream, data.size());
	}

};