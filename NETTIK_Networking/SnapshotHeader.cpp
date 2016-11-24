//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "SnapshotHeader.h"

void SnapshotHeader::write()
{
	for (size_t i = 0; i < sizeof(SnapshotHeaderData); i++)
	{
		header.push_back(((unsigned char*)(&m_data))[i]);
	}
}

void SnapshotHeader::read(SnapshotStream::Stream& data)
{
	if (data.size() != sizeof(SnapshotHeaderData))
		NETTIK_EXCEPTION("Header read doesn't meet header size.");

	enet_uint8* stream;
	stream = reinterpret_cast<enet_uint8*>(&data[0]);

	read(stream, data.size());
}

void SnapshotHeader::read(const enet_uint8* stream, size_t stream_len)
{
	if (stream_len < sizeof(SnapshotHeaderData))
		NETTIK_EXCEPTION("Header read doesn't meet header size, INTERNAL.");

	SnapshotHeaderData* data_ptr;
	data_ptr = (SnapshotHeaderData*)(stream);

	m_data = *data_ptr;

	//m_code = *code_ptr;
	//m_sequence = *seq_ptr;
	//m_count = *count_ptr;
	//m_max = *max_ptr;
}

void SnapshotHeader::copy_from(SnapshotStream& stream, size_t padding)
{
	if (stream.get()->size() < 0)
		return;

	// Padd all update frames to the max value.
	for (auto it = stream.get()->begin() + 1; it != stream.get()->end(); ++it)
	{
		std::vector<unsigned char>* buffer;
		buffer = &(*it);

		if (buffer->size() < padding)
		{
			uint32_t padsize;
			padsize = padding - buffer->size();

			for (uint32_t i = 0; i < padsize; ++i)
				buffer->push_back(0);
		}

	}

}

void SnapshotHeader::display()
{
	std::cout << "-- snapshot header --" << std::endl;
	std::cout << "message code:\t" << m_data.message_code << std::endl;
	std::cout << "sequence:\t" << m_data.sequence << std::endl;
	std::cout << "count:\t" << m_data.count << std::endl;
	std::cout << "maxlen:\t" << m_data.max_message_length << std::endl;
	std::cout << " ------------------" << std::endl;
}

void SnapshotHeader::set_code(INetworkCodes::msg_t& code)
{
	m_data.message_code = code;
}

void SnapshotHeader::set_sequence(uint32_t& seq)
{
	m_data.sequence = seq;
}

void SnapshotHeader::set_count(uint16_t& count)
{
	m_data.count = count;
}

void SnapshotHeader::set_max(size_t& max)
{
	m_data.max_message_length = max;
}