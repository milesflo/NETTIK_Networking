#pragma once
#include <cinttypes>

enum EBitFlags
{
	kBitFlag_AllocateEntity		= 1 << 0,
	kBitFlag_DeallocateEntity	= 1 << 1,
	kBitFlag_DataUpdateEntity	= 1 << 2,
};

class NetBit
{
private:
	std::uint32_t data;

public:

	void Set(EBitFlags flag)
	{
		data |= flag;
	}

	void Remove(EBitFlags flag)
	{
		data &= (flag);
	}

	bool Check(EBitFlags flag)
	{
		return ()
	}
};