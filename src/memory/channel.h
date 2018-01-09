#ifndef CHANNEL_H
#define CHANNEL_H

#include <cstdint>
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
    uint32_t GetControl() const;
    void SetControl(uint32_t value);

    uint32_t GetBase() const;
    uint32_t& GetBase();
    void SetBase(uint32_t value);

    uint32_t GetBlockControl() const;
    void SetBlockControl(uint32_t value);

    bool IsActive() const;
    void SetDone();

    Direction GetDirection() const;
    Step GetStep() const;
    Sync GetSync() const;

    std::optional<uint32_t> GetTransferSize() const;

private:
    bool m_enable;
    Direction m_direction;
    Step m_step;
    Sync m_sync;
    bool m_trigger; /**< Used to start the DMA transfer when 'm_sync' is 'MANUAL' */
    bool m_chop; /**< If true the DMA *chops* the transfer and lets the CPU runs in the gaps */
    uint8_t m_chopDMASize; /**< Chopping DMA window size (log2 number of words) */
    uint8_t m_chopCPUSize; /**< Chopping CPU window size (log2 number of words) */
    uint8_t m_unknown; /**< Unknown 2 RW bits in configuration register */
    uint32_t m_base;   /**< DMA start address */
    uint16_t m_blockSize;  /**< Size of a block in words */
    uint16_t m_blockCount; /**< Block count. Only used when 'm_sync' is 'REQUEST' */
};

}   // end namespace PSEmu

#endif // CHANNEL_H