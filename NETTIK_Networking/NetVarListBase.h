#pragma once

class NetVarListBase
{
protected:
	using data_point  = std::pair<void*, size_t>;
	using mutex_guard = std::lock_guard<std::recursive_mutex>;
	
	// Called when a remote peer inserts an item
	// into the list with a specific key. Also
	// performs initialization on the value.
	static void on_remote_add(ENetPeer* pPeer, INetworkMapAdd& packet);

	// Called when a remote peer updates an items
	// contents.
	static void on_remote_update(ENetPeer* pPeer, INetworkMapUpdate& packet);

	// Called when a remote peer deletes an item
	// from the list.
	static void on_remote_remove(ENetPeer* pPeer, INetworkMapRemove& packet);

	//-------------------------------------------
	// Finds a list object on an entity using
	// the index. Returns nullptr if no avail.
	//-------------------------------------------
	static NetVarListBase* FetchObjectList(NetObject* pNetworkObject, std::uint32_t index);

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

	virtual data_point get_at(std::uint32_t index)    = 0;
	
	virtual data_point build_at(std::uint32_t index)  = 0;
	
	virtual void remove_at(std::uint32_t index)       = 0;

	virtual void dispatch_add(std::uint32_t index)    = 0;

	virtual void dispatch_update(std::uint32_t index) = 0;

	virtual void dispatch_remove(std::uint32_t index) = 0;

public:

	//-----------------------------------------------
	// Internal processing of a list implementation.
	//-----------------------------------------------
	virtual void think() = 0;

	enum ListEvent
	{
		kListEvent_Add,
		kListEvent_Update,
		kListEvent_Remove,

		//-------------------------------------------------
		// Validates whether a list manipulation is
		// allowed to be dispatched, or whether it should
		// be re-queued.
		//-------------------------------------------------
		kListEvent_ScheduleCheck
	};

	using proto_add    = NETTIK::IPacketFactory::CProtoPacket<INetworkMapAdd>;
	using proto_update = NETTIK::IPacketFactory::CProtoPacket<INetworkMapUpdate>;
	using proto_remove = NETTIK::IPacketFactory::CProtoPacket<INetworkMapRemove>;

	virtual void SendContents(IEntityManager* pManager) = 0;

	virtual void SendContents(ENetPeer* pPeer)       = 0;

	virtual void SendDelta(IEntityManager* pManager) = 0;

	//---------------------------
	// Flushs the update queue
	//---------------------------
	void Flush();

	//-------------------------------------------
	// Adds event handlers to the passed controller
	// to route packets to their associated allocations/
	// deallocs/writes.
	//-------------------------------------------
	static void bind_controller(NETTIK::IController* pNetworkController);

protected:
	const size_t m_iDataSize;

	// Protection level
	std::recursive_mutex m_Mutex;

	// Standard netvar data.
	NetObject   * m_pParent = nullptr;
	const char  * m_psName;
	std::uint32_t m_ListID;

	// Cache update queue for items that need to be dispatched
	// to parent's peers.
	std::vector<std::unique_ptr<NETTIK::IPacketFactory::CBasePacket>> m_UpdateQueue;

	NetVarListBase(const size_t dataSize, const char* name, NetObject* pParent);
};