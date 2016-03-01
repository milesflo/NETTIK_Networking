#pragma once
#include <vector>
#include "INetworkPacketFactory.hpp"
#include <string>

class INetObject
{
public:

	virtual ~INetObject() { };
};

class CEntities
{
private:
	std::vector<INetObject*> m_Objects;
	using object_it = std::vector<INetObject*>::iterator;

	std::string m_name;

public:

	void SetName(std::string name)
	{
		m_name = name;
	}
	
	std::string GetName()
	{
		return m_name;
	} const

	object_it StartSync(INetObject* object)
	{
		m_Objects.push_back(object);
		return m_Objects.end()--;
	}

	void StopSync(object_it it)
	{
		m_Objects.erase(it);
	}



	void Update(std::vector<NETTIK::IPacketFactory::INetworkPacket*>& queue)
	{

	}

	CEntities()
	{

	}
};