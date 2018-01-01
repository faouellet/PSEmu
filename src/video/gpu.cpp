#include "gpu.h"

#include <cassert>

using namespace PSEmu;

uint32_t GPU::GetStatus() const
{
    uint32_t status = 0;

    status |= static_cast<uint32_t>(m_pageBaseX) << 0;
    status |= static_cast<uint32_t>(m_pageBaseY) << 4;
    status |= static_cast<uint32_t>(m_semiTransparency) << 5;
    status |= static_cast<uint32_t>(m_textureDepth) << 7;
    status |= static_cast<uint32_t>(m_dithering) << 9;
    status |= static_cast<uint32_t>(m_allowToDisplay) << 10;
    status |= static_cast<uint32_t>(m_forceSetMaskBit) << 11;
    status |= static_cast<uint32_t>(m_preserveMaskedPixels) << 12;
    status |= static_cast<uint32_t>(m_field) << 13;
    // Bit 14: not supported
    status |= static_cast<uint32_t>(m_disableTexture) << 15;
    status |= m_hRes.ToStatus();
    status |= static_cast<uint32_t>(m_vRes) << 19;
    status |= static_cast<uint32_t>(m_vMode) << 20;
    status |= static_cast<uint32_t>(m_displayDepth) << 21;
    status |= static_cast<uint32_t>(m_interlaced) << 22;
    status |= static_cast<uint32_t>(m_displayDisabled) << 23;
    status |= static_cast<uint32_t>(m_interrupt) << 24;

    // For now, we pretend that the GPU is always ready:
    // Ready to receive command
    status |= 1 << 26;
    // Ready to send VRAM to CPU
    status |= 1 << 27;
    // Ready to receive DMA block
    status |= 1 << 28;

    status |= static_cast<uint32_t>(m_dmaDirection) << 29;

    // Bit 31 should change depending on the currently drawn line
    // (wether it's even, odd or in the vblack apparently).
    status |= 1 << 31;

    status |= [this, status]()
    {
        switch (m_dmaDirection)
        {
            // Always 0
            case DMADirection::OFF: return 0u;
            // Should be 0 if FIFO is full, 1 otherwise
            case DMADirection::FIFO: return 1u;
            // Should be the same as status bit 28
            case DMADirection::CPU_TO_GP0: return (status >> 28) & 1;
            // Should be the same as status bit 28
            case DMADirection::VRAM_TO_CPU: return (status >> 27) & 1;
        }
    }() << 25;

    return status;
}

void GPU::SetGP0(uint32_t value)
{
    const uint32_t opcode = ((value >> 24) & 0xFF);

    switch (opcode)
    {
        case 0x00:  // NOP
            break;
        case 0xE1:  // Draw mode setting
            SetGP0DrawMode(value);
            break;
        default:
            assert(false && "Unhandled GP0 command");
    }
}

void GPU::SetGP1(uint32_t value)
{
    const uint32_t opcode = ((value >> 24) & 0xFF);

    switch (opcode)
    {
        case 0x00:  // NOP
            break;
        default:
            assert(false && "Unhandled GP0 command");
    }
}

void GPU::SetGP0DrawMode(uint32_t value)
{
    m_pageBaseX = (value & 0xF);
    m_pageBaseY = ((value >> 4) & 1);
    m_semiTransparency = ((value >> 5) & 3);

    m_textureDepth = [&]()
    {
        switch ((value >> 7) & 3)
        {
            case 0: return TextureDepth::T4BIT;
            case 1: return TextureDepth::T8BIT;
            case 2: return TextureDepth::T15BIT;
            default: assert(false); return TextureDepth::T4BIT;
        }
    }();

    m_dithering = ((value >> 9) & 1) != 0;
    m_allowToDisplay = ((value >> 10) & 1) != 0;
    m_disableTexture = ((value >> 11) & 1) != 0;
    m_rectangleTextureFlipX = ((value >> 12) & 1) != 0;
    m_rectangleTextureFlipY = ((value >> 13) & 1) != 0;
    
}

void GPU::Reset()
{
    m_interrupt = false;
    m_pageBaseX = 0;
    m_pageBaseY = 0;
    m_semiTransparency = 0;
    m_textureDepth = TextureDepth::T4BIT;
    m_textureWindowMaskX = 0;
    m_textureWindowMaskY = 0;
    m_textureWindowOffsetX = 0;
    m_textureWindowOffsetY = 0;
    m_dithering = false;
    m_allowToDisplay = false;
    m_disableTexture = false;
    m_rectangleTextureFlipX = false;
    m_rectangleTextureFlipY = false;
    m_drawingAreaLeft = 0;
    m_drawingAreaBottom = 0;
    m_drawingAreaTop = 0;
    m_drawingAreaRight = 0;
    m_drawingOffsetX = 0;
    m_drawingOffsetY = 0;
    m_forceSetMaskBit = false;
    m_preserveMaskedPixels = false;
    m_dmaDirection = DMADirection::OFF;
    m_displayDisabled = false;
    m_displayVRAMStartX = 0;
    m_displayVRAMStartY = 0;
    m_hRes = HorizontalRes{0, 0};
    m_vRes = VerticalRes::Y240LINES;
    m_vMode = VMode::NTSC;
    m_interlaced = true;
    m_displayHorizStart = 0;
    m_displayHorizEnd = 0;
    m_displayLineStart = 0;
    m_displayLineEnd = 0;
    m_displayDepth = DisplayDepth::D15BITS;
}

void GPU::SetGP1DisplayMode(uint32_t value)
{
    const uint8_t hr1 = value & 3;
    const uint8_t hr2 = (value >> 6) & 1;

    m_hRes = HorizontalRes{hr1, hr2};

    m_vRes = ((value & 0x4) != 0) ? VerticalRes::Y240LINES : VerticalRes::Y480LINES;
    m_vMode = ((value & 0x8) != 0) ? VMode::NTSC : VMode::PAL;
    m_displayDepth = ((value & 0x10) != 0) ? DisplayDepth::D15BITS : DisplayDepth::D24BITS;

    m_interlaced = (value & 0x20) != 0;

    assert(value & 0x80 == 0 && "Unsupported display mode");
}

void GPU::SetGP1DMADirection(uint32_t value)
{
    switch (value & 3)
    {
        case 0: 
            m_dmaDirection = DMADirection::OFF;
            break;
        case 1: 
            m_dmaDirection = DMADirection::FIFO;
            break;
        case 2: 
            m_dmaDirection = DMADirection::CPU_TO_GP0;
            break;
        case 3: 
            m_dmaDirection = DMADirection::VRAM_TO_CPU;
            break;
        default:
            assert(false);
    }
}

void GPU::SetGP0DrawingAreaTopLeft(uint32_t value)
{
    m_drawingAreaTop = (value >> 10) & 0x3FF;
    m_drawingAreaLeft = value & 0x3FF;
}

void GPU::SetGP0DrawingAreaBottomRight(uint32_t value)
{
    m_drawingAreaBottom = (value >> 10) & 0x3FF;
    m_drawingAreaRight = value & 0x3FF;
}

void GPU::SetGP0DrawingOffset(uint32_t value)
{
    const uint16_t x = value & 0x7FF;
    const uint16_t y = (value >> 11) & 0x7FF;

    // Values are 11 bits two's complement signed values,
    // we need to shift the value to 16 bits to force sign extension
    m_drawingOffsetX = static_cast<int16_t>(x << 5) >> 5;
    m_drawingOffsetY = static_cast<int16_t>(y << 5) >> 5;
}

void GPU::SetGP0TextureWindow(uint32_t value)
{
    m_textureWindowMaskX = value & 0x1F;
    m_textureWindowMaskY = (value >> 5) & 0x1F;
    m_textureWindowOffsetX = (value >> 10) & 0x1F;
    m_textureWindowOffsetY = (value >> 15) & 0x1F;
}

void GPU::SetGP0MaskBitSetting(uint32_t value)
{
    m_forceSetMaskBit = (value & 1) != 0;
    m_preserveMaskedPixels = (value & 2) != 0;
}

void GPU::GP1DisplayVRAMStart(uint32_t value)
{
    m_displayVRAMStartX = value & 0x3FE;
    m_displayVRAMStartY = (value >> 10) & 0x1FF;
}

void GPU::GP1DisplayHorizontalRange(uint32_t value)
{
    m_displayHorizStart = value & 0xFFF;
    m_displayHorizEnd = (value >> 12) & 0xFFF;
}

void GPU::GP1DisplayVerticalRange(uint32_t value)
{
    m_displayLineStart = value & 0x3FF;
    m_displayLineEnd = (value >> 10) & 0x3FF;
}
