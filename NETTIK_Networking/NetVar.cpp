//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------

NetVar::NetVar(NetObject* parent, const char* name, bool reliable) : m_Name(name), m_pParent(parent), m_Reliable(reliable)
{
	if (m_pParent == nullptr)
	{
		return;
	}

	if (m_pParent->m_Mutex.try_lock())
	{
		NetObject::VariableList_t& vars = m_pParent->GetVariables();
		vars.push_back(this);

		m_pParent->m_Mutex.unlock();
	}
	else
	{
		NETTIK_EXCEPTION("Attempted to lock parent mutex on netvar but failed, inconsistent class tables.");
	}
}

void NetVar::SetSequenceCheck(bool bSequenceCheck)
{
	m_CheckSequence = bSequenceCheck;
}

bool NetVar::CheckSequence(uint32_t sequence_id) const
{
	return !m_CheckSequence || (m_iSequenceID <= sequence_id);
}

void NetVar::SetSequence(uint32_t sequence_id)
{
	m_iSequenceID = sequence_id;
}

uint32_t NetVar::GetSequence() const
{
	return m_iSequenceID;
}

NetVar::~NetVar()
{
	if (m_pParent != nullptr && m_pParent->m_Mutex.try_lock())
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