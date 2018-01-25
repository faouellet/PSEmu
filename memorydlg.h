#pragma once

#include "3rd party/imgui/imgui.h"

#include <functional>

class MemoryEditor
{
private:
    struct Sizes
    {
        int     AddrDigitsCount;
        float   LineHeight;
        float   GlyphWidth;
        float   HexCellWidth;
        float   SpacingBetweenMidRows;
        float   PosHexStart;
        float   PosHexEnd;
        float   PosAsciiStart;
        float   PosAsciiEnd;
        float   WindowWidth;
    };

public:
    MemoryEditor();

public:
    void DrawWindow(const char* title, uint8_t* memData, size_t memSize, size_t baseDisplayAddr = 0x0000);
    void DrawContents(uint8_t* memData, size_t memSize, size_t baseDisplayAddr = 0x0000);

private:
    Sizes CalcSizes(size_t memSize, size_t baseDisplayAddr);
    void GotoAddrAndHighlight(size_t addrMin, size_t addrMax);

private:
    // Settings
    bool            m_readOnly;                       // = false   // set to true to disable any editing
    int             m_rows;                           // = 16      //
    bool            m_optShowAscii;                   // = true    //
    bool            m_optShowHexII;                   // = false   //
    bool            m_optGreyOutZeroes;               // = true    //
    int             m_optMidRowsCount;                // = 8       // set to 0 to disable extra spacing between every mid-rows
    int             m_optAddrDigitsCount;             // = 0       // number of addr digits to display (default calculated based on maximum displayed addr)
    ImU32           m_highlightColor;                 //           // color of highlight

    // State/Internals
    size_t               m_dataEditingAddr;
    bool                 m_dataEditingTakeFocus;
    std::array<char, 32> m_dataInputBuf;
    std::array<char, 32> m_addrInputBuf;
    size_t               m_gotoAddr;
    size_t               m_highlightMax;
    size_t               m_highlightMin;
};