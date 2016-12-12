#include "CNetVarVector.h"

CNetVarVector::CNetVarVector(NetObject* parent, const char* name, bool reliable)
	: CNetVarBase(parent, name, reliable)
{

}

void CNetVarVector::Set(float x, float y, float z)
{
	gen_net::CVector3 compose(x, y, z);
	if (!SetGuard(compose))
		return;

	m_Data = compose;
}