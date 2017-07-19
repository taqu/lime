#include <time.h>
#include <stdio.h>
#include <math.h>

typedef char Char;
typedef unsigned char UChar;
typedef wchar_t WChar;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define PI static_cast<f32>(3.14159265358979323846)

template<class T>
inline void swap(T& x0, T& x1)
{
    T t = x1;
    x1 = x0;
    x0 = t;
}

f32 gaussian(f32 d, f32 sigma)
{
    return static_cast<f32>( exp(-d*d/(2.0f*sigma)) / sqrt(2.0f*PI*sigma) );
}


f32 radicalInverseVanDerCorput(u32 bits, u32 scramble);
f32 radicalInverseSobol(u32 i, u32 scramble);

u32 bitreverse(u32 x)
{
#if defined(__GNUC__)
    x = __builtin_bswap32(x);
#elif defined(__clang__)
    x = __builtin_bswap32(x);
#else
    x = (x << 16) | (x >> 16);
    x = ((x & 0x00FF00FFU) << 8) | ((x & 0xFF00FF00U) >> 8);
#endif
    x = ((x & 0x0F0F0F0FU) << 4) | ((x & 0xF0F0F0F0U) >> 4);
    x = ((x & 0x33333333U) << 2) | ((x & 0xCCCCCCCCU) >> 2);
    x = ((x & 0x55555555U) << 1) | ((x & 0xAAAAAAAAU) >> 1);
    return x;
}

f32 radicalInverseVanDerCorput(u32 bits, u32 scramble)
{
    bits = bitreverse(bits);
    bits ^= scramble;
    return static_cast<f32>(bits) / static_cast<f32>(0x100000000L);
}

f32 radicalInverseSobol(u32 i, u32 scramble)
{
    for(u32 v=1U<<31; i; i>>=1, v ^= v>>1){
        if(i&1){
            scramble ^= v;
        }
    }
    return static_cast<f32>(scramble) / static_cast<f32>(0x100000000L);
}

inline void sobol02(f32& v0, f32& v1, u32 i, u32 scramble0, u32 scramble1)
{
    v0 = radicalInverseVanDerCorput(i, scramble0);
    v1 = radicalInverseSobol(i, scramble1);
}

//---------------------------------------------
//---
//--- RandWELL
//---
//---------------------------------------------
class RandWELL
{
public:
    RandWELL();
    explicit RandWELL(u32 seed);
    ~RandWELL();

    /**
    @brief ‹[——”¶¬Ší‰Šú‰»
    @param seed
    */
    void srand(u32 seed);

    /**
    @brief 0 - 0xFFFFFFFFU‚Ì—”¶¬
    */
    u32 rand();

    /**
    @brief 0 - 1‚Ì—”¶¬
    */
    f32 frand();

    /**
    @brief 0.0 - 0.999999881‚Ì—”¶¬
    */
    f32 frand2();

    /**
    @brief 0 - 1‚Ì—”¶¬
    */
    f64 drand();

    void swap(RandWELL& rhs);
private:
    static const u32 N = 16;

    u32 state_[N];
    u32 index_;
};

//---------------------------------------------
//---
//--- RandWELL
//---
//---------------------------------------------
RandWELL::RandWELL()
    :index_(0)
{
}

RandWELL::RandWELL(u32 seed)
    :index_(0)
{
    srand(seed);
}

RandWELL::~RandWELL()
{
}

void RandWELL::srand(u32 seed)
{
    state_[0] = seed;
    for(u32 i=1; i<N; ++i){
        state_[i] = (1812433253 * (state_[i-1]^(state_[i-1] >> 30)) + i); 
    }
}

u32 RandWELL::rand()
{
    u32 a, b, c, d;

    a = state_[index_];
    c = state_[(index_+13)&15];
    b = a^c^(a<<16)^(c<<15);
    c = state_[(index_+9)&15];
    c ^= c>>11;
    a = state_[index_] = b^c;
    d = a^((a<<5)&0xDA442D24UL);
    index_ = (index_ + 15) & 15;
    a = state_[index_];
    state_[index_] = a^b^d^(a<<2)^(b<<18)^(c<<28);
    return state_[index_];
}

f32 RandWELL::frand()
{
    u32 t = rand();

    static const u32 m0 = 0x3F800000U;
    static const u32 m1 = 0x007FFFFFU;
    t = m0|(t&m1);
    return (*(f32*)&t)- 0.999999881f;
}

f32 RandWELL::frand2()
{
    u32 t = rand();

    static const u32 m0 = 0x3F800000U;
    static const u32 m1 = 0x007FFFFFU;
    t = m0|(t&m1);
    return (*(f32*)&t) - 1.000000000f;
}

f64 RandWELL::drand()
{
    return rand()*(1.0/4294967295.0); 
}

void RandWELL::swap(RandWELL& rhs)
{
    for(u32 i=0; i<N; ++i){
        ::swap(state_[i], rhs.state_[i]);
    }
    ::swap(index_, rhs.index_);
}

int main(int argc, char** argv)
{
    time_t timer;
    time(&timer);

    RandWELL random;
    random.srand((u32)timer);

    u32 scramble0 = random.rand();
    u32 scramble1 = random.rand();
    const u32 N = 64;
    f32 samples[N][2];
    FILE* file = fopen("samples.csv", "wb");
    fprintf(file, "s0,s1\n");
    for(u32 i=0; i<N; ++i){
        sobol02(samples[i][0], samples[i][1], i, scramble0, scramble1);
        samples[i][0] = 2.0f*samples[i][0] - 1.0f;
        samples[i][1] = 2.0f*samples[i][1] - 1.0f;
        f32 d = sqrtf(samples[i][0]*samples[i][0] + samples[i][1]*samples[i][1]);
        f32 w = gaussian(d, 1.0f);
        fprintf(file, "float4(%lf, %lf, %lf, %lf),\n", samples[i][0], samples[i][1], w, d);
    }
    fclose(file);
    return 0;
}
