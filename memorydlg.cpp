#include "memorydlg.h"

#include <array>
#include <cmath>
#include <limits>
#include <stdio.h>

const size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

MemoryEditor::MemoryEditor() 
    // Settings
    : m_readOnly{ true }
    , m_rows{ 16 }
    , m_optShowAscii{ true }
    , m_optShowHexII{ false }
    , m_optGreyOutZeroes{ true }
    , m_optMidRowsCount{ 8 }
    , m_optAddrDigitsCount{ 0 }
    , m_highlightColor{ IM_COL32(255, 255, 255, 40) }
    // State/Internals
    , m_dataEditingAddr{ SIZE_T_MAX }
    , m_dataEditingTakeFocus{ false }
    , m_dataInputBuf{}
    , m_addrInputBuf{}
    , m_gotoAddr{ SIZE_T_MAX }
    , m_highlightMax{ SIZE_T_MAX }
    , m_highlightMin{ SIZE_T_MAX } { }

MemoryEditor::Sizes MemoryEditor::CalcSizes(size_t memSize, size_t baseDisplayAddr)
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

void MemoryEditor::GotoAddrAndHighlight(size_t addrMin, size_t addrMax)
{
    m_gotoAddr = addrMin;
    m_highlightMin = addrMin;
    m_highlightMax = addrMax;
}

#ifdef _MSC_VER
#define _PRISizeT   "IX"
#else
#define _PRISizeT   "zX"
#endif

// Standalone Memory Editor window
void MemoryEditor::DrawWindow(const char* title, uint8_t* memData, size_t memSize, size_t baseDisplayAddr)
{
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiSetCond_Always );
    ImGui::SetNextWindowSizeConstraints(ImVec2{width, height}, ImVec2{width, height});

    bool open = true;
    if (ImGui::Begin(title, &open, ImGuiWindowFlags_NoScrollbar))
    {
        DrawContents(memData, memSize, baseDisplayAddr);
    }
    ImGui::End();
}

// Memory Editor contents only
void MemoryEditor::DrawContents(uint8_t* memData, size_t memSize, size_t baseDisplayAddr)
{
    Sizes s{ CalcSizes(memSize, baseDisplayAddr) };
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::BeginChild("##scrolling", ImVec2{0, -ImGui::GetFrameHeightWithSpacing()});
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0.f, 0.f});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});

    const int lineTotalCount = ((memSize + m_rows - 1) / m_rows);
    ImGuiListClipper clipper(lineTotalCount, s.LineHeight);
    const size_t visibleStartAddr = clipper.DisplayStart * m_rows;
    const size_t visibleEndAddr = clipper.DisplayEnd * m_rows;

    bool dataNext = false;

    if (m_dataEditingAddr >= memSize)
    {
        m_dataEditingAddr = SIZE_T_MAX;
    }

    size_t dataEditingAddrBackup = m_dataEditingAddr;
    size_t dataEditingAddrNext = SIZE_T_MAX;
    if (m_dataEditingAddr != SIZE_T_MAX)
    {
        // Move cursor but only apply on next frame so scrolling with be synchronized (because currently we can't change the scrolling while the window is being rendered)
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && m_dataEditingAddr >= static_cast<size_t>(m_rows))
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
        else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) && (m_dataEditingAddr < memSize - 1))
        { 
            dataEditingAddrNext = m_dataEditingAddr + 1; 
            m_dataEditingTakeFocus = true; 
        }
    }
    if ((dataEditingAddrNext != SIZE_T_MAX) && ((dataEditingAddrNext / m_rows) != (dataEditingAddrBackup / m_rows)))
    {
        // Track cursor movements
        const int scrollOffset = (static_cast<int>(dataEditingAddrNext / m_rows) - static_cast<int>(dataEditingAddrBackup / m_rows));
        const bool scrollDesired = ((scrollOffset < 0) && (dataEditingAddrNext < visibleStartAddr + m_rows * 2)) 
                                    || ((scrollOffset > 0) && (dataEditingAddrNext > visibleEndAddr - m_rows * 2));
        if (scrollDesired)
        {
            ImGui::SetScrollY(ImGui::GetScrollY() + scrollOffset * s.LineHeight);
        }
    }

    // Draw vertical separator
    const ImVec2 windowPos = ImGui::GetWindowPos();
    if (m_optShowAscii)
    {
        drawList->AddLine(ImVec2{windowPos.x + s.PosAsciiStart - s.GlyphWidth, windowPos.y}, 
                          ImVec2{windowPos.x + s.PosAsciiStart - s.GlyphWidth, windowPos.y + 9999}, 
                          ImGui::GetColorU32(ImGuiCol_Border));
    }

    const ImU32 colorText = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32 colorDisabled = m_optGreyOutZeroes ? ImGui::GetColorU32(ImGuiCol_TextDisabled) : colorText;

    for (int iLine = clipper.DisplayStart; iLine < clipper.DisplayEnd; ++iLine) // display only visible lines
    {
        size_t addr = static_cast<size_t>(iLine * m_rows);
        ImGui::Text("%0*" _PRISizeT ": ", s.AddrDigitsCount, baseDisplayAddr + addr);

        // Draw Hexadecimal
        for (int n = 0; n < m_rows && addr < memSize; ++n, ++addr)
        {
            float bytePosX = s.PosHexStart + s.HexCellWidth * n;
            if (m_optMidRowsCount > 0)
            {
                bytePosX += (n / m_optMidRowsCount) * s.SpacingBetweenMidRows;
            }
            ImGui::SameLine(bytePosX);

            // Draw highlight
            if ((addr >= m_highlightMin && addr < m_highlightMax))
            {
                const ImVec2 pos = ImGui::GetCursorScreenPos();
                float highlightWidth = s.GlyphWidth * 2;
                bool isNextByteHighlighted = (addr + 1 < memSize) 
                                                && (((m_highlightMax != SIZE_T_MAX) && (addr + 1 < m_highlightMax)));
                if (isNextByteHighlighted || (n + 1 == m_rows))
                {
                    highlightWidth = s.HexCellWidth;
                    if (m_optMidRowsCount > 0 && n > 0 && (n + 1) < m_rows && ((n + 1) % m_optMidRowsCount) == 0)
                    {
                        highlightWidth += s.SpacingBetweenMidRows;
                    }
                }
                drawList->AddRectFilled(pos, ImVec2(pos.x + highlightWidth, pos.y + s.LineHeight), m_highlightColor);
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
                    sprintf(m_dataInputBuf.data(), "%02X", memData[addr]);
                }
                ImGui::PushItemWidth(s.GlyphWidth * 2);
                struct UserData
                {
                    // FIXME: We should have a way to retrieve the text edit cursor position more easily in the API, this is rather tedious. This is such a ugly mess we may be better off not using InputText() at all here.
                    static int Callback(ImGuiTextEditCallbackData* data)
                    {
                        UserData* userData = reinterpret_cast<UserData*>(data->UserData);
                        if (!data->HasSelection())
                        {
                            userData->CursorPos = data->CursorPos;
                        }
                        if ((data->SelectionStart == 0) && (data->SelectionEnd == data->BufTextLen))
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
                sprintf(userData.CurrentBufOverwrite, "%02X", memData[addr]);
                ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CallbackAlways;
                if (ImGui::InputText("##data", m_dataInputBuf.data(), 32, flags, UserData::Callback, &userData))
                {
                    dataWrite = dataNext = true;
                }
                else if (!m_dataEditingTakeFocus && !ImGui::IsItemActive())
                {
                    m_dataEditingAddr = dataEditingAddrNext = SIZE_T_MAX;
                }
                m_dataEditingTakeFocus = false;
                ImGui::PopItemWidth();
                if (userData.CursorPos >= 2)
                {
                    dataWrite = dataNext = true;
                }
                if (dataEditingAddrNext != SIZE_T_MAX)
                {
                    dataWrite = dataNext = false;
                }
                unsigned int dataInputValue;
                if (dataWrite && sscanf(m_dataInputBuf.data(), "%X", &dataInputValue) == 1)
                {
                    memData[addr] = static_cast<uint8_t>(dataInputValue);
                }
                ImGui::PopID();
            }
            else
            {
                // NB: The trailing space is not visible but ensure there's no gap that the mouse cannot click on.
                uint8_t b = memData[addr];

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
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
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
                m_dataEditingAddr = addr + static_cast<size_t>((ImGui::GetIO().MousePos.x - pos.x) / s.GlyphWidth);
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
                unsigned char c = memData[addr];
                char display_c = (c < 32 || c >= 128) ? '.' : c;
                drawList->AddText(pos, (display_c == '.') ? colorDisabled : colorText, &display_c, &display_c + 1);
                pos.x += s.GlyphWidth;
            }
        }
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();

    if (dataNext && (m_dataEditingAddr < memSize))
    {
        m_dataEditingAddr = m_dataEditingAddr + 1;
        m_dataEditingTakeFocus = true;
    }
    else if (dataEditingAddrNext != SIZE_T_MAX)
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
        ImGui::PopItemWidth();
        ImGui::Checkbox("Show AscII", &m_optShowAscii);
        ImGui::Checkbox("Show HexII", &m_optShowHexII);
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
            m_highlightMin = m_highlightMax = SIZE_T_MAX;
        }
    }
    ImGui::PopItemWidth();

    if (m_gotoAddr != SIZE_T_MAX)
    {
        if (m_gotoAddr < memSize)
        {
            ImGui::BeginChild("##scrolling");
            ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + (m_gotoAddr / m_rows) * ImGui::GetTextLineHeight());
            ImGui::EndChild();
            m_dataEditingAddr = m_gotoAddr;
            m_dataEditingTakeFocus = true;
        }
        m_gotoAddr = SIZE_T_MAX;
    }

    // Notify the main window of our ideal child content size (FIXME: we are missing an API to get the contents size from the child)
    ImGui::SetCursorPosX(s.WindowWidth);
}

#undef _PRISizeT