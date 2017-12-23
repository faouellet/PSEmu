#ifndef MEMORY_CONSTANTS_H
#define MEMORY_CONSTANTS_H

#include "../utils/range.h"

#include <array>

namespace PSEmu
{

// Reserved addresses
extern const Utils::UInt32 BIOS_ADDRESS;
extern const Utils::UInt32 EXPANSION_MAPPING_ADDRESS;
extern const Utils::UInt32 RAM_SIZE_ADDRESS;
extern const Utils::UInt32 CACHE_CONTROL_ADDRESS;
extern const Utils::UInt32 SPU_ADDRESS;
extern const Utils::UInt32 EXPANSION_1_ADDRESS;
extern const Utils::UInt32 EXPANSION_2_ADDRESS;
extern const Utils::UInt32 IRQ_CONTROL_ADDRESS;
extern const Utils::UInt32 TIMERS_ADDRESS;
extern const Utils::UInt32 DMA_ADDRESS;
extern const Utils::UInt32 GPU_ADDRESS;

// Memory segments' sizes
extern const Utils::UInt32 BIOS_SIZE;
extern const Utils::UInt32 RAM_SIZE;

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
extern const std::array<Utils::UInt32, 8> REGION_MASK;

} // end namespace PSEmu

#endif // MEMORY_CONSTANTS_H