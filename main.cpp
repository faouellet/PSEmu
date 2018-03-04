#include "src/emulator/cpu/r3000a.h"
#include "src/emulator/memory/bios.h"

#include <iostream>

using namespace PSEmu;

int main()
{
    BIOS bios;
    bios.Init("SCPH1001.BIN");

    Interconnect interconnect{std::move(bios)};
    Debugger debugger;
    R3000A cpu{std::move(interconnect), std::move(debugger)};

    for(;;)
    {
        cpu.Step();
    }

    return 0;
}