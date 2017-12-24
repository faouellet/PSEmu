#ifndef GPU_H
#define GPU_H

#include "../utils/types.h"

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

    HorizontalRes(Utils::UInt8 hr1, Utils::UInt8 hr2)
    {
        m_resolution = (hr2 & 1) | ((hr1 & 3) << 1);
    }

    Utils::UInt32 ToStatus() const
    {
        return (static_cast<Utils::UInt32>(m_resolution) << 16);
    }

private:
    Utils::UInt8 m_resolution;
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

class GPU
{
public:
    Utils::UInt32 GetStatus() const;
    void SetGP0(Utils::UInt32 value);
    void SetGP1(Utils::UInt32 value);

private:
    void SetGP0DrawMode(Utils::UInt32 value);
    void Reset();
    Utils::UInt32 Read() const { return 0; }
    void SetGP1DisplayMode(Utils::UInt32 value);
    void SetGP1DMADirection(Utils::UInt32 value);
    void SetGP0DrawingAreaTopLeft(Utils::UInt32 value);
    void SetGP0DrawingAreaBottomRight(Utils::UInt32 value);
    void SetGP0DrawingOffset(Utils::UInt32 value);
    void SetGP0TextureWindow(Utils::UInt32 value);
    void SetGP0MaskBitSetting(Utils::UInt32 value);
    void GP1DisplayVRAMStart(Utils::UInt32 value);
    void GP1DisplayHorizontalRange(Utils::UInt32 value);
    void GP1DisplayVerticalRange(Utils::UInt32 value);

private:
    // Texture page base X coordinate (4 bits, 64 byte increment)
    Utils::UInt8 m_pageBaseX;

    // Texture page base Y coordinate (1 bits, 256 line increment)
    Utils::UInt8 m_pageBaseY;

    // Semi-transparency
    Utils::UInt8 m_semiTransparency;

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
    Utils::UInt8 m_textureWindowMaskX;

    // Texture window Y mask (8 pixel steps)
    Utils::UInt8 m_textureWindowMaskY;

    // Texture window X offset (8 pixel steps)
    Utils::UInt8 m_textureWindowOffsetX;

    // Texture window Y offset (8 pixel steps)
    Utils::UInt8 m_textureWindowOffsetY;

    // Left-most column of drawing area
    Utils::UInt16 m_drawingAreaLeft;

    // Top-most line of drawing area
    Utils::UInt16 m_drawingAreaTop;

    // Right-most column of drawing area
    Utils::UInt16 m_drawingAreaRight;

    // Bottom-most line of drawing area
    Utils::UInt16 m_drawingAreaBottom;

    // Horizontal drawing offset applied to all vertex
    Utils::UInt16 m_drawingOffsetX;

    // Vertical drawing offset applied to all vertex
    Utils::UInt16 m_drawingOffsetY;

    // First column of the display area in VRAM
    Utils::UInt16 m_displayVRAMStartX;

    // Fisrt column of the display area in VRAM
    Utils::UInt16 m_displayVRAMStartY;

    // Display output horizontal start relative to HSYNC
    Utils::UInt16 m_displayHorizStart;

    // Display output horizontal end relative to HSYNC
    Utils::UInt16 m_displayHorizEnd;

    // Display output horizontal start relative to VSYNC
    Utils::UInt16 m_displayLineStart;

    // Display output horizontal end relative to VSYNC
    Utils::UInt16 m_displayLineEnd;
};

}   // end namespace PSEmu

#endif // GPU_H