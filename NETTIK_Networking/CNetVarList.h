#pragma once
#include <unordered_map>
#include <vector>
#define NetworkList( name )  CNetVarList name ={ this, #name }

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

	CNetVarList(NetObject* parent, const char* name);

	virtual ~CNetVarList();

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
	// Protection level
	std::mutex m_Mutex;

	// Standard netvar data.
	NetObject   * m_pParent = nullptr;
	const char  * m_psName;

	// Storage for all data elements.
	std::unordered_map<std::uint32_t, example_t> m_Data;

	// Cache update queue for items that need to be dispatched
	// to parent's peers.
	std::vector<std::unique_ptr<NETTIK::IPacketFactory::CBasePacket>> m_UpdateQueue;
};