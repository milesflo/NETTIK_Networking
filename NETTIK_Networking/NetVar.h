#pragma once
#include <vector>

class NetObject;
class NetVar
{
protected:
	NetObject* m_pParent;
	const char* m_Name;

	bool        m_Reliable;

public:
	virtual size_t TakeVariableSnapshot(std::vector<unsigned char>& buffer, bool bForced) = 0;

	inline const char*  GetName()     const { return m_Name; }
	inline NetObject*  GetParent()    const { return m_pParent; }
	inline bool const   GetReliable() const { return m_Reliable; }

	NetVar(NetObject* parent, const char* name, bool reliable);

	virtual ~NetVar()
	{
//		m_pParent->m_Vars.erase(m_Name);
	}
};
