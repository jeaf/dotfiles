// All Latin-1 printable characters (189)
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞßàáâãäåæçèéêëìíîïğñòóôõö÷øùúûüışÿ

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const uint8_t* base139_symbols          = "!#$%&()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz|~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓ";
const uint8_t  base139_symbols_invmap[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,0,5,6,7,8,9,10,11,0,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,0,86,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,-128,-127,-126,-125,-124,-123,-122,-121,-120,-119,-118,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint64_t       powers_of_139[]          = {1, 139, 19321, 2685619ULL, 373301041ULL, 51888844699ULL, 7212549413161ULL, 1002544368429379ULL, 139353667211683681ULL};
const long     SYMBOL_COUNT = 139;
#define        SRC_BLK_SIZE 8
#define        ENC_BLK_SIZE 9

void base139_enc_blk(const uint8_t* blksrc, const long len)
{
    assert(blksrc);
    assert(len > 0);
    assert(len <= SRC_BLK_SIZE);
    uint8_t blkarray[ENC_BLK_SIZE] = {0}; // Use ENC_BLK_SIZE to make sure
                                          // there is enough room for storing
                                          // the encoded block.
    memcpy(blkarray, blksrc, len);
    uint64_t blk = (uint64_t)blkarray[0] << 56 |
                   (uint64_t)blkarray[1] << 48 |
                   (uint64_t)blkarray[2] << 40 |
                   (uint64_t)blkarray[3] << 32 |
                   (uint64_t)blkarray[4] << 24 |
                   (uint64_t)blkarray[5] << 16 |
                   (uint64_t)blkarray[6] << 8  |
                   (uint64_t)blkarray[7];
    for (long j = ENC_BLK_SIZE - 1; j >= 0; --j)
    {
        blkarray[j] = base139_symbols[blk % SYMBOL_COUNT];
        blk /= SYMBOL_COUNT;
    }
    for (long k = 0; k < ENC_BLK_SIZE; ++k)
    {
        printf("%c", blkarray[k]);
    }
}

void base139_dec_blk(const uint8_t* enc, const long pad)
{
    assert(enc);
    uint64_t blk = 0;
    for (long j = 0; j < ENC_BLK_SIZE; ++j)
    {
        blk += base139_symbols_invmap[enc[j]] * powers_of_139[ENC_BLK_SIZE - j - 1];
    }
                 printf("%c", (blk & 0xFF00000000000000ULL) >> 56);
    if (pad < 7) printf("%c", (blk & 0x00FF000000000000ULL) >> 48);
    if (pad < 6) printf("%c", (blk & 0x0000FF0000000000ULL) >> 40);
    if (pad < 5) printf("%c", (blk & 0x000000FF00000000ULL) >> 32);
    if (pad < 4) printf("%c", (blk & 0x00000000FF000000ULL) >> 24);
    if (pad < 3) printf("%c", (blk & 0x0000000000FF0000ULL) >> 16);
    if (pad < 2) printf("%c", (blk & 0x000000000000FF00ULL) >> 8);
    if (pad < 1) printf("%c", (blk & 0x00000000000000FFULL));
}

int main(int argc, char* argv[])
{
    // Read the first 8 bytes, decide if we need to encode or decode
    uint8_t src[ENC_BLK_SIZE] = {0};
    long len = fread(src, 1, SRC_BLK_SIZE, stdin);
    long pad = 0;
    if (strncmp(src, "{paquet:", 8) == 0)
    {
        len = fread(src + ENC_BLK_SIZE - 2, 1, 2, stdin);

        // Decode the data
        while (!feof(stdin) && pad == 0)
        {
            uint8_t curblk[ENC_BLK_SIZE] = {0};
            curblk[0] = src[ENC_BLK_SIZE - 2];
            curblk[1] = src[ENC_BLK_SIZE - 1];
            len = fread(src, 1, ENC_BLK_SIZE, stdin);
            memcpy(curblk + 2, src, ENC_BLK_SIZE - 2);

            if (src[ENC_BLK_SIZE - 1] == '}')
            {
                pad = base139_symbols_invmap[src[ENC_BLK_SIZE - 2]];
            }

            if (len > 0)
            {
                base139_dec_blk(curblk, pad);
            }
        }
    }
    else
    {
        // Encode the data
        printf("{paquet:");
        base139_enc_blk(src, len);
        long pad = SRC_BLK_SIZE - len;
        while (!feof(stdin))
        {
            len = fread(src, 1, SRC_BLK_SIZE, stdin);
            pad = SRC_BLK_SIZE - len;
            if (len > 0) base139_enc_blk(src, len);
        }
        printf("%c}", base139_symbols[pad]);
    }

    return 0;
}

