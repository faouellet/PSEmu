#include "src/cpu/r3000a.h"
#include "src/memory/bios.h"

using namespace PSEmu;

int main()
{
    BIOS bios;
    bios.Init("SCPH1001.BIN");

    Interconnect interconnect{std::move(bios)};
    R3000A cpu{std::move(interconnect)};

    for(;;)
    {
        cpu.Step();
    }

    return 0;
}