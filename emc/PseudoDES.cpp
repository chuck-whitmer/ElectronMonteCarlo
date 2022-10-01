#include <stdio.h>
#include "PseudoDES.h"


uint32_t PseudoDES::c1[4] = { 0xBAA96887,0x1E17D32C,0x03BCDC3C,0x0F33D1B2 };
uint32_t PseudoDES::c2[4] = { 0x4B0F3B58,0xE874F0C3,0x6955C5A6,0x55A7CA46 };
uint32_t PseudoDES::Rand32()
{
    uint32_t kk0, kk1, kk2, iA, iB;

    iA = m_iNum ^ c1[0];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk0 = m_iSeq ^ ((Xchg16(iB) ^ c2[0]) + Low16(iA) * High16(iA));

    iA = kk0 ^ c1[1];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk1 = m_iNum ^ ((Xchg16(iB) ^ c2[1]) + Low16(iA) * High16(iA));

    if (++m_iNum == 0) m_iSeq++;

    iA = kk1 ^ c1[2];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk2 = kk0 ^ ((Xchg16(iB) ^ c2[2]) + Low16(iA) * High16(iA));

    iA = kk2 ^ c1[3];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    return kk1 ^ ((Xchg16(iB) ^ c2[3]) + Low16(iA) * High16(iA));
}

uint64_t PseudoDES::Rand64()
{
    uint32_t kk0, kk1, kk2, kk3, iA, iB;

    iA = m_iNum ^ c1[0];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk0 = m_iSeq ^ ((Xchg16(iB) ^ c2[0]) + Low16(iA) * High16(iA));

    iA = kk0 ^ c1[1];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk1 = m_iNum ^ ((Xchg16(iB) ^ c2[1]) + Low16(iA) * High16(iA));

    iA = kk1 ^ c1[2];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk2 = kk0 ^ ((Xchg16(iB) ^ c2[2]) + Low16(iA) * High16(iA));

    iA = kk2 ^ c1[3];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk3 = kk1 ^ ((Xchg16(iB) ^ c2[3]) + Low16(iA) * High16(iA));
    if (++m_iNum == 0) m_iSeq++;

    return Make64(kk3, kk2);
}

uint64_t PseudoDES::Hash64(uint64_t index)
{
    uint32_t kk0, kk1, kk2, kk3, iA, iB;

    iA = Low32(index) ^ c1[0];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk0 = High32(index) ^ ((Xchg16(iB) ^ c2[0]) + Low16(iA) * High16(iA));

    iA = kk0 ^ c1[1];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk1 = Low32(index) ^ ((Xchg16(iB) ^ c2[1]) + Low16(iA) * High16(iA));

    iA = kk1 ^ c1[2];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk2 = kk0 ^ ((Xchg16(iB) ^ c2[2]) + Low16(iA) * High16(iA));

    iA = kk2 ^ c1[3];
    iB = Low16(iA) * Low16(iA) + ~(High16(iA) * High16(iA));
    kk3 = kk1 ^ ((Xchg16(iB) ^ c2[3]) + Low16(iA) * High16(iA));

    return Make64(kk3, kk2);
}

const uint32_t PseudoDES::testData[4][4] =
{
    { 1, 1,0x604D1DCE,0x509C0C23},
    { 1,99,0xD97F8571,0xA66CB41A},
    {99, 1,0x7822309D,0x64300984},
    {99,99,0xD7F376F0,0x59BA89EB}
};

bool PseudoDES::Test()
{
    printf("eMax is %llX\n", *(int64_t *)&PseudoDES::eMax);

    bool bSuccess = true;

    for (int ii = 0; ii < 4; ii++)
    {
        bSuccess &= (Hash64(Make64(testData[ii][1], testData[ii][0])) == Make64(testData[ii][3], testData[ii][2]));
    }

    uint32_t iSaveSeq = m_iSeq;
    uint32_t iSaveNum = m_iNum;

    for (int ii = 0; ii < 4; ii++)
    {
        m_iSeq = testData[ii][0]; m_iNum = testData[ii][1];
        bSuccess &= (Rand32() == testData[ii][3]);
    }

    for (int ii = 0; ii < 10; ii++)
        printf("%.8f %.8f %.8f %.8f %.8f\n", RandomDouble(), RandomDouble(), RandomDouble(), RandomDouble(), RandomDouble());

    m_iSeq = iSaveSeq;
    m_iNum = iSaveNum;
    return bSuccess;
}
