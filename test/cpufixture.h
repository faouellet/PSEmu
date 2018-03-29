#ifndef CPU_FIXTURE
#define CPU_FIXTURE

#include <gtest/gtest.h>

#include "../src/emulator/cpu/r3000a.h"

class CPUFixture : public ::testing::Test
{
public:
    CPUFixture();

protected:
    PSEmu::R3000A m_cpu;
};

#endif // CPU_FIXTURE