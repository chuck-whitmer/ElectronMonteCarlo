#pragma once
#include <stdint.h>

class PseudoDES
{
    static uint32_t c1[4];
    static uint32_t c2[4];

    // Method versions of old macros.
    static uint32_t Low16(uint32_t x) { return x & 0xFFFF; }
    static uint32_t High16(uint32_t x) { return x >> 16; }
    static uint32_t Xchg16(uint32_t x) { return (Low16(x) << 16) | High16(x); }
    static uint32_t Low32(uint64_t x) { return (uint32_t)x; }
    static uint32_t High32(uint64_t x) { return (uint32_t)(x >> 32); }
    static uint64_t Make64(uint32_t lo, uint32_t hi) { return (((uint64_t)hi) << 32) + ((uint64_t)lo); }

    // The seed.
    uint32_t m_iNum;
    uint32_t m_iSeq;

public:
    PseudoDES()
    {
        m_iNum = 1;
        m_iSeq = 1;
    }

    PseudoDES(uint32_t iNum, uint32_t iSeq)
    {
        m_iNum = iNum;
        m_iSeq = iSeq;
    }

    PseudoDES(uint64_t iSeed)
    {
        m_iNum = Low32(iSeed);
        m_iSeq = High32(iSeed);
    }

    uint32_t Element()
    {
        return m_iNum; 
    }

    uint32_t Sequence()
    {
        return m_iSeq; 
    }

    uint64_t Seed()
    {
        return Make64(m_iNum,m_iSeq); 
    }

    uint32_t Rand32();
    uint64_t Rand64();

    static constexpr double eMax = 18446744073709551616.0;  // 2^64

    double RandomDouble()
    {
        return Rand64() / eMax;
    }

    static uint64_t Hash64(uint64_t index);

    static const uint32_t testData[4][4];
    bool Test();
};

