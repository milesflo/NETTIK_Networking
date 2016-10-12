//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include "SnapshotStream.h"
#include "INetworkCodes.hpp"
#include "SnapshotHeader.h"
#include "Constraints.h"
#include <enet\types.h>
#include <cstddef>

// Todo: Reorder because this being 4 bytes is bad.
//       but.... it ensures that data is aligned ok...
enum FrameType : uint32_t
{
	kFRAME_Data    = 0xDADADADA,
	kFRAME_Alloc   = 0xACACACAC,
	kFRAME_Dealloc = 0xDEDEDEDE
};

struct SnapshotEntListDataEntry
{
	FrameType frameType;							// 0x00

	// NetID
	uint32_t netID = 0;								// 0x04

	uint32_t controller = NET_CONTROLLER_NONE;		// 0x08
	
	// Forced flag.
	bool forced = false;

	// Entity name / varname
	char name[max_entvar_name] = { 0 };
	
	// Data buffer.
	unsigned char data[max_entvar_data] = { 0 };

	// Unused.
	bool padding_01 = false;
	bool padding_02 = false;
	bool padding_03 = false;
};
static_assert(sizeof(SnapshotEntListDataEntry) == 68U + 128U + 4U + 4U + 4U, "SnapshotEntListDataEntry size not correct.");

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
	size_t m_typesize = 0;
	void* m_dataPointer;

public:

	// high cpu load
	// 22.47%
	void write(SnapshotStream::Stream& data)
	{
		unsigned char* base_ptr;
		base_ptr = reinterpret_cast<unsigned char*>(&m_data);

		unsigned char* stream;
		stream = base_ptr;

		data.reserve(data.capacity() +  sizeof(SnapshotEntListDataEntry));

		for (size_t i = 0; i < sizeof(FrameType); i++)
			data.push_back(stream[i]);
		stream += sizeof(FrameType);

		for (size_t i = 0; i < sizeof(uint32_t); i++)
			data.push_back(stream[i]);
		stream += sizeof(uint32_t);

		for (size_t i = 0; i < sizeof(uint32_t); i++)
			data.emplace_back(stream[i]);
		stream += sizeof(uint32_t);

		data.push_back(*stream);
		stream += sizeof(bool);

		size_t name_length = strlen(m_data.name);

		// Should get a terminating null character.
		for (size_t i = 0; i < name_length + 1; i++)
			data.push_back(stream[i]);

		stream = reinterpret_cast<unsigned char*>(&m_data.data);

		for (size_t i = 0; i < m_typesize; i++)
			data.push_back(*stream++);

		for (size_t i = m_typesize; i < max_entvar_data; i++)
			data.push_back(0);

	}

	void read_data(const enet_uint8* stream, size_t stream_len)
	{
		// If the stream is less than a single character with a netcode and a byte, or
		// greater than the max data entry size, drop.
		if (stream_len < sizeof(FrameType) + sizeof(uint32_t) + sizeof(char) + sizeof(char) ||
			stream_len > sizeof(SnapshotEntListDataEntry) + sizeof(SnapshotHeaderData))
			NETTIK_EXCEPTION("Buffer underflow / overflow.");

		// header | [data entry]
		// | network id |

		unsigned char* base_ptr;
		base_ptr = reinterpret_cast<unsigned char*>(const_cast<enet_uint8*>(stream));

		unsigned char* current_ptr;
		current_ptr = base_ptr;

		FrameType* frameType_ptr;
		frameType_ptr = reinterpret_cast<FrameType*>(current_ptr);
		current_ptr += sizeof(FrameType);

		m_data.frameType = *frameType_ptr;

		uint32_t* netID_ptr;
		netID_ptr = reinterpret_cast<uint32_t*>(current_ptr);
		current_ptr += sizeof(uint32_t);

		m_data.netID = *netID_ptr;

		uint32_t* controller_ptr;
		controller_ptr = reinterpret_cast<uint32_t*>(current_ptr);
		current_ptr += sizeof(uint32_t);

		m_data.controller = *controller_ptr;

		bool* forced_ptr;
		forced_ptr = reinterpret_cast<bool*>(current_ptr);
		current_ptr += sizeof(bool);

		m_data.forced = *forced_ptr;

		for (size_t i = 0; i < max_entvar_name; i++)
		{
			if (*current_ptr == 0)
				break;

			m_data.name[i] = (unsigned char)*current_ptr++;
		}

		current_ptr++;

		size_t data_len;
		for (data_len = 0; data_len < max_entvar_data; data_len++)
		{
			m_data.data[data_len] = (char)*current_ptr++;
		}
		m_data.data[data_len + 1] = 0; // null terminate

		m_dataPointer = reinterpret_cast<void*>(++current_ptr);

	}

	inline bool get_forced() const
	{
		return m_data.forced;
	}

	inline void set_forced(bool val)
	{
		m_data.forced = val;
	}

	inline const void* get_ptr() const
	{
		return m_dataPointer;
	}

	inline uint32_t get_netid() const
	{
		return m_data.netID;
	}

	inline const char* get_name() const
	{
		return m_data.name;
	}

	inline const unsigned char* get_data() const
	{
		return m_data.data;
	}
	
	inline uint32_t get_controller() const
	{
		return m_data.controller;
	}

	inline FrameType get_frametype() const
	{
		return m_data.frameType;
	}

	void set_name(std::string data)
	{
		for (size_t i = 0; i < data.size(); i++)
			m_data.name[i] = data[i];

		m_data.name[data.size() + 1] = 0; // null character
	}

	inline void set_netid(uint32_t netid)
	{
		m_data.netID = netid;
	}

	inline void set_frametype(FrameType type)
	{
		m_data.frameType = type;
	}

	inline void set_controller(uint32_t cid)
	{
		m_data.controller = cid;
	}

	inline void set_data(unsigned char* stream, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			m_data.data[i] = stream[i];
		}

		m_typesize = size;
	}

	inline void set_data_null()
	{
		m_typesize = 2;
		m_data.data[0] = 0x01;
		m_data.data[1] = 0x00; // Set to zero just for consistency against c-strings.
	}

	void read_data(SnapshotStream::Stream& data)
	{

		// TODO: Some checks.

		enet_uint8* stream;
		stream = &data[0];

		read_data(stream, data.size());
	}

	// Static data.
	inline size_t typesize()
	{
		return m_typesize;
	}

};