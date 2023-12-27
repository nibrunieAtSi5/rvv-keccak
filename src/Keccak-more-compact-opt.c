// source: https://github.com/XKCP/XKCP/blob/master/Standalone/CompactFIPS202/C/Keccak-more-compact.c
#define FOR(i,n) for(i=0; i<n; ++i)
typedef unsigned char u8;
typedef unsigned long long int u64;
typedef unsigned int ui;

void Keccak(ui r, ui c, const u8 *in, u64 inLen, u8 sfx, u8 *out, u64 outLen);
void FIPS202_SHAKE128(const u8 *in, u64 inLen, u8 *out, u64 outLen) { Keccak(1344, 256, in, inLen, 0x1F, out, outLen); }
void FIPS202_SHAKE256(const u8 *in, u64 inLen, u8 *out, u64 outLen) { Keccak(1088, 512, in, inLen, 0x1F, out, outLen); }
void FIPS202_SHA3_224(const u8 *in, u64 inLen, u8 *out) { Keccak(1152, 448, in, inLen, 0x06, out, 28); }
void FIPS202_SHA3_256(const u8 *in, u64 inLen, u8 *out) { Keccak(1088, 512, in, inLen, 0x06, out, 32); }
void FIPS202_SHA3_384(const u8 *in, u64 inLen, u8 *out) { Keccak(832, 768, in, inLen, 0x06, out, 48); }
void FIPS202_SHA3_512(const u8 *in, u64 inLen, u8 *out) { Keccak(576, 1024, in, inLen, 0x06, out, 64); }

int LFSR86540(u8 *R) { (*R)=((*R)<<1)^(((*R)&0x80)?0x71:0); return ((*R)&2)>>1; }
#define ROL(a,o) ((((u64)a)<<o)^(((u64)a)>>(64-o)))
#define rL(x,y) ((u64*)s)[x+5*y]
#define wL(x,y,l) do { ((u64*)s)[x+5*y] = l; } while (0)
#define XL(x,y,l) do { ((u64*)s)[x+5*y] ^= l; } while (0)

extern unsigned long totalEvts, nCalls, minLatency, maxLatency;

/** return the value of the instret counter
 *
 *  The instret counter counts the number of retired (executed) instructions.
*/
static unsigned long read_instret(void)
{
  unsigned long instret;
  asm volatile ("rdinstret %0" : "=r" (instret));
  return instret;
}

// round constants for ι step
const u64 RC[25] = {
    0x0000000000000001, // RC[0]	
    0x0000000000008082, // RC[1]	
    0x800000000000808A, // RC[2]	
    0x8000000080008000, // RC[3]	
    0x000000000000808B, // RC[4]	
    0x0000000080000001, // RC[5]	
    0x8000000080008081, // RC[6]	
    0x8000000000008009, // RC[7]	
    0x000000000000008A, // RC[8]	
    0x0000000000000088, // RC[9]	
    0x0000000080008009, // RC[10]
    0x000000008000000A, // RC[11]
    0x000000008000808B, // RC[12]
    0x800000000000008B, // RC[13]
    0x8000000000008089, // RC[14]
    0x8000000000008003, // RC[15]
    0x8000000000008002, // RC[16]
    0x8000000000000080, // RC[17]
    0x000000000000800A, // RC[18]
    0x800000008000000A, // RC[19]
    0x8000000080008081, // RC[20]
    0x8000000000008080, // RC[21] 
    0x0000000080000001, // RC[22]
    0x8000000080008008, // RC[23]
};

void KeccakF1600(void *s)
{
    unsigned long start, stop;
    start = read_instret();
    ui r,x,y,i,j,Y; u8 R=0x01; u64 C[5],D;
    for(i=0; i<24; i++) {
#       if 0
        /*θ*/ FOR(x,5) C[x]=rL(x,0)^rL(x,1)^rL(x,2)^rL(x,3)^rL(x,4); FOR(x,5) { D=C[(x+4)%5]^ROL(C[(x+1)%5],1); FOR(y,5) XL(x,y,D); }
        /*ρπ*/ x=1; y=r=0; D=rL(x,y); FOR(j,24) { r+=j+1; Y=(2*x+3*y)%5; x=y; y=Y; C[0]=rL(x,y); wL(x,y,ROL(D,r%64)); D=C[0]; }
        /*χ*/ FOR(y,5) { FOR(x,5) C[x]=rL(x,y); FOR(x,5) wL(x,y,C[x]^((~C[(x+1)%5])&C[(x+2)%5])); }
#       else
        u64 C_0= rL(0,0) ^ rL(0,1) ^ rL(0,2) ^ rL(0,3) ^ rL(0,4);
        u64 C_1= rL(1,0) ^ rL(1,1) ^ rL(1,2) ^ rL(1,3) ^ rL(1,4);
        u64 C_2= rL(2,0) ^ rL(2,1) ^ rL(2,2) ^ rL(2,3) ^ rL(2,4);
        u64 C_3= rL(3,0) ^ rL(3,1) ^ rL(3,2) ^ rL(3,3) ^ rL(3,4);
        u64 C_4= rL(4,0) ^ rL(4,1) ^ rL(4,2) ^ rL(4,3) ^ rL(4,4);
        u64 D_0 = C_4 ^ ROL(C_1,1);
        XL(0,0,D_0);
        XL(0,1,D_0);
        XL(0,2,D_0);
        XL(0,3,D_0);
        XL(0,4,D_0);
        u64 D_1 = C_0 ^ ROL(C_2,1);
        XL(1,0,D_1);
        XL(1,1,D_1);
        XL(1,2,D_1);
        XL(1,3,D_1);
        XL(1,4,D_1);
        u64 D_2 = C_1 ^ ROL(C_3,1);
        XL(2,0,D_2);
        XL(2,1,D_2);
        XL(2,2,D_2);
        XL(2,3,D_2);
        XL(2,4,D_2);
        u64 D_3 = C_2 ^ ROL(C_4,1);
        XL(3,0,D_3);
        XL(3,1,D_3);
        XL(3,2,D_3);
        XL(3,3,D_3);
        XL(3,4,D_3);
        u64 D_4 = C_3 ^ ROL(C_0,1);
        XL(4,0,D_4);
        XL(4,1,D_4);
        XL(4,2,D_4);
        XL(4,3,D_4);
        XL(4,4,D_4);
        u64 T_0 = rL(1, 0);
        u64 T_1 = rL(0, 2);
        wL(0, 2, ROL(T_0, 1));
        u64 T_2 = rL(2, 1);
        wL(2, 1, ROL(T_1, 3));
        u64 T_3 = rL(1, 2);
        wL(1, 2, ROL(T_2, 6));
        u64 T_4 = rL(2, 3);
        wL(2, 3, ROL(T_3, 10));
        u64 T_5 = rL(3, 3);
        wL(3, 3, ROL(T_4, 15));
        u64 T_6 = rL(3, 0);
        wL(3, 0, ROL(T_5, 21));
        u64 T_7 = rL(0, 1);
        wL(0, 1, ROL(T_6, 28));
        u64 T_8 = rL(1, 3);
        wL(1, 3, ROL(T_7, 36));
        u64 T_9 = rL(3, 1);
        wL(3, 1, ROL(T_8, 45));
        u64 T_10 = rL(1, 4);
        wL(1, 4, ROL(T_9, 55));
        u64 T_11 = rL(4, 4);
        wL(4, 4, ROL(T_10, 2));
        u64 T_12 = rL(4, 0);
        wL(4, 0, ROL(T_11, 14));
        u64 T_13 = rL(0, 3);
        wL(0, 3, ROL(T_12, 27));
        u64 T_14 = rL(3, 4);
        wL(3, 4, ROL(T_13, 41));
        u64 T_15 = rL(4, 3);
        wL(4, 3, ROL(T_14, 56));
        u64 T_16 = rL(3, 2);
        wL(3, 2, ROL(T_15, 8));
        u64 T_17 = rL(2, 2);
        wL(2, 2, ROL(T_16, 25));
        u64 T_18 = rL(2, 0);
        wL(2, 0, ROL(T_17, 43));
        u64 T_19 = rL(0, 4);
        wL(0, 4, ROL(T_18, 62));
        u64 T_20 = rL(4, 2);
        wL(4, 2, ROL(T_19, 18));
        u64 T_21 = rL(2, 4);
        wL(2, 4, ROL(T_20, 39));
        u64 T_22 = rL(4, 1);
        wL(4, 1, ROL(T_21, 61));
        u64 T_23 = rL(1, 1);
        wL(1, 1, ROL(T_22, 20));
        u64 T_24 = rL(1, 0);
        wL(1, 0, ROL(T_23, 44));
        u64 C_0_0 = rL(0, 0);
        u64 C_0_1 = rL(1, 0);
        u64 C_0_2 = rL(2, 0);
        u64 C_0_3 = rL(3, 0);
        u64 C_0_4 = rL(4, 0);
        wL(0, 0, C_0_0 ^ (~C_0_1 & C_0_2));
        wL(1, 0, C_0_1 ^ (~C_0_2 & C_0_3));
        wL(2, 0, C_0_2 ^ (~C_0_3 & C_0_4));
        wL(3, 0, C_0_3 ^ (~C_0_4 & C_0_0));
        wL(4, 0, C_0_4 ^ (~C_0_0 & C_0_1));
        u64 C_1_0 = rL(0, 1);
        u64 C_1_1 = rL(1, 1);
        u64 C_1_2 = rL(2, 1);
        u64 C_1_3 = rL(3, 1);
        u64 C_1_4 = rL(4, 1);
        wL(0, 1, C_1_0 ^ (~C_1_1 & C_1_2));
        wL(1, 1, C_1_1 ^ (~C_1_2 & C_1_3));
        wL(2, 1, C_1_2 ^ (~C_1_3 & C_1_4));
        wL(3, 1, C_1_3 ^ (~C_1_4 & C_1_0));
        wL(4, 1, C_1_4 ^ (~C_1_0 & C_1_1));
        u64 C_2_0 = rL(0, 2);
        u64 C_2_1 = rL(1, 2);
        u64 C_2_2 = rL(2, 2);
        u64 C_2_3 = rL(3, 2);
        u64 C_2_4 = rL(4, 2);
        wL(0, 2, C_2_0 ^ (~C_2_1 & C_2_2));
        wL(1, 2, C_2_1 ^ (~C_2_2 & C_2_3));
        wL(2, 2, C_2_2 ^ (~C_2_3 & C_2_4));
        wL(3, 2, C_2_3 ^ (~C_2_4 & C_2_0));
        wL(4, 2, C_2_4 ^ (~C_2_0 & C_2_1));
        u64 C_3_0 = rL(0, 3);
        u64 C_3_1 = rL(1, 3);
        u64 C_3_2 = rL(2, 3);
        u64 C_3_3 = rL(3, 3);
        u64 C_3_4 = rL(4, 3);
        wL(0, 3, C_3_0 ^ (~C_3_1 & C_3_2));
        wL(1, 3, C_3_1 ^ (~C_3_2 & C_3_3));
        wL(2, 3, C_3_2 ^ (~C_3_3 & C_3_4));
        wL(3, 3, C_3_3 ^ (~C_3_4 & C_3_0));
        wL(4, 3, C_3_4 ^ (~C_3_0 & C_3_1));
        u64 C_4_0 = rL(0, 4);
        u64 C_4_1 = rL(1, 4);
        u64 C_4_2 = rL(2, 4);
        u64 C_4_3 = rL(3, 4);
        u64 C_4_4 = rL(4, 4);
        wL(0, 4, C_4_0 ^ (~C_4_1 & C_4_2));
        wL(1, 4, C_4_1 ^ (~C_4_2 & C_4_3));
        wL(2, 4, C_4_2 ^ (~C_4_3 & C_4_4));
        wL(3, 4, C_4_3 ^ (~C_4_4 & C_4_0));
        wL(4, 4, C_4_4 ^ (~C_4_0 & C_4_1));
#       endif
        /*ι*/ XL(0,0,RC[i]);
    }
    stop = read_instret();
    long cycleCnt = (stop - start);
    nCalls += 24;
    totalEvts += cycleCnt;
    if (cycleCnt < minLatency) minLatency = cycleCnt;
    if (cycleCnt > maxLatency) maxLatency = cycleCnt;
}
void Keccak(ui r, ui c, const u8 *in, u64 inLen, u8 sfx, u8 *out, u64 outLen)
{
    /*initialize*/ u8 s[200]; ui R=r/8; ui i,b=0; FOR(i,200) s[i]=0;
    /*absorb*/ while(inLen>0) { b=(inLen<R)?inLen:R; FOR(i,b) s[i]^=in[i]; in+=b; inLen-=b; if (b==R) { KeccakF1600(s); b=0; } }
    /*pad*/ s[b]^=sfx; if((sfx&0x80)&&(b==(R-1))) KeccakF1600(s); s[R-1]^=0x80; KeccakF1600(s);
    /*squeeze*/ while(outLen>0) { b=(outLen<R)?outLen:R; FOR(i,b) out[i]=s[i]; out+=b; outLen-=b; if(outLen>0) KeccakF1600(s); }
}