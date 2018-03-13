#include "ram.h"

#include "memorymap.h"

#include <cassert>

using namespace PSEmu;

// RAM contains garbage by default
RAM::RAM() : m_data(RAM_SIZE, 0xCA) { }
