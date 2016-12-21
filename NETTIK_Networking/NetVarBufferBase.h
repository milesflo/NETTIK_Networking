#pragma once
#include "CNetVarBase.h"
#include <chrono>

//--------------------------------------------------
// Distance interpolation should scale to before
// waiting for next network update. 
// 1.0f is resonable, but packet speed can be 
// variable, so increasing this can predict the next
// location.
//--------------------------------------------------
extern float kMaxInterpolationScale;
extern float kMaxInterpolationBandingDist;

//--------------------------------------------------
// NetVarBufferBase: Generic class for interpolated
// variable data. VarType must be a variable that
// supports void Set(float* psData);
//--------------------------------------------------
template <class VarType>
class NetVarBufferBase : public CNetVarBase< VarType >
{
public:
	NetVarBufferBase()
	{
		for (size_t i = 0; i < 4; ++i)
		{
			m_DataBuffers[i] = { 0.0f, 0.0f, 0.0f };
		}
	}

	virtual VarType GetInterpolated(std::chrono::milliseconds dtms) = 0;

	//--------------------------------------------------
	// Overloads the binary set to buffer invalidation to allow
	// interpolation between each snapshot.
	//--------------------------------------------------
	void Set(unsigned char* ptr, size_t size, ENetPeer* pWho)
	{
		if ( m_bLocked )
			return;

		if ( size == 0 )
			size = sizeof( VarType );

		size_t index_new = mod_wrap(m_ActivePositionIndex + 1, static_cast<unsigned int>(4));

		float* vector_data = reinterpret_cast<float*>(ptr);
		m_iChanges ++;
		m_bChanged = true;

		// No interpolation on replication side.
		if (m_pParent->IsNetworkLocal())
		{
			m_Data.Set(vector_data);
			return;
		}

		VarType renderPoint = GetInterpolated(std::chrono::milliseconds());
		m_DataBuffers[index_new].Set(vector_data);

		// Set the previous buffer to the current location to cause
		// rubber-banding to stop stutter.

		const VarType& currentPoint = m_DataBuffers[index_new];
		float fInterpolationDist = renderPoint.DistanceTo( currentPoint );
		if (fInterpolationDist <= kMaxInterpolationBandingDist)
		{
			m_DataBuffers[ m_ActivePositionIndex ] = renderPoint;
		}
		else
		{
			CMessageDispatcher::Add(kMessageType_Warn, "%s Dropping interpolation banding due to distance of %f", typeid(VarType).name(), fInterpolationDist);
		}

		m_ActivePositionIndex = index_new;

		//std::chrono::milliseconds fCurrentTime = GetTime();

		if ( m_bCalculatedPreviousFrameTime )
		{
			m_bCalculatedFrameTime = true;
			//m_fTimeFrame = (fCurrentTime - m_fTimeFramePrevious);
			m_fTimeFrame = std::chrono::milliseconds( NetSystem::GetSingleton()->GetNetworkRate() );
		}

		m_bCalculatedPreviousFrameTime = true;
		//m_fTimeFramePrevious = fCurrentTime;

		m_fElaspedTime = std::chrono::milliseconds(0);
	}

	void Set(VarType data)
	{
		if (!SetGuard(data))
		{
			return;
		}

		m_Data = data;
	}

	//--------------------------------------------------
	// Returns 1 always. Overrides the GetChanges 
	// call in net variables. Buffered vectors always
	// have changes to process.
	//--------------------------------------------------
	size_t GetChanges()
	{
		return 1;
	}

	//--------------------------------------------------
	// Returns the true most updated vector received
	// from the server.
	//--------------------------------------------------
	VarType& get()
	{
		if (m_pParent->IsNetworkLocal())
		{
			return m_Data;
		}
		else
		{
			return m_DataBuffers[m_ActivePositionIndex];
		}
	}

	VarType GetLive()
	{
		return get();
	}

protected:
	//--------------------------------------------------
	// Internal helper function for returning the 
	// current system time.
	//--------------------------------------------------
	std::chrono::milliseconds GetTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
	}

	//--------------------------------------------------
	// Internal helper function for wrapping a value
	// and supporting negative wrapping.
	//--------------------------------------------------
	template <class T>
	T mod_wrap(T a, T b)
	{
		return (a % b + b) % b;
	}

	NetVarBufferBase(NetObject* parent, const char* name, bool reliable) :
		CNetVarBase< VarType >(parent, name, reliable)
	{
		
	}

	std::chrono::milliseconds m_fElaspedTime;       // Elasped time to move interpolated value by.
	std::chrono::milliseconds m_fTimeFrame;         // The average time in milliseconds the vector expects an update to its contents (total frametime)
	std::chrono::milliseconds m_fTimeFramePrevious; // Timestamp of previous update function.
	
	// Interpolation buffers.
	VarType  m_DataBuffers[4];
	size_t   m_ActivePositionIndex = 0;

	bool m_bCalculatedFrameTime = false;
	bool m_bCalculatedPreviousFrameTime = false;
};