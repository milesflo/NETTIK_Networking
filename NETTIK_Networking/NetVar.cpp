#include "NetVar.h"
#include "NetObject.h"
#include "IDebug.hpp"
using namespace NETTIK;

NetVar::NetVar(NetObject* parent, const char* name, bool reliable) : m_Name(name), m_pParent(parent), m_Reliable(reliable)
{
	if (m_pParent->m_Mutex.try_lock())
	{
		m_pParent->m_Vars[m_Name] = this;
		m_pParent->m_Mutex.unlock();
	}
	else
		NETTIK_EXCEPTION("Attempted to lock parent mutex on netvar but failed, inconsistent class tables.");
}

NetVar::~NetVar()
{
	if (m_pParent->m_Mutex.try_lock())
	{
		m_pParent->m_Vars.erase(m_Name);
		m_pParent->m_Mutex.unlock();
	}
}