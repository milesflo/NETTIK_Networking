#pragma once
#include <unordered_map>
#include <vector>
#define NetworkList( name )  CNetVarList name = { this, #name }

//------------------------------------------
// A dynamic list that updates its contents 
// across the network to associated peers. 
// 
// Similar structure to the NetVar class, 
// but controls data access to dispatch
// updates via RPC and not via snapshot.
//------------------------------------------
class CNetVarList
{
public:
	// change when ready for template initialization.
	using example_t   = int;
	using mutex_guard = std::lock_guard<std::mutex>;

	using proto_add    = NETTIK::IPacketFactory::CProtoPacket<INetworkMapAdd>;
	using proto_update = NETTIK::IPacketFactory::CProtoPacket<INetworkMapUpdate>;
	using proto_remove = NETTIK::IPacketFactory::CProtoPacket<INetworkMapRemove>;

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


	CNetVarList(NetObject* parent, const char* name);

	virtual ~CNetVarList();

	//-------------------------------------------
	// Static callback handlers
	//-------------------------------------------
	// Called when a remote peer invokes an 
	// item to be created (with data).

private:
	//-------------------------------------------
	// Finds a list object on an entity using
	// the index. Returns nullptr if no avail.
	//-------------------------------------------
	static CNetVarList* FetchObjectList(NetObject* pNetworkObject, std::uint32_t index);
	
	//-------------------------------------------
	// Checks to see if the current network 
	// controller has write access to an object.
	//
	// Server has full control.
	//-------------------------------------------
	static bool HasWritePermission(ENetPeer* pCallee, NetObject* pCheckObject);

	//-------------------------------------------
	// Converts a network associated object to
	// it's literal memory location. Throws
	// exceptions on lookup errors, returns
	// nullptr if object cannot be found.
	//-------------------------------------------
	static NetObject* DecodeObject(const INetworkAssociatedObject& associated_object);

public:
	static void on_remote_add( ENetPeer* pPeer, INetworkMapAdd& packet );
	
	// Called when a remote peer updates an items
	// contents.
	static void on_remote_update( ENetPeer* pPeer, INetworkMapUpdate& packet );

	// Called when a remote peer deletes an item
	// from the list.
	static void on_remote_remove( ENetPeer* pPeer, INetworkMapRemove& packet );

	//-------------------------------------------
	// Item modification
	//-------------------------------------------
	// Creates a new entry into the internal map,
	// and schedules it for update to associated
	// peers. Returns new element.
	example_t* create(std::uint32_t element_key, example_t data);

	// Returns an element with the associated key.
	example_t* get(std::uint32_t element_key);

	// Removes an element by the associated key.
	void remove(std::uint32_t element_key);

	//-------------------------------------------
	// Sends the entire list's contents to
	// a peer, disregarding changes.
	//-------------------------------------------
	void SendContents(ENetPeer* pPeer);

	//-------------------------------------------
	// Schedules the entity manager's objects to
	// receive delta changes to the object.
	//-------------------------------------------
	void SendDelta(IEntityManager* pManager);

	//---------------------------
	// Flushs the update queue
	//---------------------------
	void Flush();

private:

	using data_point = std::pair<void*, size_t>;

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
	// Looks up an index and returns info about the
	// elements location in memory.
	//
	// Does not dispatch to RPC. Use internally.
	// Throws std::runtime_error if internal 
	// unordered_map::find returns end of list.
	//-----------------------------------------------
	data_point get_at(std::uint32_t index)
	{
		auto lookup_result = m_Data.find( index );

		if ( lookup_result == m_Data.end() )
		{
			throw std::runtime_error("lookup failed");
		}

		example_t* pItem = &(*lookup_result).second;
		return std::make_pair(pItem, sizeof(example_t));
	}


	// Protection level
	std::mutex m_Mutex;

	// Standard netvar data.
	NetObject   * m_pParent = nullptr;
	const char  * m_psName;
	std::uint32_t m_ListID;

	// Storage for all data elements.
	std::unordered_map<std::uint32_t, example_t> m_Data;

	// Cache update queue for items that need to be dispatched
	// to parent's peers.
	std::vector<std::unique_ptr<NETTIK::IPacketFactory::CBasePacket>> m_UpdateQueue;
};