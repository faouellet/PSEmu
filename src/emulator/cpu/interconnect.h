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
    explicit Interconnect(BIOS bios);

public:
    uint8_t  LoadByte(uint32_t offset);
    uint16_t LoadHalfWord(uint32_t offset);
    uint32_t LoadWord(uint32_t offset);

    void StoreByte(uint32_t address, uint8_t value);
    void StoreHalfWord(uint32_t address, uint16_t value);
    void StoreWord(uint32_t address, uint32_t value);

private:
    BIOS m_bios;
    RAM m_ram;
    GPU m_gpu;
    DMA m_dma;
};

} // end namespace PSEmu

#endif // INTERCONNECT_H