#ifndef GPU_H
#define GPU_H

#include "commandbuffer.h"
#include "renderer.h"

#include <cstdint>

namespace PSEmu
{

// Depth of the pixel values in a texture page
enum class TextureDepth
{
    T4BIT,  // 4 bits per pixel
    T8BIT,  // 8 bits per pixel
    T15BIT  // 15 bits per pixel
}; 

// Interlaced output splits each frame in two fields
enum class Field
{
    TOP,    // Top field (odd lines)
    BOTTOM  // Bottom field (even lines)
};

// Video output horizontal resolution
struct HorizontalRes
{
public:
    // TODO: Review this
    HorizontalRes() = default;

    HorizontalRes(uint8_t hr1, uint8_t hr2)
    {
        m_resolution = (hr2 & 1) | ((hr1 & 3) << 1);
    }

    uint32_t ToStatus() const
    {
        return (static_cast<uint32_t>(m_resolution) << 16);
    }

private:
    uint8_t m_resolution;
};

// Video output vertical resolution
enum class VerticalRes
{
    Y240LINES,  // 240 lines
    Y480LINES   // 480 lines (only available for interlaced output)
};

enum class VMode
{
    NTSC,   // NTSC: 480i60Hz
    PAL,    // PAL:  575i50Hz
};

enum class DisplayDepth
{
    D15BITS,    // 15 bits per pixel
    D24BITS     // 24 bits per pixel
};

// Requested DMA direction
enum class DMADirection
{
    OFF,
    FIFO,
    CPU_TO_GP0,
    VRAM_TO_CPU
};

enum class GP0Mode
{
    COMMAND,
    IMAGE_LOAD
};

class GPU
{
public:
    GPU(Renderer renderer);

    GPU(const GPU&) = delete;
    GPU& operator=(const GPU&) = delete;

    GPU(GPU&&) = default;
    GPU& operator=(GPU&&) = default;

public:
    uint32_t GetStatus() const;
    void SetGP0(uint32_t value);
    void SetGP1(uint32_t value);
    uint32_t GetRead() const;

private:    // GP0 commands
    void GP0ClearCache();
    void GP0DrawQuadMonoOpaque();
    void GP0DrawQuadShadedOpaque();
    void GP0DrawQuadTextureBlendOpaque();
    void GP0DrawTriShadedOpaque();
    void GP0SetDrawingAreaTopLeft();
    void GP0SetDrawingAreaBottomRight();
    void GP0SetDrawingOffset();
    void GP0SetDrawMode();
    void GP0LoadImage();   
    void GP0SetMaskBitSetting();
    void GP0NOP();
    void GP0StoreImage();
    void GP0SetTextureWindow();   

private:    // GP1 commands
    void GP1AcknowledgeIRQ();
    void GP1DisplayVRAMStart(uint32_t value);
    void GP1ResetCommandBuffer();
    void GP1SetDisplayEnabled(uint32_t value);
    void GP1SetDisplayMode(uint32_t value);
    void GP1SetDMADirection(uint32_t value);
    void GP1SetDisplayHorizontalRange(uint32_t value);
    void GP1SetDisplayVerticalRange(uint32_t value);

private:    // Utilities
    uint32_t Read() const { return 0; }
    void Reset();

private:
    // Texture page base X coordinate (4 bits, 64 byte increment)
    uint8_t m_pageBaseX;

    // Texture page base Y coordinate (1 bits, 256 line increment)
    uint8_t m_pageBaseY;

    // Semi-transparency
    uint8_t m_semiTransparency;

    // Texture page color depth
    TextureDepth m_textureDepth;

    // Enable dithering from 24 to 15 bits RGB
    bool m_dithering;

    // Allow drawing to display area
    bool m_allowToDisplay;

    // Force *mask* bit of the pixel to 1 when writing to VRAM
    // (otherwise don't modify it)
    bool m_forceSetMaskBit;

    // Don't draw to pixels which have the *mask* bit set
    bool m_preserveMaskedPixels;

    // Currentyl displayed field. For progressive output, this is always TOP
    Field m_field;

    // When true all textures are disabled
    bool m_disableTexture;

    // Video output horizontal resolution
    HorizontalRes m_hRes;

    // Video output vertical resolution
    VerticalRes m_vRes;

    // Video mode
    VMode m_vMode;

    // Display depth. The GPU itself always draws 15 bit RGB.
    // 24 bit output must use external assets (pre-rendered textures, MDEC, etc.)
    DisplayDepth m_displayDepth;

    // Output interlaced video signal instead of progresive
    bool m_interlaced;

    // Disable the display
    bool m_displayDisabled;

    // True when the interrupt is active
    bool m_interrupt;

    // DMA request direction
    DMADirection m_dmaDirection;

    // Mirror textured rectangles along the X axis
    bool m_rectangleTextureFlipX;

    // Mirror textured rectangles along the Y axis
    bool m_rectangleTextureFlipY;

    // Texture window X mask (8 pixel steps)
    uint8_t m_textureWindowMaskX;

    // Texture window Y mask (8 pixel steps)
    uint8_t m_textureWindowMaskY;

    // Texture window X offset (8 pixel steps)
    uint8_t m_textureWindowOffsetX;

    // Texture window Y offset (8 pixel steps)
    uint8_t m_textureWindowOffsetY;

    // Left-most column of drawing area
    uint16_t m_drawingAreaLeft;

    // Top-most line of drawing area
    uint16_t m_drawingAreaTop;

    // Right-most column of drawing area
    uint16_t m_drawingAreaRight;

    // Bottom-most line of drawing area
    uint16_t m_drawingAreaBottom;

    // Horizontal drawing offset applied to all vertex
    uint16_t m_drawingOffsetX;

    // Vertical drawing offset applied to all vertex
    uint16_t m_drawingOffsetY;

    // First column of the display area in VRAM
    uint16_t m_displayVRAMStartX;

    // Fisrt column of the display area in VRAM
    uint16_t m_displayVRAMStartY;

    // Display output horizontal start relative to HSYNC
    uint16_t m_displayHorizStart;

    // Display output horizontal end relative to HSYNC
    uint16_t m_displayHorizEnd;

    // Display output horizontal start relative to VSYNC
    uint16_t m_displayLineStart;

    // Display output horizontal end relative to VSYNC
    uint16_t m_displayLineEnd;

    // Buffer containing the current GP0 command
    CommandBuffer m_GP0Command;

    // Remaining words in the current GP0 command
    uint32_t m_GP0WordsRemaining;

    // Pointer to the method implementing the current GP0 command
    void (GPU::*m_GP0CommandMethod)();

    // Current mode of the GP0 register
    GP0Mode m_GP0Mode;

    Renderer m_renderer;
};

}   // end namespace PSEmu

#endif // GPU_H