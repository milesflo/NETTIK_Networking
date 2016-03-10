#include "CEntities.h"
#include "IController.hpp"
uint32_t m_TotalEntities = 0;

void INetObject::GetObjectSnapshot(size_t& max_value, uint16_t& num_updates, std::vector<std::vector<unsigned char>>& buffers, bool bForced)
{
	for (auto it = m_Vars.begin(); it != m_Vars.end(); ++it)
	{
		std::vector<unsigned char> buffer;
		size_t max_size = it->second->Snapshot(buffer, bForced);
		if (max_size == 0)
			continue;

		if (max_size > max_value)
			max_value = max_size;

		buffers.push_back(buffer);
		num_updates++;
	}
}