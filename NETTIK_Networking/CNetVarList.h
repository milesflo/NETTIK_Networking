#pragma once
#include "NetVarListBase.h"
#include <deque>
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

	using list_callback = std::function<bool(std::uint32_t, example_t*)>;

	CNetVarList(NetObject* parent, const char* name) : NetVarListBase(sizeof( example_t ), name, parent)
	{
		if (m_pParent == nullptr)
		{
			return;
		}

		bind(kListEvent_Remove, [=](uint32_t key, example_t* data)
		{
			VirtualInstance  * pInstance = m_pParent->m_pInstance; // "wasteland"
			IEntityManager   * pManager = m_pParent->m_pManager;  // "players"

			if (pManager == nullptr || pInstance == nullptr)
			{
				NETTIK_EXCEPTION("Tried sending contents of invalid object manager.");
			}

			std::vector<ENetPeer*> players;
			get_players(pManager, players);

			// Don't resend list data on client (subject to change with NPC sync.
			if (players.size() <= 1)
				return true;

			INetworkAssociatedObject object_ref;
			object_ref.set_instance_name(pInstance->GetName()); // "wasteland"
			object_ref.set_manager_name(pManager->GetName());   // "players"
			object_ref.set_network_code(m_pParent->m_NetCode);  // 0

			// Construct 
			auto packet = construct_protoremove(&object_ref, key, m_ListID);

			// Filter the updating player from having their update being re-sent.
			mutex_guard guard(m_Mutex);
			m_UpdateQueue.push_back(std::make_pair(std::move(packet), m_pParent->m_pPeer));

			return true;
		});
		
		bind(kListEvent_Add, [=](uint32_t key, example_t* data)
		{
			VirtualInstance  * pInstance = m_pParent->m_pInstance; // "wasteland"
			IEntityManager   * pManager = m_pParent->m_pManager;  // "players"

			if (pManager == nullptr || pInstance == nullptr)
			{
				NETTIK_EXCEPTION("Tried sending contents of invalid object manager.");
			}

			std::vector<ENetPeer*> players;
			get_players(pManager, players);

			// Don't resend list data on client (subject to change with NPC sync.
			if (players.size() <= 1)
				return true;

			INetworkAssociatedObject object_ref;
			object_ref.set_instance_name(pInstance->GetName()); // "wasteland"
			object_ref.set_manager_name(pManager->GetName());   // "players"
			object_ref.set_network_code(m_pParent->m_NetCode);  // 0

			// Construct 
			auto packet = construct_protoadd(&object_ref, key, m_Data[key], m_ListID);

			// Filter the updating player from having their update being re-sent.
			mutex_guard guard(m_Mutex);
			m_UpdateQueue.push_back(std::make_pair(std::move(packet), m_pParent->m_pPeer));

			return true;
		});

		bind(kListEvent_Update, [=](uint32_t key, example_t* data)
		{
			VirtualInstance  * pInstance = m_pParent->m_pInstance; // "wasteland"
			IEntityManager   * pManager = m_pParent->m_pManager;  // "players"

			if (pManager == nullptr || pInstance == nullptr)
			{
				NETTIK_EXCEPTION("Tried sending contents of invalid object manager.");
			}

			std::vector<ENetPeer*> players;
			get_players(pManager, players);

			// Don't resend list data on client (subject to change with NPC sync.
			if (players.size() <= 1)
				return true;

			INetworkAssociatedObject object_ref;
			object_ref.set_instance_name(pInstance->GetName()); // "wasteland"
			object_ref.set_manager_name(pManager->GetName());   // "players"
			object_ref.set_network_code(m_pParent->m_NetCode);  // 0

			// Construct 
			auto packet = construct_protoupdate(&object_ref, key, m_Data[key], m_ListID);

			// Filter the updating player from having their update being re-sent.
			mutex_guard guard(m_Mutex);
			m_UpdateQueue.push_back(std::make_pair(std::move(packet), m_pParent->m_pPeer));

			return true;
		});

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
		m_UpdateQueue.push_back( std::make_pair(std::move(packet), nullptr) );

		return (existing_it != m_Data.end() ? &existing_it->second : nullptr);
	}

	void remove_all()
	{
		mutex_guard guard( m_Mutex );

		auto data_copy = m_Data;
		for (auto it = data_copy.begin(); it != data_copy.end(); ++it)
		{
			remove(it->first);
		}
	}

	void clear()
	{
		mutex_guard guard( m_Mutex );

		while (!m_Data.empty())
		{
			remove(m_Data.begin());
		}
	}

	bool remove(std::uint32_t element_key)
	{
		auto existing_it = m_Data.find(element_key);
		if (existing_it == m_Data.end())
		{
			NETTIK::IController* pNetworkController = NETTIK::IController::GetSingleton();
			if (pNetworkController)
			{
				pNetworkController->GetQueue().Add(kMessageType_Warn, "Tried to remove invalid key from networked list.");
			}

			return false;
		}

		return remove(existing_it);
	}

	//-------------------------------------------
	// Removes an element using the specified
	// key. Returns false if the object failed 
	// to remove.
	//-------------------------------------------
	bool remove(typename std::unordered_map<std::uint32_t, example_t>::iterator existing_it)
	{
		mutex_guard guard( m_Mutex );

		std::uint32_t element_key = existing_it->first;
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
		m_UpdateQueue.push_back( std::make_pair(std::move(packet), nullptr) );

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
			NETTIK::IController* pNetworkController = NETTIK::IController::GetSingleton();
			if ( pNetworkController )
			{
				pNetworkController->GetQueue().Add(kMessageType_Warn, "Tried to setting invalid key from networked list.");
			}
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
		m_UpdateQueue.push_back( std::make_pair(std::move(packet), nullptr) );
	}

	//-------------------------------------------
	// Returns a copy of the list's contents.
	//-------------------------------------------
	std::unordered_map<std::uint32_t, example_t> get_list_copy()
	{
		mutex_guard guard( m_Mutex );

		return m_Data;
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
		// mutex_guard guard( m_Mutex );

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

	void SendContents(IEntityManager* pManager)
	{
		std::vector<ENetPeer*> pTargets;
		get_players(pManager, pTargets);

		for (auto it = pTargets.begin(); it != pTargets.end(); ++it)
		{
			SendContents(*it);
		}
	}

	//-------------------------------------------
	// Schedules the entity manager's objects to
	// receive delta changes to the object.
	//-------------------------------------------
	void SendDelta(IEntityManager* pManager)
	{
		std::vector<ENetPeer*> pTargets;
		get_players(pManager, pTargets);

		// Add all the peers to each update queue entry. Call flush afterwards to
		// invoke the dispatch of the packet data.
		// mutex_guard guard(m_Mutex);
		mutex_guard guard(m_Mutex);

		for (auto update_item = m_UpdateQueue.begin(); update_item != m_UpdateQueue.end(); ++update_item)
		{
			ENetPeer* pFilterPeer = update_item->second;
			for (auto peer = pTargets.begin(); peer != pTargets.end(); ++peer)
			{
				if (*peer != pFilterPeer)
				{
					(*update_item).first->AddPeer(*peer);
				}
			}
		}
	}

	void bind(ListEvent evt, list_callback callback)
	{
		if (m_Callbacks.find(evt) == m_Callbacks.end())
		{
			m_Callbacks[evt] = {};
		}

		m_Callbacks[evt].push_back( callback );
	}

	//-----------------------------------------------
	// Think needs to be ran frequently to recheck
	// local processing of remote events that could 
	// have been scheduled for a later time.
	//-----------------------------------------------
	void think()
	{
		dispatcher_think();
	}

	void invalidate_ents()
	{
		mutex_guard guard(m_Mutex);

		for (auto it = m_Data.begin(); it != m_Data.end(); ++it)
		{
			this->dispatch_add(it->first);
		}
		
	}

protected:

	void get_players(IEntityManager* pManager, std::vector<ENetPeer*>& target_list)
	{
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
					target_list.push_back((*object)->m_pPeer);
				}
			}

			object_list.safe_unlock();
		}
	}

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

		NETTIK::IController* pNetworkController = NETTIK::IController::GetSingleton();
		if ( pNetworkController )
		{
			pNetworkController->GetQueue().Add(kMessageType_Print, "build_at()");
		}

		if (!insert_result.second)
		{
			throw std::exception("insertation failed");
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

	bool needs_scheduling(std::uint32_t key, list_callback dest)
	{
		auto schedule_it = m_Callbacks.find(kListEvent_ScheduleCheck);
		if (schedule_it == m_Callbacks.end())
		{
			return false;
		}

		if (!schedule_it->second.at(0)(key, &m_Data[key]))
		{
			mutex_guard guard(m_QueuedEventsMutex);
			m_QueuedEvents.push_back({ key, dest });
			return true;
		}

		return false;
	}

	void dispatch_add(std::uint32_t key)
	{
		// Dispatch callback.
		auto callback_it = m_Callbacks.find(kListEvent_Add);

		if (callback_it != m_Callbacks.end())
		{
			auto& callback_list = callback_it->second;

			for (auto it = callback_list.begin(); it != callback_list.end(); ++it)
			{
				if (!needs_scheduling(key, (*it)))
				{
					(*it)(key, &m_Data[key]);
				}
			}
		}
	}
	
	void dispatch_update(std::uint32_t key)
	{
		// Dispatch callback.
		auto callback_it = m_Callbacks.find(kListEvent_Update);

		if (callback_it != m_Callbacks.end())
		{
			auto& callback_list = callback_it->second;

			for (auto it = callback_list.begin(); it != callback_list.end(); ++it)
			{
				if (!needs_scheduling(key, (*it)))
				{
					(*it)(key, &m_Data[key]);
				}
			}
		}
	}

	void dispatch_remove(std::uint32_t key)
	{
		// Dispatch callback.
		auto callback_it = m_Callbacks.find(kListEvent_Remove);

		if (callback_it != m_Callbacks.end())
		{
			auto& callback_list = callback_it->second;

			for (auto it = callback_list.begin(); it != callback_list.end(); ++it)
			{
				if (!needs_scheduling(key, (*it)))
				{
					(*it)(key, &m_Data[key]);
				}
			}
		}
	}

	void dispatcher_think()
	{
		// If the schedule event doesn't exist, nothing should 
		// really write to the queued events list as this is
		// invoked via a previous schedule check.
		auto schedule_it = m_Callbacks.find(kListEvent_ScheduleCheck);
		if (schedule_it == m_Callbacks.end())
		{
			return;
		}

		if (!schedule_it->second.at(0)(0, nullptr))
		{
			return;
		}

		mutex_guard guard(m_QueuedEventsMutex);

		for (auto it = m_QueuedEvents.begin(); it != m_QueuedEvents.end(); ++it)
		{
			it->dest( it->key, &m_Data[ it->key ] );
		}

		m_QueuedEvents.clear();
	}

	// Storage for all data elements.
	std::unordered_map<std::uint32_t, example_t> m_Data;
	std::unordered_map<ListEvent, std::vector<list_callback>> m_Callbacks;

	struct QueuedEvent
	{
		std::uint32_t key;
		list_callback dest;
	};

	std::recursive_mutex m_QueuedEventsMutex;
	std::deque<QueuedEvent> m_QueuedEvents;
};