// All Latin-1 printable characters (189)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ
// 189**8 = 1.6281501e+18
// 2**60  = 1.1529215e+18
// 2**56  = 7.2057594e+16
// 128**8 = 7.2057594e+16
// 182**8 = 1.2038465e+18
// 2**64  = 1.8446744e+19
// 139**9 = 1.937016e+19

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint8_t* base139_symbols = "!#$%&()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz|~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓ";
const uint8_t base139_symbols_invmap[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,0,5,6,7,8,9,10,11,0,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,0,86,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,-128,-127,-126,-125,-124,-123,-122,-121,-120,-119,-118,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint64_t powers_of_139[] = {1, 139, 19321, 2685619ULL, 373301041ULL, 51888844699ULL, 7212549413161ULL, 1002544368429379ULL, 139353667211683681ULL};
const long SYMBOL_COUNT = 139;
const long SRC_BLK_SIZE = 8;
const long ENC_BLK_SIZE = 9;

//void gen_invmap(const char* symbols)
//{
//    assert(symbols);
//    printf("const char base139_symbols_invmap[] = {");
//    long len = strlen(symbols);
//    char invmap[256] = {0};
//    for (long i = 0; i < len; ++i)
//    {
//        invmap[(uint8_t)symbols[i]] = i;
//    }
//    for (long j = 0; j < sizeof(invmap); ++j)
//    {
//        printf("%d,", invmap[j]);
//    }
//    printf("};\n");
//}

void base139_enc(const uint8_t* src, long srclen, uint8_t* dst, long dstlen)
{
    assert(strlen(base139_symbols) == SYMBOL_COUNT);
    assert(src);
    assert(srclen > 0);
    assert(dst);

    // Figure out padding
    long quot = srclen / SRC_BLK_SIZE;
    long rem  = srclen % SRC_BLK_SIZE;
    long pad  = rem > 0 ? (SRC_BLK_SIZE - rem) : 0;
    printf("quot: %d, rem: %d, pad: %d\n", quot, rem, pad);
    
    // Compute the required space for dst. The +1 is for the terminating null.
    assert(dstlen >= (((srclen + pad) / SRC_BLK_SIZE * ENC_BLK_SIZE) + 1));

    // Process whole blocks
    for (long i = 0; i < quot; ++i)
    {
        const uint8_t* blkptr = src + SRC_BLK_SIZE * i;
        uint64_t blk = (uint64_t)blkptr[0] << 56 |
                       (uint64_t)blkptr[1] << 48 |
                       (uint64_t)blkptr[2] << 40 |
                       (uint64_t)blkptr[3] << 32 |
                       (uint64_t)blkptr[4] << 24 |
                       (uint64_t)blkptr[5] << 16 |
                       (uint64_t)blkptr[6] << 8  |
                       (uint64_t)blkptr[7];
        //printf("enc blk: %llu\n", blk);
        for (long j = ENC_BLK_SIZE - 1; j >= 0; --j)
        {
            //printf("enc symb: %x, digit: %x\n", base139_symbols[blk % SYMBOL_COUNT], blk % SYMBOL_COUNT);
            (dst + i * ENC_BLK_SIZE)[j] = base139_symbols[blk % SYMBOL_COUNT];
            blk /= SYMBOL_COUNT;
        }
    }

    // Process last block
    if (pad > 0)
    {
        uint64_t blk = 0;
        for (long k = 0; k < (SRC_BLK_SIZE - pad); ++k)
        {
            blk |= (uint64_t)(src + SRC_BLK_SIZE * quot)[k] << ((SRC_BLK_SIZE - k - 1) * 8);
        }

        for (long j = ENC_BLK_SIZE - 1; j >= 0; --j)
        {
            (dst + quot * ENC_BLK_SIZE)[j] = base139_symbols[blk % SYMBOL_COUNT];
            blk /= SYMBOL_COUNT;
        }
    }

    // Put the terminating null and return size
    uint8_t* lastbyte = dst + quot * ENC_BLK_SIZE;
    if (pad > 0) lastbyte += ENC_BLK_SIZE - pad;
    *lastbyte = 0;
}

long base139_dec(const uint8_t* enc, uint8_t* dst, long dstlen)
{
    long enclen = strlen(enc);
    assert(enc);
    assert(enclen > 0);
    assert(dst);

    // Figure out padding
    long quot = enclen / ENC_BLK_SIZE;
    long rem  = enclen % ENC_BLK_SIZE;
    long pad  = rem > 0 ? (ENC_BLK_SIZE - rem) : 0;
    printf("quot: %d, rem: %d, pad: %d\n", quot, rem, pad);
    
    // Compute the required space for dst
    assert(dstlen >= (((enclen + pad) / ENC_BLK_SIZE * SRC_BLK_SIZE)));

    // Process whole blocks
    for (long i = 0; i < quot; ++i)
    {
        uint64_t blk = 0;
        for (long j = 0; j < ENC_BLK_SIZE; ++j)
        {
            uint8_t symbol = (enc + i * ENC_BLK_SIZE)[j];
            blk += base139_symbols_invmap[symbol] * powers_of_139[ENC_BLK_SIZE - j - 1];
        }
        //printf("dec blk: %llu\n", blk);
        uint8_t* dstptr = dst + i * SRC_BLK_SIZE;
        dstptr[0] = (blk & 0xFF00000000000000ULL) >> 56;
        dstptr[1] = (blk & 0x00FF000000000000ULL) >> 48;
        dstptr[2] = (blk & 0x0000FF0000000000ULL) >> 40;
        dstptr[3] = (blk & 0x000000FF00000000ULL) >> 32;
        dstptr[4] = (blk & 0x00000000FF000000ULL) >> 24;
        dstptr[5] = (blk & 0x0000000000FF0000ULL) >> 16;
        dstptr[6] = (blk & 0x000000000000FF00ULL) >> 8;
        dstptr[7] = (blk & 0x00000000000000FFULL);
    }

    // Process last block
    if (pad > 0)
    {
        uint64_t blk = 0;
        for (long j = 0; j < ENC_BLK_SIZE; ++j)
        {
            uint8_t symbol = j < pad ? (enc + quot * ENC_BLK_SIZE)[j] : base139_symbols[SYMBOL_COUNT - 1];
            blk += base139_symbols_invmap[symbol] * powers_of_139[ENC_BLK_SIZE - j - 1];
        }
        uint8_t* dstptr = dst + quot * SRC_BLK_SIZE;
        dstptr[0] = (blk & 0xFF00000000000000ULL) >> 56;
        dstptr[1] = (blk & 0x00FF000000000000ULL) >> 48;
        dstptr[2] = (blk & 0x0000FF0000000000ULL) >> 40;
        dstptr[3] = (blk & 0x000000FF00000000ULL) >> 32;
        dstptr[4] = (blk & 0x00000000FF000000ULL) >> 24;
        dstptr[5] = (blk & 0x0000000000FF0000ULL) >> 16;
        dstptr[6] = (blk & 0x000000000000FF00ULL) >> 8;
        dstptr[7] = (blk & 0x00000000000000FFULL);

        return (quot + 1) * SRC_BLK_SIZE - pad;
    }

    return quot * SRC_BLK_SIZE;
}

int main()
{
    uint8_t src[] = "aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffff";
    //uint8_t src[] = {'a', 'b', 0, 0, 0, 0, 0, 0};
    uint8_t dst[100] = {0};
    uint8_t dec[100] = {0};
    //long enc_len = base139_enc(src, strlen(src), dst, sizeof(dst));
    printf("sizeof(src): %d\n", sizeof(src));
    base139_enc(src, sizeof(src), dst, sizeof(dst));
    printf("Encoded len: %d\n", strlen(dst));
    printf("Src    : %s\n", src);
    printf("Encoded: %s\n", dst);
    long len = base139_dec(dst, dec, sizeof(dec));
    printf("Returned len: %d\n", len);
    dec[len] = 0;
    printf("Decoded: %s\n", dec);

    return 0;
}

