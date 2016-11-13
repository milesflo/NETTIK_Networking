# NETTIK (KITTEN) Networking
NETTIK is an object oriented networking engine with the purpose of allowing replicated environments in game development. It's hierarchical structure allows for virtual instances, and entity ownership for clients to replicate properties to other peers.

This library is based on snapshotting and data invalidation. You supply variable members to classes that inherit the base NetObject class. On each tick, the server will check for invalidations and send these changes to connected peers. Clients can do the exact same, but the server must give the client ownership.

In an FPS example, the server should allocate a player to each new connection and give them ownership. The client can check for ownership, and once it's controllable player they can update the network variables (position, rotation, velocity) and dispatch them to the server. The server will validate changes and rebroadcast them to the other connected peers.

The library is a core component in [NV:MP](https://nv-mp.com/forum/index.php), a modification of Fallout: New Vegas which makes the game multiplayer. As with NV:MP, this project is also under development, unfinished and experimental. 

## Compiling
NETTIK uses [ENet](http://enet.bespin.org/) for transport of packets. ENet is already included in the project with compiled binaries under lib/ and include/. [Protobuf](https://github.com/google/protobuf) is also used for reliable network packets, although objects should use the NetVar attributes for object synch. The library comes with compiled binaries already in the repository. If you need to rebuild the solution, there are dynamic and static configurations to target your implementation.

## Usage and Documentation
You should inherit IControllerServer or IControllerClient abstract classes to implement the update routines. NETTIK is multithreaded and is (generally) safe to interface with from other threads. The NetVar classes are thread-safe.

Use the NetworkVariable, NetworkUnicodeString or NetworkVector macros to define your object attributes. Make sure these aren't private as the base object needs to keep track of state changes internally.

### IControllerServer
TODO

### IControllerClient
TODO


### Macro NetworkVariable( type, name, reliable_flag )
Type can be any struct or class that needs its data to be networked. Objects inside a network variable should not be large as a whole binary copy through the network is done on each change. The name parameter is the name for the member, no prefix or suffix is applied. Setting the reliable flag to true makes changes done to the variable reliably deliver to connected peers in sequences. Specify false for data that doesn't need to arrive reliably (such as positional data).

#### m_NetVar.Set( data );
Sets the component data and invalidates it for the next snapshot iteration. If the data is exactly the same (via a binary comparison check), it will not invalidate.

#### m_nNetVar.GetChanges(); / m_nNetVar.ClearChanges()
Returns how many changes have happened since the last ClearChanges(). Use this in update routines to check for changes on variables. Call ClearChanges() to mark it as "processed".

#### m_nNetVar.Lock() / m_nNetVar.Unlock();
Prevents **any** remote changes to the variable. Useful if you want to give a client ownership of the entity, but don't want them to change a specific attribute (such as login session, guidd, etc...)

#### m_nNetVar.Invalidate();
Invalidates the contents and sends the variable to connected clients again.

#### VarType& m_nNetVar.get();
Returns the mutable variable stored in the netvar. Changes done to this won't be resynced unless a Set call is made or Invalidate is explicitly invoked after the change.

#### void* _get();
Returns a pointer to the underlying data.

#### const char* GetName();
Returns the variable name (used as the member name).

#### NetObject* GetParent();
Returns the object parent.

#### bool GetReliable();
Returns if the variable is reliable.

### Macro NetworkString / NetworkUnicodeString( name, size )
Extends the network variable class to string data. The size is the amount of data the string can hold (with a NULL char). All string variables are reliable.

### Macro NetworkVector( name, reliable )
Extends the network variable class to vector data. Implements the x, y, z components to the Set call. Returns its underlying structure of NETTIK::CVector3. 

### Macro NetworkList( element_type, name )
Extends the network variable class to an associative array. Can dynamically store elements with a key. The list supports events for handling remote changes, so GetChanges and ClearChanges() are obsolete. All lists are reliable.

#### List Events
* kListEvent_Add - Called when a new element is added to the list
* kListEvent_Update - Called when an element's data is modified.
* kListEvent_Remote - Called when an element is about to be removed from the list.

List callbacks use the template list_callback, which takes a function that returns a boolean and takes parameters: uint_32 (key), List_Element* (value).

#### bind( ListEvent evt, list_callback callback)
Binds a new events to this list. Multiple events can can allocated to a list event. Event procedures will be called from the network process thread.

#### List_Element* create( uint32 key, List_Element value );
Creates a new element on the list with the specified key. If the key already exists, it will just return the existing item and not change its contents to the value.

#### bool remove(uint32_t key);
Removes an element using the specified key. Returns false if the object failed to remove.

#### void set(uint32_t key, List_Element value);
Updates a key with the new data passed. Will be binary copied, don't expect a deep copy to happen as it is unsupported.

### class NetObject
Network objects should be extended to implement processing and extra data variables. Using the NetworkVariable macro in these definitions will dynamically allocate a new variable entry to be networked with this entity. 

#### bool IsActive();
Returns if the entity is active for snapshotting (useful for disabling objects for optimisation.)

#### bool IsServer();
Returns if the entity is running under a server instance.

#### virtual void NetworkUpdate(ReplicationInfo& repinfo);
A virtual call for updating the network object. The deriving class should implement this as a multiplayer update procedure. Replication info specifies if the entity should broadcast (has ownership), should replicate (should process incoming changes) and the elapsed time in milliseconds since the last snapshot.

#### virtual void Initialize();
A virtual call for notifying when the object is ready for network synch. This can be optionally implemented to properly dispatch network requests when the controller prepares the object.

#### std::string GetPeerHost();
Returns a formatted string specifying the ENet peer owners's IP and host information.

#### IsNetworkLocal();
Is the object owned by the local peer. (CLIENT: we control the peer, but the server may send changes too.)

#### IsNetworkRemote();
Is the object remotely controlled (CLIENT: we don't know the owner, but we assume the server will tell us information.)

#### DestroyNetworkedEntity();
Bubbles a request up to the manager to delete the object. You shouldn't call this explicitly, the destructor will invoke this. Anything referencing this entity will make all pointers dangling.

#### InvalidateVars();
Flags all variables to be re-transmitted to all associated peers.

#### VariableList_t& GetVariables();
Returns a list of all variable objects on the object.

#### GetMaps()
Returns a list of variable objects that are maps/lists.

#### UpdateListDelta()
An internal function for processing variable list deltas. Shouldn't be called explicitly.

#### SendLists(ENetPeer* pPeer)
An internal function for sending all variable maps/lists to a peer regardless of deltas.

#### uint32_t m_NetCode
The unique identifier that this object represents as.

#### uint32_t m_RealmID
If the object is setup using a CPlayerRealm, this ID will be allocated as a player ID. This is not unique and is allocated per free ID (starting from 0 inclusive).

#### m_Controller
Either specified as:
* NET_CONTROLLER_NONE - Doesn't have a controller visible by this peer.
* NET_CONTROLLER_LOCAL - This peer has been assigned to control this object.

On the server, this is ignored and shouldn't be checked as the server has full authoritative control over each object. To check for ownership, use m_pPeer tp check a peer's ownership.

#### m_pPeer
The ENet peer responsible for controlling this entity.

#### m_pInstance
The instance the entity is registered to.

#### m_pManager
The manager interface that owned this object.