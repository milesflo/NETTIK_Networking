#include "CNetVarBufferedQuat.h"

CNetVarBufferedQuat::CNetVarBufferedQuat(NetObject* parent, const char* name, bool reliable)
	: NetVarBufferBase<gen_net::CQuaternion>(parent, name, reliable)
{

}

//--------------------------------------------------
// Calculates the interpolated value of the angles
// contents 
//--------------------------------------------------
gen_net::CQuaternion CNetVarBufferedQuat::GetInterpolated(std::chrono::milliseconds dtms)
{
	// If the vector hasn't received enough updates to 
	// interpolate, return the last vector to be stored.
	if (m_iChanges <= 1)
	{
		return GetLive();
	}
	
	// Calculate where the vector came from, and where the vector is heading.
	gen_net::CQuaternion& from = m_DataBuffers[ mod_wrap(m_ActivePositionIndex - 1, static_cast<unsigned int>(4)) ];
	gen_net::CQuaternion& to = m_DataBuffers[ m_ActivePositionIndex ];

	// Don't interpolate if the average time frame hasn't been adjusted.
	if (!m_bCalculatedFrameTime || m_fTimeFrame.count() == 0)
	{
		return to;
	}

	// Get current time and evaluate distance in time the current call
	// is from receiving the next vector update.
	m_fElaspedTime += (dtms / 2);

	// Calculate interpolated values from A to B (lerp)
	unsigned long long iDiff = m_fElaspedTime.count();
	float fDT = static_cast<float>( iDiff ) / static_cast<float>( m_fTimeFrame.count() );

	// If the timescale is still within bounds of the average step time, 
	// calculate the lerp value.
	if (fDT <= 1.0f)
	{
		gen_net::CQuaternion result;
		gen_net::Slerp(from, to, fDT, result);
		return result;
	}

	// If the server is out of sync, and hasn't sent any more data
	// since the average step time then just return the current
	// destination.
	return to;
}

//--------------------------------------------------
// Standard angle manipulator.
//--------------------------------------------------
void CNetVarBufferedQuat::Set(float w, float x, float y, float z)
{
	gen_net::CQuaternion compose(w, x, y, z);

	if (!SetGuard(compose))
	{
		return;
	}

	m_Data = compose;
}

void CNetVarBufferedQuat::Rotate(float w, float x, float y, float z)
{
	gen_net::CQuaternion rotquat(w, x, y, z);
	gen_net::CQuaternion pivot = (m_Data * rotquat);
	pivot.Normalise();

	if (!SetGuard(pivot))
	{
		return;
	}

	m_Data = pivot;
}