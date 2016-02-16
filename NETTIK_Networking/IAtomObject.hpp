#pragma once
#include <inttypes.h>


namespace NETTIK
{
	class IAtomObject
	{
	private:
		uint32_t m_ID;
		bool     m_bAllocated;
		bool     m_bChanged;

	public:
		//! Is the object encapsulated in a controller?
		bool IsAllocated() const { return m_bAllocated; }
		void SetAllocated(const bool val) { m_bAllocated = val; }

		IAtomObject();
		~IAtomObject();

		virtual void Update(const bool replicating)
		{

		}
	};

}