#include "NetObject.h"
#include "NetVar.h"
#include "CEntities.h"

void NetObject::DestroyNetworkedEntity()
{
	// yes... two way traffic :'(
	m_pManager->Remove(this->m_NetCode);
}

void NetObject::TakeObjectSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& buffers, bool bReliableFlag, bool bForced)
{
	for (auto it = m_Vars.begin(); it != m_Vars.end(); ++it)
	{
		if (it->second->GetReliable() != bReliableFlag)
			continue;

		size_t max_size = it->second->TakeVariableSnapshot(buffers, bForced);
		if (max_size == 0)
			continue;

		if (max_size > max_value)
			max_value = max_size;


		num_updates++;
	}
}