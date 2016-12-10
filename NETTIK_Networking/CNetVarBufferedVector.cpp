#include "CNetVarBufferedVector.h"

// Minimum amount of interpolation distance, anything less
// will be directly applied.
const float  CNetVarBufferedVector::kMaxInterpolationDistance = 10.0f;

CNetVarBufferedVector::CNetVarBufferedVector(NetObject* parent, const char* name, bool reliable)
	: CNetVarBase(parent, name, reliable)
{
}

//--------------------------------------------------
// Overloads the binary set to buffer invalidation to allow
// interpolation between each snapshot.
//--------------------------------------------------
void CNetVarBufferedVector::Set(unsigned char* ptr, size_t size, ENetPeer* pWho)
{
	if ( m_bLocked )
		return;

	if ( size == 0 )
		size = sizeof(CVector3);

	size_t index_new = mod_wrap(m_ActivePositionIndex + 1, static_cast<unsigned int>(4));

	float* vector_data = reinterpret_cast<float*>(ptr);
	m_VectorBuffer[index_new].Set(vector_data);
	m_ActivePositionIndex = index_new;

	m_iChanges++;
	m_bChanged = true;
		
	std::chrono::milliseconds fCurrentTime = GetTime();

	if ( m_bCalculatedPreviousFrameTime )
	{
		m_bCalculatedFrameTime = true;
		m_fTimeFrame = (fCurrentTime - m_fTimeFramePrevious);
	}

	m_bCalculatedPreviousFrameTime = true;
	m_fTimeFramePrevious = fCurrentTime;

	m_fElaspedTime = std::chrono::milliseconds(0);
}

//--------------------------------------------------
// Returns 1 always. Overrides the GetChanges 
// call in net variables. Buffered vectors always
// have changes to process.
//--------------------------------------------------
size_t CNetVarBufferedVector::GetChanges()
{
	return 1;
}

//--------------------------------------------------
// Returns the true most updated vector received
// from the server.
//--------------------------------------------------
CVector3 CNetVarBufferedVector::GetLastVector()
{
	return m_VectorBuffer[m_ActivePositionIndex];
}

//--------------------------------------------------
// Calculates the interpolated value of the vectors
// contents 
//--------------------------------------------------
CVector3 CNetVarBufferedVector::GetInterpolated(std::chrono::milliseconds dtms)
{
	// If the vector hasn't received enough updates to 
	// interpolate, return the last vector to be stored.
	if (m_iChanges <= 1)
	{
		return GetLastVector();
	}

	// Calculate where the vector came from, and where the vector is heading.
	CVector3& from = m_VectorBuffer[ mod_wrap(m_ActivePositionIndex - 1, static_cast<unsigned int>(4)) ];
	CVector3& to = m_VectorBuffer[ m_ActivePositionIndex ];
	CVector3 heading = (to - from);

	// Don't interpolate small values.
	if (heading.Length() > kMaxInterpolationDistance)
		return to;

	// Don't interpolate if the average time frame hasn't been adjusted.
	if (!m_bCalculatedFrameTime || m_fTimeFrame.count() == 0)
	{
		CMessageDispatcher::Add(kMessageType_Print, "Skipping interpolation...");
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
	if (fDT <= 1.0f)
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
	CVector3 compose(x, y, z);

	if (!SetGuard(compose))
	{
		return;
	}

	m_Data = compose;
}

//--------------------------------------------------
// Internal helper function for returning the 
// current system time.
//--------------------------------------------------
std::chrono::milliseconds CNetVarBufferedVector::GetTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
}
