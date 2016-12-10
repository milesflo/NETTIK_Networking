//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <string>
#include <enet/enet.h>
#include "SnapshotStream.h"
#include "NetObject.h"
#include "LockableVector.h"

//-------------------------------------------------
// IEntityManager: An interface to manage the
// contents of networked entity lists.
//-------------------------------------------------
class IEntityManager
{
public:
	template <class T>
	using EventCallback = std::function<void(T*)>;

	//-------------------------------------------------
	// Accessors for the entity manager name.
	//-------------------------------------------------
	void SetName(std::string name);
	const std::string& GetName() const;

	//-------------------------------------------------
	// Accessors to get object references inside the
	// entity list.
	//-------------------------------------------------
	virtual std::shared_ptr<NetObject> GetByNetID(NetObject::NetID id) = 0;
	virtual std::shared_ptr<NetObject> GetByPeerID(ENetPeer* peer)     = 0;

	//-------------------------------------------------
	// Called when the entities receive a list update.
	//-------------------------------------------------
	virtual void PostUpdate(float elapsedTime)    = 0;

	//-------------------------------------------------
	// Processes a snapshot buffer of the entity list.
	// Forcing this will take a full snapshot, the
	// reliable flag will poll for reliable data.
	//-------------------------------------------------
	virtual void GetSnapshot
	(
		std::size_t&    max_value,
		std::uint16_t&  num_updates,
		SnapshotStream& buffers,
		bool bReliableFlag,
		bool bForced
	) = 0;

	//-------------------------------------------------
	// Adds a self-maintaining object. Invokes the type
	// specifier's constructor but operates in this
	// scope.
	//-------------------------------------------------
	virtual std::shared_ptr<NetObject> BuildLocal(NetObject::NetID netid, uint32_t controller = NET_CONTROLLER_NONE) = 0;

	//-------------------------------------------------
	// Removes a self-maintaining object.
	//-------------------------------------------------
	virtual void FreeLocal(NetObject::NetID netid) = 0;

	//-------------------------------------------------
	// Removes a self-maintaing object.
	//-------------------------------------------------
	// virtual bool RemoveLocal(uint32_t entityCode) = 0;

	//-------------------------------------------------
	// Adds a reference to the network queue.
	// Must be of base class "NetObject".
	//-------------------------------------------------
	// virtual uint32_t Add(NetObject* object)       = 0;

	//-------------------------------------------------
	// Removes a reference from the network queue.
	// Doesn't destroy the pointer.
	//-------------------------------------------------
	virtual bool FreeEntity(NetObject* pTarget)      = 0;

	//-------------------------------------------------
	// Counts the internal entity lists.
	//-------------------------------------------------
	size_t Count();
	virtual size_t CountMaintained() = 0;

	virtual ~IEntityManager();

	//-------------------------------------------------
	// Returns a mutex locked vector of the internal
	// objects.
	//-------------------------------------------------
	LockableVector<std::shared_ptr<NetObject>>& GetObjects();

	//-------------------------------------------------
	// Gets the next UID of the application. Increments
	// the next ID on call.
	//-------------------------------------------------
	std::uint32_t GetNextID();

	virtual void SendObjectLists(ENetPeer* pOwner) = 0;

protected:
	//-------------------------------------------------
	// Object list
	//-------------------------------------------------
	// LockableVector<NetObject*>  m_Objects;
	LockableVector<std::shared_ptr<NetObject>> m_NetworkObjects; // NET2
	std::string m_ManagerName;
};
