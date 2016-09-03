#include <gtest\gtest.h>
#include "NETTIK_Networking.hpp"

TEST(VarBase, Usage)
{
	CNetVarBase<int> m_intdata(nullptr, "m_intdata", true);
	m_intdata.Set(5);

	ASSERT_EQ(m_intdata.get(), 5);
}