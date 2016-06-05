#pragma once
#include <string>
#include <enet/enet.h>
#include "SnapshotStream.h"
#include "NetObject.h"

class IEntityManager
{
public:
	virtual void SetName(std::string name) = 0;
	virtual std::string GetName() const = 0;

	virtual NetObject* GetByNetID(uint32_t id) = 0;
	virtual NetObject* GetByPeerID(ENetPeer* id) = 0;
	virtual void PostUpdate(float elapsedTime) = 0;
	virtual void GetSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& buffers, bool bReliableFlag, bool bForced) = 0;

	//! Adds a self-maintaining object. Invokes the type specifier's constructor but
	//  operates in this scope.
	virtual NetObject* AddLocal(uint32_t netid, uint32_t controller = NET_CONTROLLER_NONE) = 0;

	//! Removes a self-maintaing object.
	virtual bool RemoveLocal(uint32_t entityCode) = 0;

	//! Adds a reference to the network queue. Must be of base class "NetObject".
	virtual uint32_t Add(NetObject* object) = 0;

	//! Removes a reference from the network queue. Doesn't destroy the pointer.
	virtual bool Remove(uint32_t entityCode) = 0;

	virtual ~IEntityManager() { }
};
