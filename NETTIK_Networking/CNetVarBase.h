#pragma once
#include "NetVar.h"
#include "Constraints.h"
#include "SnapshotEntList.h"
#define DEFINE_NetVar(type, name, reliable) \
	CNetVarBase<type> name = CNetVarBase<type>(this, #name, reliable)

//! A templated class for representing a fixed data type (non-pointer)
//  network variable.
template< class VarType >
class CNetVarBase : public NetVar
{
	static_assert(sizeof(VarType) < max_entvar_data, "Sizeof type templated to CNetVarBase is too large.");

private:
	bool        m_bChanged = true;

protected:
	VarType     m_Data;

	bool SetGuard(VarType& data)
	{
		if (!m_pParent->m_Active)
			return false;

		if (m_Data == data)
			return false;

		m_bChanged = true;

		return true;
	}

public:

	inline VarType& get()
	{
		return m_Data;
	}

	CNetVarBase(const CNetVarBase& from)
	{
		m_Name = from.m_Name;
		m_bChanged = from.m_bChanged;
	}

	size_t TakeVariableSnapshot(SnapshotStream& buffers, bool bForced)
	{
		// Variable hasn't changed and snapshot isn't forced.
		if (!bForced && !m_bChanged)
			return 0;

		// This is slow AF.
		// TODO: Pass character buffer and fill and update
		// an index of the current stream length.
		uint32_t code;
		code = NETID_Reserved::RTTI_Object::OBJECT_FRAME;

		SnapshotStream::Stream& buffer = buffers.create();

		SnapshotEntList generator;
		generator.set_frametype(FrameType::kFRAME_Data);
		generator.set_name(m_Name);
		generator.set_netid(m_pParent->m_NetCode);
		generator.set_data(reinterpret_cast<unsigned char*>(&m_Data), sizeof(VarType));
		generator.write(buffer);

		m_bChanged = false;
		return buffer.size();
	}

	CNetVarBase(NetObject* parent, const char* name, bool reliable) : NetVar(parent, name, reliable)
	{
	}

	virtual ~CNetVarBase()
	{

	}

	void Set(unsigned char* ptr, size_t size)
	{
		if (size == 0)
			size = sizeof(VarType);

		unsigned char* data;
		data = reinterpret_cast<unsigned char*>(&m_Data);

		printf("size = %d | my size = %d\n", size, sizeof(VarType));
		//for (size_t i = 0; i < size; i++)
		//	(&m_Data)[i] = stream[i];
		for (size_t i = 0; i < size; i++)
			data[i] = ptr[i];
	}

	void Set(VarType data)
	{
		if (!SetGuard(data))
			return;

		m_Data = data;
	}
};
