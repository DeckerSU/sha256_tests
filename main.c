#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>  // for sleep
#include "cryptoext/sha256.h"
#include "curve25519.h"

#define NUM_ITER 1000
//#define NUM_ITER 1

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

int32_t init_hexbytes_noT(char *hexbytes,uint8_t *message,long len);
void vcalc_sha256_hardware(char hashstr[(256 >> 3) * 2 + 1],uint8_t hash[256 >> 3],uint8_t *src,int32_t len);

/* iguana_utils.c */

char hexbyte(int32_t c)
{
    c &= 0xf;
    if ( c < 10 )
        return('0'+c);
    else if ( c < 16 )
        return('a'+c-10);
    else return(0);
}

int32_t init_hexbytes_noT(char *hexbytes,unsigned char *message,long len)
{
    int32_t i;
    if ( len <= 0 )
    {
        hexbytes[0] = 0;
        return(1);
    }

    for (i=0; i<len; i++)
    {
        hexbytes[i*2] = hexbyte((message[i]>>4) & 0xf);
        hexbytes[i*2 + 1] = hexbyte(message[i] & 0xf);
        //snprintf(hexbytes+i*2, 3, "%02x", message[i] & 0xff);
        //printf("i.%d (%02x) [%c%c]\n",i,message[i],hexbytes[i*2],hexbytes[i*2+1]);
    }
    hexbytes[len*2] = 0;
    //printf("len.%ld\n",len*2+1);
    return((int32_t)len*2+1);
}

// vcalc_sha256 with hardware acceleration
void vcalc_sha256_hardware(char hashstr[(256 >> 3) * 2 + 1],uint8_t hash[256 >> 3],uint8_t *src,int32_t len) {
    SHA256_hash(src, len, hash);
     if ( hashstr != 0 )
        init_hexbytes_noT(hashstr,hash,256 >> 3);

    /*
    snprintf(hashstr, (256 >> 3) * 2 + 1, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", hash[0],
             hash[1],hash[2],hash[3],hash[4],hash[5],hash[6],hash[7],hash[8],hash[9],hash[10],hash[11],hash[12],hash[13],hash[14],hash[15],
             hash[16],hash[17],hash[18],hash[19],hash[20],hash[21],hash[22],hash[23],hash[24],hash[25],hash[26],hash[27],hash[28],hash[29],hash[30],
             hash[31]); */
}

int main()
{
    //uint8_t digest[SHA256_DIGEST_SIZE];
    int i;

    char data[1048576];
    int len = sizeof(data) / sizeof(data[0]) - 1;
    printf("Data len: %d\n", len);

    char hashstr[(256 >> 3) * 2 + 1];
    uint8_t hash[256 >> 3];

    memset(data, 0xDE, sizeof(data));

    struct timeval  tv1, tv2;

    //1 = AVX, 2 = SSE4, 3 = RORX, 4 = RORX8
    printf("Hardware SHA256 [type]: " YELLOW);
    switch (SHA_TYPE) {
        case 1: printf("AVX"); break;
        case 2: printf("SSE4"); break;
        case 3: printf("RORX"); break;
        case 4: printf("ROR8"); break;
    }

    printf("\n" RESET);

    printf("Hardware SHA256:\n");

    gettimeofday(&tv1, NULL);
    for (i=0; i < NUM_ITER; i++) {
        vcalc_sha256_hardware(hashstr, hash, (uint8_t *) data, len);
    }
    gettimeofday(&tv2, NULL);
    printf("%s\n", hashstr);
    printf ("Total time = " YELLOW "%f seconds\n" RESET,
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));

    printf("Software SHA256:\n");

    gettimeofday(&tv1, NULL);
    for (i=0; i < NUM_ITER; i++) {
        vcalc_sha256(hashstr, hash, (uint8_t *) data, len);
    }
    gettimeofday(&tv2, NULL);
    printf("%s\n", hashstr);
    printf ("Total time = " YELLOW "%f seconds\n" RESET,
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));

    //printf("d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592\n");
    //printf("bb7aa507ee307613a18d2b1eabb42e76a5e3ff8a00d021b738f6a5a8f299ed29\n");
    printf("860f58999c792790db1b966865b7897da0a1fb37377bf8469f99472d6f388f3a\n");


    sleep(1);
    return 0;
}
