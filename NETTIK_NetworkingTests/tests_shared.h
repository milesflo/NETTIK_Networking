#pragma once
#include "NETTIK_Networking.hpp"

class CEntity : public NetObject
{
public:
	DEFINE_NETOBJECT("CEntity");

	CNetVar(int, test_int, true);
	CNetVar(float, test_float, false);

	void Update()
	{
	}

	CEntity() : NetObject()
	{

	}

	virtual ~CEntity()
	{

	}
};