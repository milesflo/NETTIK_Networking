#pragma once
#include "NetVarListBase.h"
#define NetworkList( type, name )  CNetVarList<type> name = { this, #name }

class NetObject;
//------------------------------------------
// A dynamic list that updates its contents 
// across the network to associated peers. 
// 
// Similar structure to the NetVar class, 
// but controls data access to dispatch
// updates via RPC and not via snapshot.
//------------------------------------------

template <class example_t>
class CNetVarList : public NetVarListBase
{
public:
	// change when ready for template initialization.
	using proto_add    = NETTIK::IPacketFactory::CProtoPacket<INetworkMapAdd>;
	using proto_update = NETTIK::IPacketFactory::CProtoPacket<INetworkMapUpdate>;
	using proto_remove = NETTIK::IPacketFactory::CProtoPacket<INetworkMapRemove>;
	
	CNetVarList(NetObject* parent, const char* name) : NetVarListBase(sizeof( example_t ), name, parent)
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

	virtual ~CNetVarList()
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

	//-------------------------------------------
	// Static callback handlers
	//-------------------------------------------
	// Called when a remote peer invokes an 
	// item to be created (with data).

protected:
	
	template <class T>
	inline std::unique_ptr<proto_add> construct_protoadd(INetworkAssociatedObject* object, std::uint32_t key, T& value, std::uint32_t list_id)
	{
		const void   * element_stream = reinterpret_cast<const void*>(&value);
		const size_t   element_size   = sizeof( T );

		std::unique_ptr<proto_add> result;
		result.reset( new proto_add( NETTIK::INetworkCodes::internal_evt_list_add ) );

		if (object != nullptr)
		{
			result->mutable_target_object()->CopyFrom(*object);
		}
		
		result->set_key(key);
		result->set_value(element_stream, element_size);
		result->set_list_id(list_id);

		result->SetReliable();

		return std::move(result);
	}

	template <class T>
	inline std::unique_ptr<proto_update> construct_protoupdate(INetworkAssociatedObject* object, std::uint32_t key, T& value, std::uint32_t list_id)
	{
		const void   * element_stream = reinterpret_cast<const void*>(&value);
		const size_t   element_size   = sizeof( T );

		std::unique_ptr<proto_update> result;
		result.reset( new proto_update( NETTIK::INetworkCodes::internal_evt_list_data ) );

		if (object != nullptr)
		{
			result->mutable_target_object()->CopyFrom(*object);
		}
		
		result->set_key(key);
		result->set_value(element_stream, element_size);
		result->set_list_id(list_id);

		result->SetReliable();

		return std::move(result);
	}

	inline std::unique_ptr<proto_remove> construct_protoremove(INetworkAssociatedObject* object, std::uint32_t key, std::uint32_t list_id)
	{
		std::unique_ptr<proto_remove> result;
		result.reset( new proto_remove( NETTIK::INetworkCodes::internal_evt_list_remove ) );

		if (object != nullptr)
		{
			result->mutable_target_object()->CopyFrom(*object);
		}
		
		result->set_key(key);
		result->set_list_id(list_id);

		result->SetReliable();

		return std::move(result);
	}

public:

	//-------------------------------------------
	// Item modification
	//-------------------------------------------
	// Creates a new entry into the internal map,
	// and schedules it for update to associated
	// peers. Returns new element.
	
	example_t* create(std::uint32_t element_key, example_t data)
	{
		mutex_guard guard( m_Mutex );

		auto existing_it = m_Data.find( element_key );
		if ( existing_it != m_Data.end() )
		{
			printf("key already exists in list.\n");
			return &existing_it->second;
		}

		m_Data.insert(std::make_pair(element_key, data));
		existing_it = m_Data.find( element_key );

		VirtualInstance  * pInstance = m_pParent->m_pInstance; // "wasteland"
		IEntityManager   * pManager  = m_pParent->m_pManager;   // "players"

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

	void remove_all()
	{
		mutex_guard guard( m_Mutex );

		auto data_copy = m_Data;
		for (auto it = data_copy.begin(); it != data_copy.end(); ++it)
		{
			printf("remove item.\n");
			remove(it->first);
		}
	}

	//-------------------------------------------
	// Removes an element using the specified
	// key. Returns false if the object failed 
	// to remove.
	//-------------------------------------------
	bool remove(std::uint32_t element_key)
	{
		mutex_guard guard( m_Mutex );

		auto existing_it = m_Data.find(element_key);
		if (existing_it == m_Data.end())
		{
			printf("warning: tried removing invalid key in networked list\n");
			return false;
		}

		m_Data.erase(existing_it);

		VirtualInstance  * pInstance = m_pParent->m_pInstance; // "wasteland"
		IEntityManager   * pManager  = m_pParent->m_pManager;  // "players"

		if (pManager == nullptr || pInstance == nullptr)
		{
			NETTIK_EXCEPTION("Tried sending contents of invalid object manager.");
		}

		INetworkAssociatedObject object_ref;
		object_ref.set_instance_name(pInstance->GetName()); // "wasteland"
		object_ref.set_manager_name(pManager->GetName());   // "players"
		object_ref.set_network_code(m_pParent->m_NetCode);  // 0

		auto packet = construct_protoremove(&object_ref, element_key, m_ListID);
		m_UpdateQueue.push_back(std::move(packet));

		return true;
	}

	//-------------------------------------------
	// Sets data using the specified key. Data
	// will be binary copied.
	//-------------------------------------------
	void set(std::uint32_t element_key, example_t data)
	{
		mutex_guard guard( m_Mutex );
	
		auto existing_it = m_Data.find( element_key );
		if ( existing_it == m_Data.end() )
		{
			printf("warning: tried setting invalid key in networked list\n");
			return;
		}

		// Do the standard copy into the map.
		m_Data[ element_key ] = data;

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
	
		auto packet = construct_protoupdate(&object_ref, element_key, data, m_ListID);
		m_UpdateQueue.push_back( std::move(packet) );
	}


	//-------------------------------------------
	// Returns an element with the associated key.
	//-------------------------------------------
	example_t* get(std::uint32_t element_key)
	{
		mutex_guard guard( m_Mutex );
		
		auto item_it = m_Data.find(element_key);
		if (item_it == m_Data.end())
		{
			return nullptr;
		}

		return &(item_it->second);
	}

	//-------------------------------------------
	// Sends the entire list's contents to
	// a peer, disregarding changes.
	//-------------------------------------------
	void SendContents(ENetPeer* pPeer)
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

	//-------------------------------------------
	// Schedules the entity manager's objects to
	// receive delta changes to the object.
	//-------------------------------------------
	void SendDelta(IEntityManager* pManager)
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
			}
		}
	}

protected:

	//-----------------------------------------------
	// Looks up an index and returns info about the
	// elements location in memory.
	//
	// Does not dispatch to RPC. Use internally.
	// Throws std::runtime_error if internal 
	// unordered_map::find returns end of list.
	//-----------------------------------------------
	data_point get_at(std::uint32_t index)
	{
		mutex_guard guard(m_Mutex);

		auto lookup_result = m_Data.find(index);

		if (lookup_result == m_Data.end())
		{
			throw std::runtime_error("lookup failed");
		}

		void* pItem = static_cast<void*>( &(*lookup_result).second );
		return std::make_pair(pItem, m_iDataSize);
	}

	//-----------------------------------------------
	// Builds a new element at the index using the 
	// datatype's default constructor. Returns info
	// about the new element's memory location, and
	// the size of the new element (sizeof example_t).
	//
	// Does not dispatch to RPC. Use internally.
	// Throws std::runtime_error if internal
	// unordered_map::insert fails.
	//-----------------------------------------------
	data_point build_at(std::uint32_t index)
	{
		mutex_guard guard( m_Mutex );

		// If the index already exists, just return that.
		auto lookup_result = m_Data.find(index);
		if ( lookup_result != m_Data.end() )
		{
			return std::make_pair(&lookup_result->second, sizeof(example_t));
		}

		// Build the element.
		auto insert_result = m_Data.insert(std::make_pair(index, example_t()));
		if (!insert_result.second)
		{
			throw std::runtime_error("insertation failed");
		}

		example_t* pItem = &(insert_result.first->second);
		return std::make_pair(pItem, sizeof(example_t));
	}

	//-----------------------------------------------
	// Removes an element at the specified index. 
	// 
	// Does not dispatch to RPC. Use internally.
	// Throws std::runtime_error if internal
	// unordered_map::find returns end of list.
	//
	// Standard list::erase, no explicit heap clean.
	//-----------------------------------------------
	
	void remove_at(std::uint32_t index)
	{
		mutex_guard guard( m_Mutex );

		auto lookup_result = m_Data.find(index);

		if ( lookup_result == m_Data.end() )
		{
			throw std::runtime_error("lookup failed");
		}

		m_Data.erase( lookup_result );
	}


	// Storage for all data elements.
	std::unordered_map<std::uint32_t, example_t> m_Data;

};