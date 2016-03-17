#pragma once
#include "SnapshotStream.h"
#include "INetworkCodes.hpp"
#include "IDebug.hpp"
#include "NETIDReserved.h"
#include <enet\types.h>
#include <cstddef>

struct SnapshotHeaderData
{
	NETTIK::INetworkCodes::msg_t    message_code = 0; // 0

	uint16_t count = 0;									// 2
	uint32_t sequence = 0;								// 4

	// 4
	size_t   max_message_length = 0;                  // 8 (x86)
};
static_assert(sizeof(SnapshotHeaderData) == 12U, "Snapshot header size not correct.");


class SnapshotHeader
{
private:
	SnapshotStream::Stream&      header;
	SnapshotHeaderData m_data;

public:

	static void Generate(SnapshotStream& stream, uint32_t sequence, uint16_t updates, size_t max)
	{

		static NETTIK::INetworkCodes::msg_t code = NETID_Reserved::RTTI_Object::OBJECT_FRAME;

		SnapshotStream::Stream& header = stream.header();

		SnapshotHeader generator(header);
		generator.set_code(code);
		generator.set_sequence(sequence);
		generator.set_count(updates);
		generator.set_max(max);
		generator.write();

		generator.copy_from(stream, max);
	}

	SnapshotHeader(SnapshotStream::Stream& header) : header(header)
	{

	}

	SnapshotHeader() : header(SnapshotStream::Stream()) { }

	void display();

	void write();

	void read(const enet_uint8* stream, size_t stream_len);
	void read(SnapshotStream::Stream& data);

	void copy_from(SnapshotStream& stream, size_t padding);

	inline NETTIK::INetworkCodes::msg_t& code()
	{
		return m_data.message_code;
	}
	inline uint32_t& sequence()
	{
		return m_data.sequence;
	}
	inline uint16_t& count()
	{
		return m_data.count;
	}

	inline size_t& max_size()
	{
		return m_data.max_message_length;
	}

	void set_code(NETTIK::INetworkCodes::msg_t& code);
	void set_sequence(uint32_t& seq);
	void set_count(uint16_t& count);
	void set_max(size_t& max);

	const size_t size()
	{
		return sizeof(SnapshotHeaderData);
	}
};