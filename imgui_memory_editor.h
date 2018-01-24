#pragma once

#include "3rd party/imgui/imgui.h"

#include <array>
#include <functional>
#include <limits>
#include <stdio.h>

struct MemoryEditor
{
    using u8 = unsigned char;

    // Settings
    bool            Open;                           // = true   // set to false when DrawWindow() was closed. ignore if not using DrawWindow
    bool            ReadOnly;                       // = false  // set to true to disable any editing
    int             Rows;                           // = 16     //
    bool            OptShowAscii;                   // = true   //
    bool            OptShowHexII;                   // = false  //
    bool            OptGreyOutZeroes;               // = true   //
    int             OptMidRowsCount;                // = 8      // set to 0 to disable extra spacing between every mid-rows
    int             OptAddrDigitsCount;             // = 0      // number of addr digits to display (default calculated based on maximum displayed addr)
    ImU32           HighlightColor;                 //          // color of highlight

    std::function<u8(u8*, size_t)> ReadFn;          // = NULL   // optional handler to read bytes
    std::function<void(u8*, size_t, u8)> WriteFn;   // = NULL   // optional handler to write bytes
    std::function<bool(u8*, size_t)> HighlightFn;   // = NULL   // optional handler to return Highlight property (to support non-contiguous highlighting)

    // State/Internals
    bool            ContentsWidthChanged;
    size_t          DataEditingAddr;
    bool            DataEditingTakeFocus;
    std::array<char, 32> DataInputBuf;
    std::array<char, 32> AddrInputBuf;
    size_t          GotoAddr;
    size_t          HighlightMax, HighlightMin;

    MemoryEditor() 
        // Settings
        : Open{ true }
        , ReadOnly{ false }
        , Rows{ 16 }
        , OptShowAscii{ true }
        , OptShowHexII{ false }
        , OptGreyOutZeroes{ true }
        , OptMidRowsCount{ 8 }
        , OptAddrDigitsCount{ 0 }
        , HighlightColor{ IM_COL32(255, 255, 255, 40) }
        , ReadFn{}
        , WriteFn{}
        , HighlightFn{}
        // State/Internals
        , ContentsWidthChanged{ false }
        , DataEditingAddr{ (size_t)-1 }
        , DataEditingTakeFocus{ false }
        , DataInputBuf{}
        , AddrInputBuf{}
        , GotoAddr{ (size_t)-1 }
        , HighlightMax{ (size_t)-1 }
        , HighlightMin{ (size_t)-1 }
    { }

    void GotoAddrAndHighlight(size_t addrMin, size_t addrMax)
    {
        GotoAddr = addrMin;
        HighlightMin = addrMin;
        HighlightMax = addrMax;
    }

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

    void CalcSizes(Sizes& s, size_t memSize, size_t baseDisplayAddr)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        s.AddrDigitsCount = OptAddrDigitsCount;
        if (s.AddrDigitsCount == 0)
        {
            for (size_t n = baseDisplayAddr + memSize - 1; n > 0; n >>= 4)
            {
                ++s.AddrDigitsCount;
            }
        }
            
        s.LineHeight = ImGui::GetTextLineHeight();
        s.GlyphWidth = ImGui::CalcTextSize("F").x + 1;                  // We assume the font is mono-space
        s.HexCellWidth = (float)(int)(s.GlyphWidth * 2.5f);             // "FF " we include trailing space in the width to easily catch clicks everywhere
        s.SpacingBetweenMidRows = (float)(int)(s.HexCellWidth * 0.25f); // Every OptMidRowsCount columns we add a bit of extra spacing
        s.PosHexStart = (s.AddrDigitsCount + 2) * s.GlyphWidth;
        s.PosHexEnd = s.PosHexStart + (s.HexCellWidth * Rows);
        s.PosAsciiStart = s.PosAsciiEnd = s.PosHexEnd;
        if (OptShowAscii)
        {
            s.PosAsciiStart = s.PosHexEnd + s.GlyphWidth * 1;
            if (OptMidRowsCount > 0)
            {
                s.PosAsciiStart += ((Rows + OptMidRowsCount - 1) / OptMidRowsCount) * s.SpacingBetweenMidRows;
            }
            s.PosAsciiEnd = s.PosAsciiStart + Rows * s.GlyphWidth;
        }
        s.WindowWidth = s.PosAsciiEnd + style.ScrollbarSize + style.WindowPadding.x * 2 + s.GlyphWidth;
    }

#ifdef _MSC_VER
#define _PRISizeT   "IX"
#else
#define _PRISizeT   "zX"
#endif

    // Standalone Memory Editor window
    void DrawWindow(const char* title, u8* memData, size_t memSize, size_t baseDisplayAddr = 0x0000)
    {
        Sizes s;
        CalcSizes(s, memSize, baseDisplayAddr);
        ImGui::SetNextWindowSizeConstraints(ImVec2{0.0f, 0.0f}, ImVec2{s.WindowWidth, std::numeric_limits<float>::max()});

        Open = true;
        if (ImGui::Begin(title, &Open, ImGuiWindowFlags_NoScrollbar))
        {
            if (ImGui::IsRootWindowOrAnyChildHovered() && ImGui::IsMouseClicked(1))
            {
                ImGui::OpenPopup("context");
            }
            DrawContents(memData, memSize, baseDisplayAddr);
            if (ContentsWidthChanged)
            {
                CalcSizes(s, memSize, baseDisplayAddr);
                ImGui::SetWindowSize(ImVec2(s.WindowWidth, ImGui::GetWindowSize().y));
            }
        }
        ImGui::End();
    }

    // Memory Editor contents only
    void DrawContents(u8* memData, size_t memSize, size_t baseDisplayAddr = 0x0000)
    {
        Sizes s;
        CalcSizes(s, memSize, baseDisplayAddr);
        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::BeginChild("##scrolling", ImVec2{0, -ImGui::GetFrameHeightWithSpacing()});
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0.f, 0.f});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});

        const int lineTotalCount = (int)((memSize + Rows - 1) / Rows);
        ImGuiListClipper clipper(lineTotalCount, s.LineHeight);
        const size_t visibleStartAddr = clipper.DisplayStart * Rows;
        const size_t visibleEndAddr = clipper.DisplayEnd * Rows;

        bool dataNext = false;

        if (ReadOnly || DataEditingAddr >= memSize)
        {
            DataEditingAddr = (size_t)-1;
        }

        size_t dataEditingAddrBackup = DataEditingAddr;
        size_t dataEditingAddrNext = (size_t)-1;
        if (DataEditingAddr != (size_t)-1)
        {
            // Move cursor but only apply on next frame so scrolling with be synchronized (because currently we can't change the scrolling while the window is being rendered)
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && DataEditingAddr >= (size_t)Rows)          
            { 
                dataEditingAddrNext = DataEditingAddr - Rows; 
                DataEditingTakeFocus = true; 
            }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) && DataEditingAddr < memSize - Rows) 
            { 
                dataEditingAddrNext = DataEditingAddr + Rows; 
                DataEditingTakeFocus = true; 
            }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)) && DataEditingAddr > 0)               
            { 
                dataEditingAddrNext = DataEditingAddr - 1; 
                DataEditingTakeFocus = true; 
            }
            else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) && DataEditingAddr < memSize - 1)   
            { 
                dataEditingAddrNext = DataEditingAddr + 1; 
                DataEditingTakeFocus = true; 
            }
        }
        if (dataEditingAddrNext != (size_t)-1 && (dataEditingAddrNext / Rows) != (dataEditingAddrBackup / Rows))
        {
            // Track cursor movements
            const int scrollOffset = ((int)(dataEditingAddrNext / Rows) - (int)(dataEditingAddrBackup / Rows));
            const bool scrollDesired = (scrollOffset < 0 && dataEditingAddrNext < visibleStartAddr + Rows * 2) || (scrollOffset > 0 && dataEditingAddrNext > visibleEndAddr - Rows * 2);
            if (scrollDesired)
            {
                ImGui::SetScrollY(ImGui::GetScrollY() + scrollOffset * s.LineHeight);
            }
        }

        // Draw vertical separator
        ImVec2 windowPos = ImGui::GetWindowPos();
        if (OptShowAscii)
        {
            drawList->AddLine(ImVec2(windowPos.x + s.PosAsciiStart - s.GlyphWidth, windowPos.y), ImVec2(windowPos.x + s.PosAsciiStart - s.GlyphWidth, windowPos.y + 9999), ImGui::GetColorU32(ImGuiCol_Border));
        }

        const ImU32 colorText = ImGui::GetColorU32(ImGuiCol_Text);
        const ImU32 colorDisabled = OptGreyOutZeroes ? ImGui::GetColorU32(ImGuiCol_TextDisabled) : colorText;

        for (int iLine = clipper.DisplayStart; iLine < clipper.DisplayEnd; iLine++) // display only visible lines
        {
            size_t addr = (size_t)(iLine * Rows);
            ImGui::Text("%0*" _PRISizeT ": ", s.AddrDigitsCount, baseDisplayAddr + addr);

            // Draw Hexadecimal
            for (int n = 0; n < Rows && addr < memSize; ++n, ++addr)
            {
                float byte_pos_x = s.PosHexStart + s.HexCellWidth * n;
                if (OptMidRowsCount > 0)
                {
                    byte_pos_x += (n / OptMidRowsCount) * s.SpacingBetweenMidRows;
                }
                ImGui::SameLine(byte_pos_x);

                // Draw highlight
                if ((addr >= HighlightMin && addr < HighlightMax) || (HighlightFn && HighlightFn(memData, addr)))
                {
                    ImVec2 pos = ImGui::GetCursorScreenPos();
                    float highlight_width = s.GlyphWidth * 2;
                    bool is_next_byte_highlighted =  (addr + 1 < memSize) && ((HighlightMax != (size_t)-1 && addr + 1 < HighlightMax) || (HighlightFn && HighlightFn(memData, addr + 1)));
                    if (is_next_byte_highlighted || (n + 1 == Rows))
                    {
                        highlight_width = s.HexCellWidth;
                        if (OptMidRowsCount > 0 && n > 0 && (n + 1) < Rows && ((n + 1) % OptMidRowsCount) == 0)
                            highlight_width += s.SpacingBetweenMidRows;
                    }
                    drawList->AddRectFilled(pos, ImVec2(pos.x + highlight_width, pos.y + s.LineHeight), HighlightColor);
                }

                if (DataEditingAddr == addr)
                {
                    // Display text input on current byte
                    bool dataWrite = false;
                    ImGui::PushID((void*)addr);
                    if (DataEditingTakeFocus)
                    {
                        ImGui::SetKeyboardFocusHere();
                        ImGui::CaptureKeyboardFromApp(true);
                        sprintf(AddrInputBuf.data(), "%0*" _PRISizeT, s.AddrDigitsCount, baseDisplayAddr + addr);
                        sprintf(DataInputBuf.data(), "%02X", ReadFn ? ReadFn(memData, addr) : memData[addr]);
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
                    sprintf(userData.CurrentBufOverwrite, "%02X", ReadFn ? ReadFn(memData, addr) : memData[addr]);
                    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CallbackAlways;
                    if (ImGui::InputText("##data", DataInputBuf.data(), 32, flags, UserData::Callback, &userData))
                    {
                        dataWrite = dataNext = true;
                    }
                    else if (!DataEditingTakeFocus && !ImGui::IsItemActive())
                    {
                        DataEditingAddr = dataEditingAddrNext = (size_t)-1;
                    }
                    DataEditingTakeFocus = false;
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
                    if (dataWrite && sscanf(DataInputBuf.data(), "%X", &data_input_value) == 1)
                    {
                        if (WriteFn)
                        {
                            WriteFn(memData, addr, (u8)data_input_value);
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
                    u8 b = ReadFn ? ReadFn(memData, addr) : memData[addr];

                    if (OptShowHexII)
                    {
                        if ((b >= 32 && b < 128))
                        {
                            ImGui::Text(".%c ", b);
                        }
                        else if (b == 0xFF && OptGreyOutZeroes)
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
                        if (b == 0 && OptGreyOutZeroes)
                        {
                            ImGui::TextDisabled("00 ");
                        }
                        else
                        {
                            ImGui::Text("%02X ", b);
                        }
                    }
                    if (!ReadOnly && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                    {
                        DataEditingTakeFocus = true;
                        dataEditingAddrNext = addr;
                    }
                }
            }

            if (OptShowAscii)
            {
                // Draw ASCII values
                ImGui::SameLine(s.PosAsciiStart);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                addr = iLine * Rows;
                ImGui::PushID(iLine);
                if (ImGui::InvisibleButton("ascii", ImVec2(s.PosAsciiEnd - s.PosAsciiStart, s.LineHeight)))
                {
                    DataEditingAddr = addr + (size_t)((ImGui::GetIO().MousePos.x - pos.x) / s.GlyphWidth);
                    DataEditingTakeFocus = true;
                }
                ImGui::PopID();
                for (int n = 0; n < Rows && addr < memSize; n++, addr++)
                {
                    if (addr == DataEditingAddr)
                    {
                        drawList->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_FrameBg));
                        drawList->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_TextSelectedBg));
                    }
                    unsigned char c = ReadFn ? ReadFn(memData, addr) : memData[addr];
                    char display_c = (c < 32 || c >= 128) ? '.' : c;
                    drawList->AddText(pos, (display_c == '.') ? colorDisabled : colorText, &display_c, &display_c + 1);
                    pos.x += s.GlyphWidth;
                }
            }
        }
        clipper.End();
        ImGui::PopStyleVar(2);
        ImGui::EndChild();

        if (dataNext && DataEditingAddr < memSize)
        {
            DataEditingAddr = DataEditingAddr + 1;
            DataEditingTakeFocus = true;
        }
        else if (dataEditingAddrNext != (size_t)-1)
        {
            DataEditingAddr = dataEditingAddrNext;
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
            if (ImGui::DragInt("##rows", &Rows, 0.2f, 4, 32, "%.0f rows")) 
            {
                ContentsWidthChanged = true;
            }
            ImGui::PopItemWidth();
            ImGui::Checkbox("Show HexII", &OptShowHexII);
            if (ImGui::Checkbox("Show Ascii", &OptShowAscii))
            {
                ContentsWidthChanged = true;
            } 
            ImGui::Checkbox("Grey out zeroes", &OptGreyOutZeroes);
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::Text("Range %0*" _PRISizeT "..%0*" _PRISizeT, s.AddrDigitsCount, baseDisplayAddr, s.AddrDigitsCount, baseDisplayAddr + memSize - 1);
        ImGui::SameLine();
        ImGui::PushItemWidth((s.AddrDigitsCount + 1) * s.GlyphWidth + style.FramePadding.x * 2.0f);
        if (ImGui::InputText("##addr", AddrInputBuf.data(), 32, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            size_t goto_addr;
            if (sscanf(AddrInputBuf.data(), "%" _PRISizeT, &goto_addr) == 1)
            {
                GotoAddr = goto_addr - baseDisplayAddr;
                HighlightMin = HighlightMax = (size_t)-1;
            }
        }
        ImGui::PopItemWidth();

        if (GotoAddr != (size_t)-1)
        {
            if (GotoAddr < memSize)
            {
                ImGui::BeginChild("##scrolling");
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + (GotoAddr / Rows) * ImGui::GetTextLineHeight());
                ImGui::EndChild();
                DataEditingAddr = GotoAddr;
                DataEditingTakeFocus = true;
            }
            GotoAddr = (size_t)-1;
        }

        // Notify the main window of our ideal child content size (FIXME: we are missing an API to get the contents size from the child)
        ImGui::SetCursorPosX(s.WindowWidth);
    }
};
#undef _PRISizeT