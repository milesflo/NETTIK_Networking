#include "NetVarListBase.h"
#include "CNetVarBase.h"

NetVarListBase::NetVarListBase(const size_t dataSize, const char* name, NetObject* parent) :
	m_iDataSize(dataSize),
	m_psName(name),
	m_pParent(parent)
{

}

void NetVarListBase::bind_controller(NetSystem* pNetworkController)
{
	pNetworkController->on(INetworkCodes::internal_evt_list_add,    CALL_RPC_FAR_PROTO(INetworkMapAdd,     on_remote_add    ));
	pNetworkController->on(INetworkCodes::internal_evt_list_data,   CALL_RPC_FAR_PROTO(INetworkMapUpdate,  on_remote_update ));
	pNetworkController->on(INetworkCodes::internal_evt_list_remove, CALL_RPC_FAR_PROTO(INetworkMapRemove,  on_remote_remove ));
}

void NetVarListBase::Flush()
{
	mutex_guard guard(m_Mutex);

	// Clearing the list will invoke all proto packets
	// to dispatch their contents to linked peers.
	m_UpdateQueue.clear();
}

void NetVarListBase::on_remote_update(ENetPeer* pPeer, INetworkMapUpdate& packet)
{
	std::shared_ptr<NetObject> pTargetObject;

	// Decode the packet target object into a NetObject pointer.
	try {
		pTargetObject = DecodeObject( packet.target_object() );
	}
	catch (std::runtime_error& e)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "Dropped %s: %s", __FUNCTION__, e.what());
		return;
	}

	if (pTargetObject == nullptr)
	{
		CMessageDispatcher::Add(kMessageType_Error, "(%s) Target object %d not found", __FUNCTION__, packet.target_object().network_code());
		return;
	}

	// Try to find the list assigned to the packet using the 
	// list_id.
	NetVarListBase* pList = FetchObjectList(pTargetObject, packet.list_id());
	if (!pList)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "(%s) Target list %d not found", __FUNCTION__, packet.list_id());
		return;
	}

	std::uint32_t item_key = packet.key();
	char   * pItemData;
	size_t   iItemSize;

	// Get the list item's data information for transferring
	// the packet over.
	try {
		data_point insert_result = pList->get_at( item_key );
		pItemData = reinterpret_cast<char*>( insert_result.first );
		iItemSize = insert_result.second;
	}
	catch (std::exception& e)
	{
		CMessageDispatcher::Add(kMessageType_Error, "(%s) Lookup for item %d failed: %s", __FUNCTION__, item_key, e.what());
		return;
	}

	// After creation, write the initial data passed via the packet. Do a 
	// bounds check before to ensure that we aren't gonna write too far over.
	const std::string& iRemoteData = packet.value();
	const size_t       iRemoteSize = iRemoteData.length();

	if ( iItemSize != iRemoteSize )
	{
		CMessageDispatcher::Add(kMessageType_Warn, "(%s) Initial item length in packet does not meet template size, preventing write.", __FUNCTION__);
		return;
	}

	// Size agreed, write away. 
	const char* pRemoteData = iRemoteData.c_str();
	for (size_t i = 0; i < iItemSize; ++i)
	{
		pItemData[ i ] = pRemoteData[ i ];
	}

	pList->dispatch_update(item_key);
}

void NetVarListBase::on_remote_remove(ENetPeer* pPeer, INetworkMapRemove& packet)
{
	std::shared_ptr<NetObject> pTargetObject;

	// Decode the packet target object into a NetObject pointer.
	try {
		pTargetObject = DecodeObject( packet.target_object() );
	}
	catch (std::runtime_error& e)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "Dropped %s: %s", __FUNCTION__, e.what());
		return;
	}

	if (pTargetObject == nullptr)
	{
		CMessageDispatcher::Add(kMessageType_Error, "(%s) Target object %d not found", __FUNCTION__, packet.target_object().network_code());
		return;
	}

	// Try to find the list assigned to the packet using the 
	// list_id.
	NetVarListBase* pList = FetchObjectList(pTargetObject, packet.list_id());
	if (!pList)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "(%s) Target list %d not found", __FUNCTION__, packet.list_id());
		return;
	}

	std::uint32_t item_key = packet.key();

	// Dispatch callback before removing item in memory.
	pList->dispatch_remove(item_key);

	try {
		pList->remove_at(item_key);
	}
	catch (std::exception& e)
	{
		CMessageDispatcher::Add(kMessageType_Error, "(%s) Lookup for item %d failed: %s", __FUNCTION__, item_key, e.what());
	}
}

void NetVarListBase::on_remote_add(ENetPeer* pPeer, INetworkMapAdd& packet)
{
	std::shared_ptr<NetObject> pTargetObject;

	// Decode the packet target object into a NetObject pointer.
	try {
		pTargetObject = DecodeObject( packet.target_object() );
	}
	catch (std::runtime_error& e)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "Dropped %s: %s", __FUNCTION__, e.what());
		return;
	}

	if (pTargetObject == nullptr)
	{
		CMessageDispatcher::Add(kMessageType_Error, "(%s) Target object %d not found", __FUNCTION__, packet.target_object().network_code());
		return;
	}

	// Try to find the list assigned to the packet using the 
	// list_id.
	NetVarListBase* pList = FetchObjectList(pTargetObject, packet.list_id());
	if (!pList)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "(%s) Target list %d not found", __FUNCTION__, packet.list_id());
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
	}
	catch (std::exception& e)
	{
		CMessageDispatcher::Add(kMessageType_Error, "Insertation of new item failed: %s", e.what());
		return;
	}

	// After creation, write the initial data passed via the packet. Do a 
	// bounds check before to ensure that we aren't gonna write too far over.
	const std::string& iRemoteData = packet.value();
	const size_t       iRemoteSize = iRemoteData.length();

	if ( iItemSize != iRemoteSize )
	{
		CMessageDispatcher::Add(kMessageType_Warn, "(%s) Initial item length in packet does not meet template size, preventing write.", __FUNCTION__);
		return;
	}

	// Size agreed, write away. 
	const char* pRemoteData = iRemoteData.c_str();
	for (size_t i = 0; i < iItemSize; ++i)
	{
		pItemData[ i ] = pRemoteData[ i ];
	}

	pList->dispatch_add(item_key);
}

std::shared_ptr<NetObject> NetVarListBase::DecodeObject(const INetworkAssociatedObject& associated_object)
{
	IEntityManager   * pManager;
	VirtualInstance  * pInstance;

	NetSystem* pNetworkController = NetSystem::GetSingleton();
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

bool NetVarListBase::HasWritePermission(ENetPeer* pCallee, NetObject* pCheckObject)
{
	NetSystem* pNetworkController = NetSystem::GetSingleton();
	
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
		CMessageDispatcher::Add(kMessageType_Warn, "Tried checking nullptr objects %p and %p", pCallee, pCheckObject);
		return false;
	}

	return (pCallee == pCheckObject->m_pPeer);
}

NetVarListBase* NetVarListBase::FetchObjectList(std::shared_ptr<NetObject>& pNetworkObject, std::uint32_t index)
{
	if (pNetworkObject == nullptr)
	{
		CMessageDispatcher::Add(kMessageType_Warn, "nullptr when fetching object list");
		return nullptr;
	}

	auto& object_lists = pNetworkObject->GetMaps();
	
	if (pNetworkObject->m_Mutex.try_lock())
	{
		NetVarListBase* pResultList;
		try {
			pResultList = object_lists.at(index);
		}
		catch (std::exception& e)
		{
			CMessageDispatcher::Add(kMessageType_Warn, "List not found with index %d (%s)", index, e.what());
			pResultList = nullptr;
		}
		pNetworkObject->m_Mutex.unlock();
		return pResultList;
	}

	NETTIK_EXCEPTION("FetchObjectList failed. Tried locking object, inconsistent class tables.");
	return nullptr;
}
