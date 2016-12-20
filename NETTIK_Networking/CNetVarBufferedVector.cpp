#include "CNetVarBufferedVector.h"

// Minimum amount of interpolation distance, anything less
// will be directly applied.
const float  CNetVarBufferedVector::kMaxInterpolationDistance = 10.0f;

CNetVarBufferedVector::CNetVarBufferedVector(NetObject* parent, const char* name, bool reliable)
	: NetVarBufferBase<gen_net::CVector3>(parent, name, reliable)
{
}

//--------------------------------------------------
// Calculates the interpolated value of the vectors
// contents 
//--------------------------------------------------
gen_net::CVector3 CNetVarBufferedVector::GetInterpolated(std::chrono::milliseconds dtms)
{
	// If the vector hasn't received enough updates to 
	// interpolate, return the last vector to be stored.
	if (m_iChanges <= 1)
	{
		return GetLive();
	}

	// Calculate where the vector came from, and where the vector is heading.
	gen_net::CVector3& from = m_DataBuffers[ mod_wrap(m_ActivePositionIndex - 1, static_cast<unsigned int>(4)) ];
	gen_net::CVector3& to = m_DataBuffers[ m_ActivePositionIndex ];
	gen_net::CVector3 heading = (to - from);

	// Don't interpolate small values.
	if (heading.Length() > kMaxInterpolationDistance)
		return to;

	// Don't interpolate if the average time frame hasn't been adjusted.
	if (!m_bCalculatedFrameTime || m_fTimeFrame.count() == 0)
	{
		return to;
	}

	// Get current time and evaluate distance in time the current call
	// is from receiving the next vector update.
	std::chrono::milliseconds fCurrentTime = GetTime();
	m_fElaspedTime += (dtms / 2);

	// Calculate interpolated values from A to B (lerp)
	unsigned long long iDiff = m_fElaspedTime.count();
	float fDT = static_cast<float>( iDiff ) / static_cast<float>( m_fTimeFrame.count() );
	
	// If the timescale is still within bounds of the average step time, 
	// calculate the lerp value.
	if (fDT <= kMaxInterpolationScale)
	{
		return from + (heading * fDT);
	}

	// If the server is out of sync, and hasn't sent any more data
	// since the average step time then just return the current
	// destination.
	return to;
}

//--------------------------------------------------
// Standard vector manipulator.
//--------------------------------------------------
void CNetVarBufferedVector::Set(float x, float y, float z)
{
	gen_net::CVector3 compose(x, y, z);

	if (!SetGuard(compose))
	{
		return;
	}

	m_Data = compose;
}
