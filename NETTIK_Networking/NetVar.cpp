#include "NetVar.h"
#include "NetObject.h"

NetVar::NetVar(NetObject* parent, const char* name, bool reliable) : m_Name(name), m_pParent(parent), m_Reliable(reliable)
{
	m_pParent->m_Vars[m_Name] = this;
}
