// All Latin-1 printable characters (189)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞßàáâãäåæçèéêëìíîïğñòóôõö÷øùúûüışÿ
// 189**8 = 1.6281501e+18
// 2**60  = 1.1529215e+18
// 2**56  = 7.2057594e+16
// 128**8 = 7.2057594e+16
// 182**8 = 1.2038465e+18
// 2**64  = 1.8446744e+19
// 139**9 = 1.937016e+19

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* base139_symbols = "!#$%&()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz|~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓ";
const long SYMBOL_COUNT = 139;
const long SRC_BLK_SIZE = 8;
const long DST_BLK_SIZE = 9;

long base139_enc(const uint8_t* src, long srclen, uint8_t* dst, long dstlen)
{
    assert(strlen(base139_symbols) == SYMBOL_COUNT);
    assert(src);
    assert(srclen > 0);
    assert(dst);

    // Figure out padding
    long quot = srclen / SRC_BLK_SIZE;
    long rem  = srclen % SRC_BLK_SIZE;
    long pad  = rem > 0 ? (SRC_BLK_SIZE - rem) : 0;
    
    // Compute the required space for dst. The +1 is for the terminating null.
    assert(dstlen >= (((srclen + pad) / SRC_BLK_SIZE * DST_BLK_SIZE) + 1));

    // Process whole blocks
    for (long i = 0; i < quot; ++i)
    {
        uint64_t blk = *((uint64_t*)(src + SRC_BLK_SIZE * i));
        for (long j = DST_BLK_SIZE - 1; j >= 0; --j)
        {
            (dst + i * DST_BLK_SIZE)[j] = base139_symbols[blk % SYMBOL_COUNT];
            blk /= SYMBOL_COUNT;
        }
    }

    // Process last block (must be big-endian)
    if (pad > 0)
    {
        uint64_t blk = 0;
        for (long k = 0; k < (SRC_BLK_SIZE - pad); ++k)
        {
            blk |= (uint64_t)(src + SRC_BLK_SIZE * quot)[k] << ((SRC_BLK_SIZE - k - 1) * 8);
        }

        for (long j = DST_BLK_SIZE - 1; j >= 0; --j)
        {
            printf("blk: %llx\n", blk);
            (dst + quot * DST_BLK_SIZE)[j] = base139_symbols[blk % SYMBOL_COUNT];
            blk /= SYMBOL_COUNT;
        }
    }

    // Put the terminating null
    (dst + quot * DST_BLK_SIZE)[DST_BLK_SIZE - pad] = 0;

    return 0;
}

int main()
{
    uint8_t src[] = "abcdefgh";
    uint8_t dst[37] = {0};
    base139_enc(src, strlen(src), dst, sizeof(dst));
    printf("Src    : %s\n", src);
    printf("Encoded: %s\n", dst);

    return 0;
}

