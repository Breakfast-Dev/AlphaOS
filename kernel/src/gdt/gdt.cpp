#include "gdt.h"

__attribute((aligned(0x1000)))
GDT DefaultGDT = {
    {0, 0, 0, 0x00, 0x00, 0}, // Null
    {0, 0, 0, 0x9a, 0xa0, 0}, // Kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // Kernel data segment
    {0, 0, 0, 0x00, 0x00, 0}, // User null
    {0, 0, 0, 0x9a, 0xa0, 0}, // User code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // User data segment
};