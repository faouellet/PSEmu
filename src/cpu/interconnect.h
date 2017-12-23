#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include "../memory/bios.h"
#include "../memory/dma.h"
#include "../memory/ram.h"
#include "../video/gpu.h"

namespace PSEmu
{

class Interconnect
{
public:
    explicit Interconnect(BIOS&& bios);

public:
    Utils::UInt8  LoadByte(Utils::UInt32 offset);
    Utils::UInt16 LoadHalfWord(Utils::UInt32 offset);
    Utils::UInt32 LoadWord(Utils::UInt32 offset);

    void StoreByte(Utils::UInt32 address, Utils::UInt8 value);
    void StoreHalfWord(Utils::UInt32 address, Utils::UInt16 value);
    void StoreWord(Utils::UInt32 address, Utils::UInt32 value);

private:
    BIOS m_bios;
    RAM m_ram;
    DMA m_dma;
    GPU m_gpu;
};

} // end namespace PSEmu

#endif // INTERCONNECT_H