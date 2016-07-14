//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <vector>

class SnapshotStream
{
public:
	typedef std::vector<unsigned char> Stream;
	typedef std::vector<Stream>        StreamCollection;

	Stream& create()
	{
		m_data.emplace_back();
		return m_data.back();
	}

	Stream& header()
	{
		return m_data.at(0);
	}

	Stream& result()
	{
		if (m_Finished)
			return m_data.at(0);

		Stream& header = m_data.at(0);
		// Copy contents into header.
		for (size_t i = 1; i < m_data.size(); ++i)
		{
			Stream& stream = m_data.at(i);

			for (auto it = stream.begin(); it != stream.end(); ++it)
			{
				header.push_back(*it);
			}
		}

		m_Finished = true;
		return m_data.at(0);
	}

	inline void clear()
	{
		m_Finished = false;
		m_data.clear();
		m_data.emplace_back();
	}

	inline bool modified()
	{
		return m_data.size() > 1;
	}

	inline StreamCollection* get()
	{
		return &m_data;
	}

	SnapshotStream()
	{
		m_data.emplace_back();
	}

protected:
	StreamCollection m_data;
	bool  m_Finished = false;
};