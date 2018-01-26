#ifndef MEMORY_CONSTANTS_H
#define MEMORY_CONSTANTS_H

#include "../utils/range.h"

#include <array>
#include <cstdint>

namespace PSEmu
{

// Reserved addresses
extern const uint32_t BIOS_ADDRESS;
extern const uint32_t EXPANSION_MAPPING_ADDRESS;
extern const uint32_t RAM_SIZE_ADDRESS;
extern const uint32_t CACHE_CONTROL_ADDRESS;
extern const uint32_t SPU_ADDRESS;
extern const uint32_t EXPANSION_1_ADDRESS;
extern const uint32_t EXPANSION_2_ADDRESS;
extern const uint32_t IRQ_CONTROL_ADDRESS;
extern const uint32_t TIMERS_ADDRESS;
extern const uint32_t DMA_ADDRESS;
extern const uint32_t GPU_ADDRESS;

// Memory segments' sizes
extern const uint32_t BIOS_SIZE;
extern const uint32_t RAM_SIZE;

// Reserved ranges
extern const Utils::Range BIOS_RANGE;
extern const Utils::Range MEMCONTROL_RANGE;
extern const Utils::Range RAM_RANGE;
extern const Utils::Range CACHE_CONTROL_RANGE;
extern const Utils::Range SPU_RANGE;
extern const Utils::Range EXPANSION_1_RANGE;
extern const Utils::Range EXPANSION_2_RANGE;
extern const Utils::Range IRQ_CONTROL_RANGE;
extern const Utils::Range TIMERS_RANGE;
extern const Utils::Range DMA_RANGE;
extern const Utils::Range GPU_RANGE;

// Other
extern const std::array<uint32_t, 8> REGION_MASK;

} // end namespace PSEmu

#endif // MEMORY_CONSTANTS_H