#ifndef CHANNEL_H
#define CHANNEL_H

#include "../utils/types.h"

#include <optional>

namespace PSEmu
{

enum class Direction
{
    TO_RAM,
    FROM_RAM
};

enum class Step
{
    INCREMENT,
    DECREMENT
};

enum class Sync
{
    // Transfer starts when the CPU writes to the Trigger bit and transfers everything at once
    MANUAL,
    // Sync blocks to DMA requests
    REQUEST,
    // Used to transfer GPU command lists
    LINKED_LIST
};

class Channel
{
public:
    Utils::UInt32 GetControl() const;
    void SetControl(Utils::UInt32 value);

    Utils::UInt32 GetBase() const;
    Utils::UInt32& GetBase();
    void SetBase(Utils::UInt32 value);

    Utils::UInt32 GetBlockControl() const;
    void SetBlockControl(Utils::UInt32 value);

    bool IsActive() const;
    bool SetDone();

    Direction GetDirection() const;
    Step GetStep() const;
    Sync GetSync() const;

    std::optional<Utils::UInt32> GetTransferSize() const;

private:
    bool m_enable;
    Direction m_direction;
    Step m_step;
    Sync m_sync;
    bool m_trigger; /**< Used to start the DMA transfer when 'm_sync' is 'MANUAL' */
    bool m_chop; /**< If true the DMA *chops* the transfer and lets the CPU runs in the gaps */
    Utils::UInt8 m_chopDMASize; /**< Chopping DMA window size (log2 number of words) */
    Utils::UInt8 m_chopCPUSize; /**< Chopping CPU window size (log2 number of words) */
    Utils::UInt8 m_unknown; /**< Unknown 2 RW bits in configuration register */
    Utils::UInt32 m_base;   /**< DMA start address */
    Utils::UInt16 m_blockSize;  /**< Size of a block in words */
    Utils::UInt16 m_blockCount; /**< Block count. Only used when 'm_sync' is 'REQUEST' */
};

}   // end namespace PSEmu

#endif // CHANNEL_H