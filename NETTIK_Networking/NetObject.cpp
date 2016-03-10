#include "NetObject.h"
#include "NetVar.h"
//#include "CEntities.h"

void NetObject::TakeObjectSnapshot(size_t& max_value, uint16_t& num_updates, std::vector<std::vector<unsigned char>>& buffers, bool bReliableFlag, bool bForced)
{
	for (auto it = m_Vars.begin(); it != m_Vars.end(); ++it)
	{
		if (it->second->GetReliable() != bReliableFlag)
			continue;

		std::vector<unsigned char> buffer;
		size_t max_size = it->second->TakeVariableSnapshot(buffer, bForced);
		if (max_size == 0)
			continue;

		if (max_size > max_value)
			max_value = max_size;

		buffers.push_back(buffer);
		num_updates++;
	}
}