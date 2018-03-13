#include "interconnect.h"

using namespace PSEmu;

Interconnect::Interconnect(BIOS bios) : m_bios{ std::move(bios) }, m_ram{}, m_gpu{}, m_dma{ m_gpu, m_ram } { }

// TODO: Document
uint32_t Interconnect::GetPhysicalAddress(uint32_t virtAddr)
{
    return virtAddr & REGION_MASK[virtAddr >> 29];
}
