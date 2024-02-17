#include "Precompile.h"
#include "UtilityForBit.h"

// How many bits are standing?
UINT Utl::CountBits(uint64_t b) {
    b = (b & 0x5555555555555555) + ((b >> 1)  & 0x5555555555555555);
    b = (b & 0x3333333333333333) + ((b >> 2)  & 0x3333333333333333);
    b = (b & 0x0f0f0f0f0f0f0f0f) + ((b >> 4)  & 0x0f0f0f0f0f0f0f0f);
    b = (b & 0x00ff00ff00ff00ff) + ((b >> 8)  & 0x00ff00ff00ff00ff);
    b = (b & 0x0000ffff0000ffff) + ((b >> 16) & 0x0000ffff0000ffff);
    b = (b & 0x00000000ffffffff) + ((b >> 32) & 0x00000000ffffffff);
    return (UINT)(b);
}

// How many bits are standing?
UINT Utl::CountBits(uint32_t b) {
    b = (b & 0x55555555) + ((b >> 1)  & 0x55555555);
    b = (b & 0x33333333) + ((b >> 2)  & 0x33333333);
    b = (b & 0x0f0f0f0f) + ((b >> 4)  & 0x0f0f0f0f);
    b = (b & 0x00ff00ff) + ((b >> 8)  & 0x00ff00ff);
    b = (b & 0x0000ffff) + ((b >> 16) & 0x0000ffff);
    return b;
}
