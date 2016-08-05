//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#include "NetVar.h"
#include "NetObject.h"
#include "IDebug.h"
using namespace NETTIK;

NetVar::NetVar(NetObject* parent, const char* name, bool reliable) : m_Name(name), m_pParent(parent), m_Reliable(reliable)
{
	if (m_pParent->m_Mutex.try_lock())
	{
		NetObject::VariableList_t& vars = m_pParent->GetVariables();
		vars.push_back(this);

		m_pParent->m_Mutex.unlock();
	}
	else
		NETTIK_EXCEPTION("Attempted to lock parent mutex on netvar but failed, inconsistent class tables.");
}

NetVar::~NetVar()
{
	if (m_pParent->m_Mutex.try_lock())
	{
		NetObject::VariableList_t& vars = m_pParent->GetVariables();
		
		auto it = std::find(vars.begin(), vars.end(), this);
		if (it != vars.end())
		{
			vars.erase(it);
		}

		m_pParent->m_Mutex.unlock();
	}
}