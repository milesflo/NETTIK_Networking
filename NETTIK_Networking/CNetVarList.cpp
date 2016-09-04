#include "CNetVarList.h"

CNetVarList::CNetVarList(NetObject* parent, const char* name) : m_psName(name), m_pParent(parent)
{
	if (m_pParent == nullptr)
	{
		return;
	}

	if (m_pParent->m_Mutex.try_lock())
	{
		NetObject::MapList_t& lists = m_pParent->GetMaps();
		lists.push_back(this);

		m_pParent->m_Mutex.unlock();
		printf("m_pParent registered list.\n");
	}
	else
	{
		NETTIK_EXCEPTION("Attempted to lock parent mutex on netlist but failed, inconsistent class tables.");
	}
}

CNetVarList::~CNetVarList()
{
	if (m_pParent != nullptr && m_pParent->m_Mutex.try_lock())
	{
		NetObject::MapList_t& lists = m_pParent->GetMaps();

		auto it = std::find(lists.begin(), lists.end(), this);
		if (it != lists.end())
		{
			lists.erase(it);
		}

		m_pParent->m_Mutex.unlock();
	}
}

CNetVarList::example_t* CNetVarList::create(std::uint32_t element_key, example_t data)
{
	auto existing_it = m_Data.find( element_key );
	if ( existing_it != m_Data.end() )
	{
		printf("key already exists in list.\n");
		return &existing_it->second;
	}

	m_Data.insert(std::make_pair(element_key, data));
	existing_it = m_Data.find( element_key );

	printf("created key %d\n", element_key);
	return (existing_it != m_Data.end() ? &existing_it->second : nullptr);
}

void CNetVarList::SendContents(ENetPeer* pPeer)
{
	mutex_guard guard( m_Mutex );

	IEntityManager* pManager = m_pParent->m_pManager;

	if (pManager == nullptr)
	{
		NETTIK_EXCEPTION("Tried sending contents of invalid object manager.");
	}

	INetworkAssociatedObject object_ref;
	object_ref.set_instance_name(pManager->GetName());
	object_ref.set_network_code(m_pParent->m_NetCode);
	
	printf("INetworkAssociatedObject::instance_name = %s\n", pManager->GetName().c_str());
	printf("INetworkAssociatedObject::network_code  = %d\n", m_pParent->m_NetCode);

	for (auto map_item = m_Data.begin(); map_item != m_Data.end(); ++map_item)
	{
		printf("processing %d (%d)\n", map_item->first, map_item->second);

		NETTIK::IPacketFactory::CProtoPacket<INetworkMapAdd> update_packet;
		update_packet.mutable_target_object()->CopyFrom( object_ref );
		
		update_packet.set_key(map_item->first);

		const void* element_stream = reinterpret_cast<const void*>( &map_item->second );
		const size_t element_size =  sizeof( example_t );

		update_packet.set_value(element_stream, element_size);

		update_packet.AddPeer(pPeer);
	}
	printf("fin.\n");
}

void CNetVarList::SendDelta(IEntityManager* pManager)
{
	std::vector<ENetPeer*> pTargets;
	// Get all of the parent instance's peers and push them 
	// into a target list. 
	{
		LockableVector<NetObject*>& object_list = pManager->GetObjects();
		object_list.safe_lock();

		auto object_list_raw = object_list.get();

		for (auto object = object_list_raw->begin(); object != object_list_raw->end(); ++object)
		{
			// Bots don't have peers, make sure we're not pushing nullptr
			// bot objects into the firing line.
			if ((*object)->m_pPeer != nullptr)
			{
				pTargets.push_back((*object)->m_pPeer);
			}
		}

		object_list.safe_unlock();
	}

	// Add all the peers to each update queue entry. Call flush afterwards to
	// invoke the dispatch of the packet data.
	mutex_guard guard(m_Mutex);
	
	for (auto update_item = m_UpdateQueue.begin(); update_item != m_UpdateQueue.end(); ++update_item)
	{
		for (auto peer = pTargets.begin(); peer != pTargets.end(); ++peer)
		{
			(*update_item)->AddPeer(*peer);
			printf("debug: allocating %p\n", *peer);
		}
	}
}

void CNetVarList::Flush()
{
	mutex_guard guard( m_Mutex );
	
	// Clearing the list will invoke all proto packets
	// to dispatch their contents to linked peers.
	m_UpdateQueue.clear();
}