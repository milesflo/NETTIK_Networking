#pragma once
#include "NetVar.h"
#include "Constraints.h"
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
	VarType*     m_Data = nullptr;

	bool SetGuard(VarType& data)
	{
		if (!m_pParent->m_Active)
			return false;

		if (m_Data == nullptr)
			return false;

		if (*m_Data == data)
			return false;
		m_bChanged = true;

		return true;
	}

public:

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

		if (!m_Data)
			return 0;

		// This is slow AF.
		// TODO: Pass character buffer and fill and update
		// an index of the current stream length.
		uint32_t code;
		code = NETID_Reserved::RTTI_Object::OBJECT_DAT;

		SnapshotStream::Stream& buffer = buffers.create();

		// | network id |
		for (size_t i = 0; i < sizeof(uint32_t); i++)
			buffer.push_back(((unsigned char*)(&m_pParent->m_NetCode))[i]);

		// | varname |
		for (size_t i = 0; i < strlen(m_Name); i++)
			buffer.push_back((((m_Name)[i])));
		buffer.push_back(0); // NULL terminate

		// | data buffer ....
		for (size_t i = 0; i < sizeof(VarType); i++)
			buffer.push_back(((unsigned char*)(m_Data))[i]);

		m_bChanged = false;
		return buffer.size();
	}

	CNetVarBase(NetObject* parent, const char* name, bool reliable) : NetVar(parent, name, reliable)
	{
		m_Data = new VarType();
		*m_Data = VarType();
	}

	virtual ~CNetVarBase()
	{
		delete(m_Data);
		m_Data = nullptr;
	}

	virtual void Set(VarType data)
	{
		if (!SetGuard(data))
			return;

		*m_Data = data;
	}
};
