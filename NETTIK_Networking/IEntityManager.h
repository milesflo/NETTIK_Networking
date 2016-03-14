#pragma once
#include <string>
#include "SnapshotStream.h"

class IEntityManager
{
public:
	virtual void SetName(std::string name) = 0;
	virtual std::string GetName() const = 0;

	virtual void PostUpdate() = 0;
	virtual void GetSnapshot(size_t& max_value, uint16_t& num_updates, SnapshotStream& buffers, bool bReliableFlag, bool bForced) = 0;

	//! Adds a reference to the network queue. Must be of base class "NetObject".
	virtual uint32_t Add(void* object) = 0;

	//! Removes a reference from the network queue. Doesn't destroy the pointer.
	virtual bool Remove(uint32_t entityCode) = 0;

	virtual ~IEntityManager() { }
};
