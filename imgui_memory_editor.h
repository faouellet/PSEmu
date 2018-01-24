#pragma once

#include "3rd party/imgui/imgui.h"

#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <stdio.h>

class MemoryEditor
{
private:
    using u8 = unsigned char;

    // Settings
    bool            m_open;                           // = true    // set to false when DrawWindow() was closed. ignore if not using DrawWindow
    bool            m_readOnly;                       // = false   // set to true to disable any editing
    int             m_rows;                           // = 16      //
    bool            m_optShowAscii;                   // = true    //
    bool            m_optShowHexII;                   // = false   //
    bool            m_optGreyOutZeroes;               // = true    //
    int             m_optMidRowsCount;                // = 8       // set to 0 to disable extra spacing between every mid-rows
    int             m_optAddrDigitsCount;             // = 0       // number of addr digits to display (default calculated based on maximum displayed addr)
    ImU32           m_highlightColor;                 //           // color of highlight

    std::function<u8(u8*, size_t)> m_readFn;          // = nullptr // optional handler to read bytes
    std::function<void(u8*, size_t, u8)> m_writeFn;   // = nullptr // optional handler to write bytes
    std::function<bool(u8*, size_t)> m_HighlightFn;   // = nullptr // optional handler to return Highlight property (to support non-contiguous highlighting)

    // State/Internals
    bool                 m_contentsWidthChanged;
    size_t               m_dataEditingAddr;
    bool                 m_dataEditingTakeFocus;
    std::array<char, 32> m_dataInputBuf;
    std::array<char, 32> m_addrInputBuf;
    size_t               m_gotoAddr;
    size_t               m_highlightMax;
    size_t               m_highlightMin;

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

private:
    Sizes CalcSizes(size_t memSize, size_t baseDisplayAddr)
    {
        Sizes s;
        ImGuiStyle& style = ImGui::GetStyle();
        s.AddrDigitsCount = m_optAddrDigitsCount;
        if (s.AddrDigitsCount == 0)
        {
            for (size_t n = baseDisplayAddr + memSize - 1; n > 0; n >>= 4)
            {
                ++s.AddrDigitsCount;
            }
        }
            
        s.LineHeight = ImGui::GetTextLineHeight();
        s.GlyphWidth = ImGui::CalcTextSize("F").x + 1;                  // We assume the font is mono-space
        s.HexCellWidth = std::floor(s.GlyphWidth * 2.5f);             // "FF " we include trailing space in the width to easily catch clicks everywhere
        s.SpacingBetweenMidRows = std::floor(s.HexCellWidth * 0.25f); // Every m_optMidRowsCount columns we add a bit of extra spacing
        s.PosHexStart = (s.AddrDigitsCount + 2) * s.GlyphWidth;
        s.PosHexEnd = s.PosHexStart + (s.HexCellWidth * m_rows);
        s.PosAsciiStart = s.PosAsciiEnd = s.PosHexEnd;
        if (m_optShowAscii)
        {
            s.PosAsciiStart = s.PosHexEnd + s.GlyphWidth * 1;
            if (m_optMidRowsCount > 0)
            {
                s.PosAsciiStart += ((m_rows + m_optMidRowsCount - 1) / m_optMidRowsCount) * s.SpacingBetweenMidRows;
            }
            s.PosAsciiEnd = s.PosAsciiStart + m_rows * s.GlyphWidth;
        }
        s.WindowWidth = s.PosAsciiEnd + style.ScrollbarSize + style.WindowPadding.x * 2 + s.GlyphWidth;

        return s;
    }

    void GotoAddrAndHighlight(size_t addrMin, size_t addrMax)
    {
        m_gotoAddr = addrMin;
        m_highlightMin = addrMin;
        m_highlightMax = addrMax;
    }

public:
    MemoryEditor() 
        // Settings
        : m_open{ true }
        , m_readOnly{ false }
        , m_rows{ 16 }
        , m_optShowAscii{ true }
        , m_optShowHexII{ false }
        , m_optGreyOutZeroes{ true }
        , m_optMidRowsCount{ 8 }
        , m_optAddrDigitsCount{ 0 }
        , m_highlightColor{ IM_COL32(255, 255, 255, 40) }
        , m_readFn{}
        , m_writeFn{}
        , m_HighlightFn{}
        // State/Internals
        , m_contentsWidthChanged{ false }
        , m_dataEditingAddr{ std::numeric_limits<size_t>::max() }
        , m_dataEditingTakeFocus{ false }
        , m_dataInputBuf{}
        , m_addrInputBuf{}
        , m_gotoAddr{ std::numeric_limits<size_t>::max() }
        , m_highlightMax{ std::numeric_limits<size_t>::max() }
        , m_highlightMin{ std::numeric_limits<size_t>::max() }
    { }

#ifdef _MSC_VER
#define _PRISizeT   "IX"
#else
#define _PRISizeT   "zX"
#endif

    // Standalone Memory Editor window
    void DrawWindow(const char* title, u8* memData, size_t memSize, size_t baseDisplayAddr = 0x0000)
    {
        Sizes s{ CalcSizes(memSize, baseDisplayAddr) };
        ImGui::SetNextWindowSizeConstraints(ImVec2{0.0f, 0.0f}, ImVec2{s.WindowWidth, std::numeric_limits<float>::max()});

        m_open = true;
        if (ImGui::Begin(title, &m_open, ImGuiWindowFlags_NoScrollbar))
        {
            DrawContents(memData, memSize, baseDisplayAddr);
            if (m_contentsWidthChanged)
            {
                ImGui::SetWindowSize(ImVec2(s.WindowWidth, ImGui::GetWindowSize().y));
            }
        }
        ImGui::End();
    }

    // Memory Editor contents only
    void DrawContents(u8* memData, size_t memSize, size_t baseDisplayAddr = 0x0000)
    {
        Sizes s{ CalcSizes(memSize, baseDisplayAddr) };
        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::BeginChild("##scrolling", ImVec2{0, -ImGui::GetFrameHeightWithSpacing()});
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0.f, 0.f});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});

        const int lineTotalCount = (int)((memSize + m_rows - 1) / m_rows);
        ImGuiListClipper clipper(lineTotalCount, s.LineHeight);
        const size_t visibleStartAddr = clipper.DisplayStart * m_rows;
        const size_t visibleEndAddr = clipper.DisplayEnd * m_rows;

        bool dataNext = false;

        if (m_readOnly || m_dataEditingAddr >= memSize)
        {
            m_dataEditingAddr = std::numeric_limits<size_t>::max();
        }

        size_t dataEditingAddrBackup = m_dataEditingAddr;
        size_t dataEditingAddrNext = std::numeric_limits<size_t>::max();
        if (m_dataEditingAddr != std::numeric_limits<size_t>::max())
        {
            // Move cursor but only apply on next frame so scrolling with be synchronized (because currently we can't change the scrolling while the window is being rendered)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && m_dataEditingAddr >= (size_t)m_rows)          
            { 
                dataEditingAddrNext = m_dataEditingAddr - m_rows; 
                m_dataEditingTakeFocus = true; 
            }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) && m_dataEditingAddr < memSize - m_rows) 
            { 
                dataEditingAddrNext = m_dataEditingAddr + m_rows; 
                m_dataEditingTakeFocus = true; 
            }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)) && m_dataEditingAddr > 0)               
            { 
                dataEditingAddrNext = m_dataEditingAddr - 1; 
                m_dataEditingTakeFocus = true; 
            }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) && m_dataEditingAddr < memSize - 1)   
            { 
                dataEditingAddrNext = m_dataEditingAddr + 1; 
                m_dataEditingTakeFocus = true; 
            }
        }
        if (dataEditingAddrNext != (size_t)-1 && (dataEditingAddrNext / m_rows) != (dataEditingAddrBackup / m_rows))
        {
            // Track cursor movements
            const int scrollOffset = ((int)(dataEditingAddrNext / m_rows) - (int)(dataEditingAddrBackup / m_rows));
            const bool scrollDesired = (scrollOffset < 0 && dataEditingAddrNext < visibleStartAddr + m_rows * 2) || (scrollOffset > 0 && dataEditingAddrNext > visibleEndAddr - m_rows * 2);
            if (scrollDesired)
            {
                ImGui::SetScrollY(ImGui::GetScrollY() + scrollOffset * s.LineHeight);
            }
        }

        // Draw vertical separator
        ImVec2 windowPos = ImGui::GetWindowPos();
        if (m_optShowAscii)
        {
            drawList->AddLine(ImVec2(windowPos.x + s.PosAsciiStart - s.GlyphWidth, windowPos.y), ImVec2(windowPos.x + s.PosAsciiStart - s.GlyphWidth, windowPos.y + 9999), ImGui::GetColorU32(ImGuiCol_Border));
        }

        const ImU32 colorText = ImGui::GetColorU32(ImGuiCol_Text);
        const ImU32 colorDisabled = m_optGreyOutZeroes ? ImGui::GetColorU32(ImGuiCol_TextDisabled) : colorText;

        for (int iLine = clipper.DisplayStart; iLine < clipper.DisplayEnd; iLine++) // display only visible lines
        {
            size_t addr = (size_t)(iLine * m_rows);
            ImGui::Text("%0*" _PRISizeT ": ", s.AddrDigitsCount, baseDisplayAddr + addr);

            // Draw Hexadecimal
            for (int n = 0; n < m_rows && addr < memSize; ++n, ++addr)
            {
                float byte_pos_x = s.PosHexStart + s.HexCellWidth * n;
                if (m_optMidRowsCount > 0)
                {
                    byte_pos_x += (n / m_optMidRowsCount) * s.SpacingBetweenMidRows;
                }
                ImGui::SameLine(byte_pos_x);

                // Draw highlight
                if ((addr >= m_highlightMin && addr < m_highlightMax) || (m_HighlightFn && m_HighlightFn(memData, addr)))
                {
                    ImVec2 pos = ImGui::GetCursorScreenPos();
                    float highlight_width = s.GlyphWidth * 2;
                    bool is_next_byte_highlighted =  (addr + 1 < memSize) && ((m_highlightMax != (size_t)-1 && addr + 1 < m_highlightMax) || (m_HighlightFn && m_HighlightFn(memData, addr + 1)));
                    if (is_next_byte_highlighted || (n + 1 == m_rows))
                    {
                        highlight_width = s.HexCellWidth;
                        if (m_optMidRowsCount > 0 && n > 0 && (n + 1) < m_rows && ((n + 1) % m_optMidRowsCount) == 0)
                            highlight_width += s.SpacingBetweenMidRows;
                    }
                    drawList->AddRectFilled(pos, ImVec2(pos.x + highlight_width, pos.y + s.LineHeight), m_highlightColor);
                }

                if (m_dataEditingAddr == addr)
                {
                    // Display text input on current byte
                    bool dataWrite = false;
                    ImGui::PushID((void*)addr);
                    if (m_dataEditingTakeFocus)
                    {
                        ImGui::SetKeyboardFocusHere();
                        ImGui::CaptureKeyboardFromApp(true);
                        sprintf(m_addrInputBuf.data(), "%0*" _PRISizeT, s.AddrDigitsCount, baseDisplayAddr + addr);
                        sprintf(m_dataInputBuf.data(), "%02X", m_readFn ? m_readFn(memData, addr) : memData[addr]);
                    }
                    ImGui::PushItemWidth(s.GlyphWidth * 2);
                    struct UserData
                    {
                        // FIXME: We should have a way to retrieve the text edit cursor position more easily in the API, this is rather tedious. This is such a ugly mess we may be better off not using InputText() at all here.
                        static int Callback(ImGuiTextEditCallbackData* data)
                        {
                            UserData* userData = (UserData*)data->UserData;
                            if (!data->HasSelection())
                            {
                                userData->CursorPos = data->CursorPos;
                            }
                            if (data->SelectionStart == 0 && data->SelectionEnd == data->BufTextLen)
                            {
                                // When not editing a byte, always rewrite its content (this is a bit tricky, since InputText technically "owns" the master copy of the buffer we edit it in there)
                                data->DeleteChars(0, data->BufTextLen);
                                data->InsertChars(0, userData->CurrentBufOverwrite);
                                data->SelectionStart = 0;
                                data->SelectionEnd = data->CursorPos = 2;
                            }
                            return 0;
                        }
                        char   CurrentBufOverwrite[3];  // Input
                        int    CursorPos;               // Output
                    };
                    UserData userData;
                    userData.CursorPos = -1;
                    sprintf(userData.CurrentBufOverwrite, "%02X", m_readFn ? m_readFn(memData, addr) : memData[addr]);
                    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CallbackAlways;
                    if (ImGui::InputText("##data", m_dataInputBuf.data(), 32, flags, UserData::Callback, &userData))
                    {
                        dataWrite = dataNext = true;
                    }
                    else if (!m_dataEditingTakeFocus && !ImGui::IsItemActive())
                    {
                        m_dataEditingAddr = dataEditingAddrNext = (size_t)-1;
                    }
                    m_dataEditingTakeFocus = false;
                    ImGui::PopItemWidth();
                    if (userData.CursorPos >= 2)
                    {
                        dataWrite = dataNext = true;
                    }
                    if (dataEditingAddrNext != (size_t)-1)
                    {
                        dataWrite = dataNext = false;
                    }
                    unsigned int data_input_value;
                    if (dataWrite && sscanf(m_dataInputBuf.data(), "%X", &data_input_value) == 1)
                    {
                        if (m_writeFn)
                        {
                            m_writeFn(memData, addr, (u8)data_input_value);
                        }
                        else
                        {
                            memData[addr] = (u8)data_input_value;
                        }
                    }
                    ImGui::PopID();
                }
                else
                {
                    // NB: The trailing space is not visible but ensure there's no gap that the mouse cannot click on.
                    u8 b = m_readFn ? m_readFn(memData, addr) : memData[addr];

                    if (m_optShowHexII)
                    {
                        if ((b >= 32 && b < 128))
                        {
                            ImGui::Text(".%c ", b);
                        }
                        else if (b == 0xFF && m_optGreyOutZeroes)
                        {
                            ImGui::TextDisabled("## ");
                        }
                        else if (b == 0x00)
                        {
                            ImGui::Text("   ");
                        }
                        else
                        {
                            ImGui::Text("%02X ", b);
                        }
                    }
                    else
                    {
                        if (b == 0 && m_optGreyOutZeroes)
                        {
                            ImGui::TextDisabled("00 ");
                        }
                        else
                        {
                            ImGui::Text("%02X ", b);
                        }
                    }
                    if (!m_readOnly && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                    {
                        m_dataEditingTakeFocus = true;
                        dataEditingAddrNext = addr;
                    }
                }
            }

            if (m_optShowAscii)
            {
                // Draw ASCII values
                ImGui::SameLine(s.PosAsciiStart);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                addr = iLine * m_rows;
                ImGui::PushID(iLine);
                if (ImGui::InvisibleButton("ascii", ImVec2(s.PosAsciiEnd - s.PosAsciiStart, s.LineHeight)))
                {
                    m_dataEditingAddr = addr + (size_t)((ImGui::GetIO().MousePos.x - pos.x) / s.GlyphWidth);
                    m_dataEditingTakeFocus = true;
                }
                ImGui::PopID();
                for (int n = 0; n < m_rows && addr < memSize; n++, addr++)
                {
                    if (addr == m_dataEditingAddr)
                    {
                        drawList->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_FrameBg));
                        drawList->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_TextSelectedBg));
                    }
                    unsigned char c = m_readFn ? m_readFn(memData, addr) : memData[addr];
                    char display_c = (c < 32 || c >= 128) ? '.' : c;
                    drawList->AddText(pos, (display_c == '.') ? colorDisabled : colorText, &display_c, &display_c + 1);
                    pos.x += s.GlyphWidth;
                }
            }
        }
        clipper.End();
        ImGui::PopStyleVar(2);
        ImGui::EndChild();

        if (dataNext && m_dataEditingAddr < memSize)
        {
            m_dataEditingAddr = m_dataEditingAddr + 1;
            m_dataEditingTakeFocus = true;
        }
        else if (dataEditingAddrNext != (size_t)-1)
        {
            m_dataEditingAddr = dataEditingAddrNext;
        }

        ImGui::Separator();

        // Options menu
        if (ImGui::Button("Options"))
        {
            ImGui::OpenPopup("context");
        }
        if (ImGui::BeginPopup("context"))
        {
            ImGui::PushItemWidth(56);
            if (ImGui::DragInt("##rows", &m_rows, 0.2f, 4, 32, "%.0f rows")) 
            {
                m_contentsWidthChanged = true;
            }
            ImGui::PopItemWidth();
            ImGui::Checkbox("Show HexII", &m_optShowHexII);
            if (ImGui::Checkbox("Show Ascii", &m_optShowAscii))
            {
                m_contentsWidthChanged = true;
            } 
            ImGui::Checkbox("Grey out zeroes", &m_optGreyOutZeroes);
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::Text("Range %0*" _PRISizeT "..%0*" _PRISizeT, s.AddrDigitsCount, baseDisplayAddr, s.AddrDigitsCount, baseDisplayAddr + memSize - 1);
        ImGui::SameLine();
        ImGui::PushItemWidth((s.AddrDigitsCount + 1) * s.GlyphWidth + style.FramePadding.x * 2.0f);
        if (ImGui::InputText("##addr", m_addrInputBuf.data(), 32, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            size_t goto_addr;
            if (sscanf(m_addrInputBuf.data(), "%" _PRISizeT, &goto_addr) == 1)
            {
                m_gotoAddr = goto_addr - baseDisplayAddr;
                m_highlightMin = m_highlightMax = std::numeric_limits<size_t>::max();
            }
        }
        ImGui::PopItemWidth();

        if (m_gotoAddr != std::numeric_limits<size_t>::max())
        {
            if (m_gotoAddr < memSize)
            {
                ImGui::BeginChild("##scrolling");
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + (m_gotoAddr / m_rows) * ImGui::GetTextLineHeight());
                ImGui::EndChild();
                m_dataEditingAddr = m_gotoAddr;
                m_dataEditingTakeFocus = true;
            }
            m_gotoAddr = (size_t)-1;
        }

        // Notify the main window of our ideal child content size (FIXME: we are missing an API to get the contents size from the child)
        ImGui::SetCursorPosX(s.WindowWidth);
    }
};
#undef _PRISizeT