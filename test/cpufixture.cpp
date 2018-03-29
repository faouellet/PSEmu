#include "cpufixture.h"

CPUFixture::CPUFixture() : m_cpu{PSEmu::Interconnect{PSEmu::BIOS{}}, PSEmu::Debugger{}} {}