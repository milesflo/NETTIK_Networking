#include "CNetVarList.h"
#define LOG_CALL \
	printf("%s\n", __FUNCTION__)

CNetVarList* CNetVarList::FetchObjectList(NetObject* pNetworkObject, std::uint32_t index)
{
	if (pNetworkObject == nullptr)
	{
		printf("warning: nullptr when fetching object list\n");
		return nullptr;
	}

	auto& object_lists = pNetworkObject->GetMaps();
	
	if (pNetworkObject->m_Mutex.try_lock())
	{
		CNetVarList* pResultList;
		try {
			pResultList = object_lists.at(index);
		}
		catch (std::exception& e)
		{
			printf("warning: list not found with index %d (%s)\n", index, e.what());
			pResultList = nullptr;
		}
		pNetworkObject->m_Mutex.unlock();
		return pResultList;
	}

	NETTIK_EXCEPTION("FetchObjectList failed. Tried locking object, inconsistent class tables.");
	return nullptr;
}

bool CNetVarList::HasWritePermission(ENetPeer* pCallee, NetObject* pCheckObject)
{
	NETTIK::IController* pNetworkController = NETTIK::IController::GetSingleton();
	
	if (!pNetworkController)
	{
		return false;
	}

	// Server maintains full write permissions on an entity.
	if (pNetworkController->IsServer())
	{
		return true;
	}

	if (pCallee == nullptr || pCheckObject == nullptr)
	{
		printf("warning: tried checking nullptr objects (%p, %p)\n", pCallee, pCheckObject);
		return false;
	}

	return (pCallee == pCheckObject->m_pPeer);
}

NetObject* CNetVarList::DecodeObject(const INetworkAssociatedObject& associated_object)
{
	IEntityManager   * pManager;
	VirtualInstance  * pInstance;

	NETTIK::IController* pNetworkController = NETTIK::IController::GetSingleton();
	if (!pNetworkController)
	{
		throw std::runtime_error("network controller is nullptr");
	}

	pInstance = pNetworkController->GetInstance( associated_object.instance_name() );
	if (!pInstance)
	{
		throw std::runtime_error("instance is nullptr");
	}

	pManager = pInstance->GetManager( associated_object.manager_name() );
	if (!pManager)
	{
		throw std::runtime_error("manager is nullptr");
	}

	return pManager->GetByNetID( associated_object.network_code() );
}

void CNetVarList::on_remote_add(ENetPeer* pPeer, INetworkMapAdd& packet)
{
	LOG_CALL;

	NetObject * pTargetObject;

	// Decode the packet target object into a NetObject pointer.
	try {
		pTargetObject = DecodeObject( packet.target_object() );
	}
	catch (std::runtime_error& e)
	{
		printf("warning: dropped on_remote_add, exception: %s\n", e.what());
		return;
	}

	if (pTargetObject == nullptr)
	{
		printf("error: target object not found. netcode: %d\n", packet.target_object().network_code());
		return;
	}

	// Try to find the list assigned to the packet using the 
	// list_id.
	CNetVarList* pList = FetchObjectList(pTargetObject, packet.list_id());
	if (!pList)
	{
		printf("warning: dropped on_remote_add, list is nullptr (%d)\n", packet.list_id());
		return;
	}

	std::uint32_t item_key = packet.key();
	char   * pItemData;
	size_t   iItemSize;

	// Get the list item's data information for transferring
	// the packet over.
	try {
		data_point insert_result = pList->build_at( item_key );
		pItemData = reinterpret_cast<char*>( insert_result.first );
		iItemSize = insert_result.second;

		printf("inserted item at %p with size of %d\n", pItemData, iItemSize);
	}
	catch (std::exception& e)
	{
		printf("error: insertation of new item failed, %s", e.what());
		return;
	}

	// After creation, write the initial data passed via the packet. Do a 
	// bounds check before to ensure that we aren't gonna write too far over.
	const std::string& iRemoteData = packet.value();
	const size_t       iRemoteSize = iRemoteData.length();

	if ( iItemSize != iRemoteSize )
	{
		printf("error: initial item length in packet does not meet template size, preventing write.");
		return;
	}

	// Size agreed, write away. 
	const char* pRemoteData = iRemoteData.c_str();
	for (size_t i = 0; i < iItemSize; ++i)
	{
		pItemData[ i ] = pRemoteData[ i ];
	}

	printf("wrote\n");
	printf("debug = %d\n", *reinterpret_cast<const example_t*>( pRemoteData ) );
}

void CNetVarList::on_remote_update(ENetPeer* pPeer, INetworkMapUpdate& packet)
{

	LOG_CALL;

}

void CNetVarList::on_remote_remove(ENetPeer* pPeer, INetworkMapRemove& packet)
{

	LOG_CALL;

}

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
		m_ListID = lists.size() - 1;

		m_pParent->m_Mutex.unlock();
		return;
	}
	NETTIK_EXCEPTION("Attempted to lock parent mutex on netlist but failed, inconsistent class tables.");
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
	NETTIK::IController* pNetworkController = NETTIK::IController::GetSingleton();

	// If the parent doesn't have a peer (clientside), and the peer is not a local
	// entity, don't try to update its contents to the server.
	//if (!pNetworkController->IsServer())
	//{
	//	if (m_pParent != nullptr && !m_pParent->IsNetworkLocal())
	//	{
	//		printf("tried modifying list without valid permissions (%p)\n", m_pParent->m_pPeer);
	//		return nullptr;
	//	}
	//}

	auto existing_it = m_Data.find( element_key );
	if ( existing_it != m_Data.end() )
	{
		printf("key already exists in list.\n");
		return &existing_it->second;
	}

	m_Data.insert(std::make_pair(element_key, data));
	existing_it = m_Data.find( element_key );

	printf("created key %d\n", element_key);

	//std::unique_ptr<proto_add> delta_broadcast(new proto_add());
	//m_UpdateQueue.emplace_back(new proto_add());

	VirtualInstance  * pInstance = m_pParent->m_pInstance; // "wasteland"
	IEntityManager   * pManager = m_pParent->m_pManager;  // "players"

	if (pManager == nullptr || pInstance == nullptr)
	{
		NETTIK_EXCEPTION("Tried sending contents of invalid object manager.");
	}
	INetworkAssociatedObject object_ref;
	object_ref.set_instance_name( pInstance->GetName() ); // "wasteland"
	object_ref.set_manager_name( pManager->GetName() );   // "players"
	object_ref.set_network_code( m_pParent->m_NetCode );  // 0
	
	auto packet = construct_protoadd(&object_ref, element_key, data, m_ListID);
	m_UpdateQueue.push_back( std::move(packet) );

	return (existing_it != m_Data.end() ? &existing_it->second : nullptr);
}

void CNetVarList::SendContents(ENetPeer* pPeer)
{
	mutex_guard guard( m_Mutex );

	VirtualInstance  * pInstance = m_pParent->m_pInstance; // "wasteland"
	IEntityManager   * pManager  = m_pParent->m_pManager;  // "players"

	if (pPeer == nullptr)
	{
		NETTIK_EXCEPTION("Passed peer is nullptr");
	}

	if (pManager == nullptr || pInstance == nullptr)
	{
		NETTIK_EXCEPTION("Tried sending contents of invalid object manager.");
	}

	INetworkAssociatedObject object_ref;
	object_ref.set_instance_name( pInstance->GetName() ); // "wasteland"
	object_ref.set_manager_name( pManager->GetName() );   // "players"
	object_ref.set_network_code( m_pParent->m_NetCode );  // 0
	
	for (auto map_item = m_Data.begin(); map_item != m_Data.end(); ++map_item)
	{
		auto packet = construct_protoadd(&object_ref, map_item->first, map_item->second, m_ListID);
		packet->AddPeer(pPeer);
		packet->Dispatch();
	}
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
	if (!m_UpdateQueue.empty())
	{
		printf("Flush()\n");
	}
	
	// Clearing the list will invoke all proto packets
	// to dispatch their contents to linked peers.
	m_UpdateQueue.clear();
}