// All Latin-1 printable characters (189)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~�����������������������������������������������������������������������������������������������

#include "miniz.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const uint8_t* base139_symbols          = "!#$%&()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz|~���������������������������������������������������";
const uint8_t  base139_symbols_invmap[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,0,5,6,7,8,9,10,11,0,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,0,86,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,-128,-127,-126,-125,-124,-123,-122,-121,-120,-119,-118,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint64_t       powers_of_139[]          = {1, 139, 19321, 2685619ULL, 373301041ULL, 51888844699ULL, 7212549413161ULL, 1002544368429379ULL, 139353667211683681ULL};
const long     SYMBOL_COUNT = 139;
const long     SRC_BLK_SIZE = 8;
const long     ENC_BLK_SIZE = 9;

void base139_enc(const uint8_t* src, const long srclen, uint8_t* dst, const long dstlen)
{
    assert(strlen(base139_symbols) == SYMBOL_COUNT);
    assert(src);
    assert(srclen > 0);
    assert(dst);

    // Figure out padding
    long quot = srclen / SRC_BLK_SIZE;
    long rem  = srclen % SRC_BLK_SIZE;
    long pad  = rem > 0 ? (SRC_BLK_SIZE - rem) : 0;
    
    // Compute the required space for dst. The +2 is for the terminating null,
    // and the byte that contains the padding count.
    assert(dstlen >= (((srclen + pad) / SRC_BLK_SIZE * ENC_BLK_SIZE) + 2));

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
        for (long j = ENC_BLK_SIZE - 1; j >= 0; --j)
        {
            (dst + i * ENC_BLK_SIZE)[j] = base139_symbols[blk % SYMBOL_COUNT];
            blk /= SYMBOL_COUNT;
        }
    }

    // Process last block
    if (pad > 0)
    {
        uint8_t lastblk[8] = {0};
        memcpy(lastblk, src + SRC_BLK_SIZE * quot, SRC_BLK_SIZE - pad);
        uint64_t blk = (uint64_t)lastblk[0] << 56 |
                       (uint64_t)lastblk[1] << 48 |
                       (uint64_t)lastblk[2] << 40 |
                       (uint64_t)lastblk[3] << 32 |
                       (uint64_t)lastblk[4] << 24 |
                       (uint64_t)lastblk[5] << 16 |
                       (uint64_t)lastblk[6] << 8  |
                       (uint64_t)lastblk[7];
        for (long j = ENC_BLK_SIZE - 1; j >= 0; --j)
        {
            (dst + quot * ENC_BLK_SIZE)[j] = base139_symbols[blk % SYMBOL_COUNT];
            blk /= SYMBOL_COUNT;
        }
    }

    // Put the padding count byte and the terminating null
    uint8_t* lastbyte = dst + quot * ENC_BLK_SIZE;
    if (pad > 0) lastbyte += ENC_BLK_SIZE;
    *lastbyte++ = base139_symbols[pad];
    *lastbyte = 0;
}

long base139_dec(const uint8_t* enc, uint8_t* dst, long dstlen)
{
    assert(enc);
    long enclen = strlen(enc);
    assert(enclen > 0);
    assert(dst);

    // Compute number of blocks
    long quot = enclen / ENC_BLK_SIZE;
    long rem  = enclen % ENC_BLK_SIZE;
    assert(rem == 1); // The pad count byte
    long pad = base139_symbols_invmap[enc[enclen - 1]];
    
    // Compute the required space for dst
    assert(dstlen >= (((enclen + pad) / ENC_BLK_SIZE * SRC_BLK_SIZE)));

    // Process blocks
    for (long i = 0; i < quot; ++i)
    {
        uint64_t blk = 0;
        for (long j = 0; j < ENC_BLK_SIZE; ++j)
        {
            uint8_t symbol = (enc + i * ENC_BLK_SIZE)[j];
            blk += base139_symbols_invmap[symbol] * powers_of_139[ENC_BLK_SIZE - j - 1];
        }
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

    return quot * SRC_BLK_SIZE - pad;
}

int main(int argc, char* argv[])
{
    // Read stdin into buffer
    long     buflen = 1024;
    uint8_t* buf    = malloc(buflen);
    long tot_len    = fread(buf, 1, buflen, stdin);
    while (!feof(stdin))
    {
        long old_len = buflen;
        buflen *= 2;
        buf = realloc(buf, buflen);
        tot_len += fread(buf + old_len, 1, buflen - old_len, stdin);
    }
    
    // Determine if encoding of decoding
    if (tot_len > 8 && strncmp(buf, "{paquet:", 8) == 0)
    {
        // Find the end of the paquet
        char* back = buf + tot_len - 1;
        while (back != buf)
        {
            --back;
            if (isspace(*back) || *back == '}') *back = '\0';
            else break;
        }

        // Decode it
        uint8_t* dec = malloc(tot_len * 2);
        long len = base139_dec(buf + 8, dec, tot_len * 2);

        // Decompress it
        long uncomp_len = tot_len * 20;
        uint8_t* uncomp_dec = malloc(uncomp_len);
        mz_uncompress(uncomp_dec, &uncomp_len, dec, tot_len);
        uncomp_dec[uncomp_len] = 0;

        // Write to stdout
        printf("%s\n", uncomp_dec);
    }
    else
    {
        // Compress data
        long comp_len = mz_compressBound(tot_len);
        uint8_t* compbuf = malloc(comp_len);
        mz_compress(compbuf, &comp_len, buf, tot_len);

        // Encode data
        uint8_t* dst = malloc(tot_len * 20);
        base139_enc(compbuf, comp_len, dst, tot_len * 20);

        // Write to stdout
        printf("{paquet:%s}\n", dst);
    }

    return 0;
}
