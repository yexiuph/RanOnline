//*******************************************************
//
//*******************************************************
#ifndef _4A83ED61477FA930_F472C929D56B1227_44A6_A2AA_B2EC_C401_
#define _4A83ED61477FA930_F472C929D56B1227_44A6_A2AA_B2EC_C401_
#define _CRT_SECURE_DEPRECATE_MEMORY
#include <memory.h>
#include <string>

namespace CHINA_MD5
{

struct MD5Context
{
    unsigned long state[4];
    unsigned long count[2];
    unsigned char buffer[64];
};

class MD5 : public MD5Context
{
public:
    MD5()
    {
        count[0] = count[1] = 0;
        state[0] = 0x67452301;
        state[1] = 0xefcdab89;
        state[2] = 0x98badcfe;
        state[3] = 0x10325476;
    }
    void update( const unsigned char* input, size_t inputLen )
    {
        unsigned int i, index, partLen;

        index = (unsigned int)( ( count[0] >> 3 ) & 0x3F );

        if( ( count[0] += ( (unsigned long)inputLen << 3 ) ) < ( (unsigned long)inputLen << 3 ) )
            ++count[1];
        count[1] += ( (unsigned long)inputLen >> 29 );

        partLen = 64 - index;

        if( inputLen >= partLen )
        {
            memcpy( &buffer[index], input, partLen );
            transform( state, buffer );

            for( i = partLen; i + 63 < inputLen; i += 64 )
                transform( state, &input[i] );

            index = 0;
        }
        else
            i = 0;

        memcpy( &buffer[index], &input[i], inputLen - i );
    }
    unsigned char* get() const
    {
        MD5 m( *this );
        memcpy( digest, m.pget(), sizeof( digest ) );
        return digest;
    }
    std::string asString() const
    {
        unsigned char* hash = get();
        std::string result;
        for( int i = 0; i < 16; ++i )
        {
            result += ( hash[i] / 16 ) < 10 ? '0' + ( hash[i] / 16 ) : 'A' + ( hash[i] / 16 ) - 10;
            result += ( hash[i] % 16 ) < 10 ? '0' + ( hash[i] % 16 ) : 'A' + ( hash[i] % 16 ) - 10;
        }
        return result;
    }
private:
    mutable unsigned char digest[16];
    unsigned char* pget()
    {
        unsigned char bits[8];
        unsigned int index, padLen;

        encode( bits, count, 8 );

        index = (unsigned int)( ( count[0] >> 3 ) & 0x3f );
        padLen = ( index < 56 ) ? ( 56 - index ) : ( 120 - index );

        static unsigned char PADDING[64] = { 0x80 };
        update( PADDING, padLen );

        update( bits, 8 );

        encode( digest, state, 16 );

        memset( (MD5Context*)this, 0, sizeof( MD5Context ) );
        return digest;
    }
    static unsigned long F( unsigned long x, unsigned long y, unsigned long z )
    {
        return ( x & y ) | ( ~x & z );
    }
static unsigned long G( unsigned long x, unsigned long y, unsigned long z )
    {
        return ( x & z) | ( y & ~z );
    }
    static unsigned long H( unsigned long x, unsigned long y, unsigned long z)
    {
        return x ^ y ^ z;
    }
    static unsigned long I( unsigned long x, unsigned long y, unsigned long z)
    {
        return y ^ ( x | ~z );
    }
    static unsigned long ROTATE_LEFT( unsigned long x, unsigned long n )
    {
        return ( x << n ) | ( x >> ( 32 - n ) );
    }
    static void FF( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac )
    {
        a += F( b , c , d ) + x + (unsigned long)ac;
        a = ROTATE_LEFT( a, s );
        a += b;
    }
    static void GG( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac )
    {
        a += G( b , c , d ) + x + (unsigned long)ac;
        a = ROTATE_LEFT( a, s );
        a += b;
    }
    static void HH( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac )
    {
        a += H( b , c , d ) + x + (unsigned long)ac;
        a = ROTATE_LEFT( a, s );
        a += b;
    }
    static void II( unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac )
    {
        a += I( b , c , d ) + x + (unsigned long)ac;
        a = ROTATE_LEFT( a, s );
        a += b;
    }
    static void transform( unsigned long state[4], const unsigned char block[64] )
    {
        unsigned long a = state[0], b = state[1], c = state[2], d = state[3], x[16];

        decode( x, block, 64 );

        /* Round 1 */
        FF( a, b, c, d, x[0], 7, 0xd76aa478 ); /* 1 */
        FF( d, a, b, c, x[1], 12, 0xe8c7b756 ); /* 2 */
        FF( c, d, a, b, x[2], 17, 0x242070db ); /* 3 */
        FF( b, c, d, a, x[3], 22, 0xc1bdceee ); /* 4 */
        FF( a, b, c, d, x[4], 7, 0xf57c0faf ); /* 5 */
        FF( d, a, b, c, x[5], 12, 0x4787c62a ); /* 6 */
        FF( c, d, a, b, x[6], 17, 0xa8304613 ); /* 7 */
        FF( b, c, d, a, x[7], 22, 0xfd469501 ); /* 8 */
        FF( a, b, c, d, x[8], 7, 0x698098d8 ); /* 9 */
        FF( d, a, b, c, x[9], 12, 0x8b44f7af ); /* 10 */
        FF( c, d, a, b, x[10], 17, 0xffff5bb1 ); /* 11 */
        FF( b, c, d, a, x[11], 22, 0x895cd7be ); /* 12 */
        FF( a, b, c, d, x[12], 7, 0x6b901122 ); /* 13 */
        FF( d, a, b, c, x[13], 12, 0xfd987193 ); /* 14 */
        FF( c, d, a, b, x[14], 17, 0xa679438e ); /* 15 */
        FF( b, c, d, a, x[15], 22, 0x49b40821 ); /* 16 */

        /* Round 2 */
        GG( a, b, c, d, x[1], 5, 0xf61e2562 ); /* 17 */
        GG( d, a, b, c, x[6], 9, 0xc040b340 ); /* 18 */
        GG( c, d, a, b, x[11], 14, 0x265e5a51 ); /* 19 */
        GG( b, c, d, a, x[0], 20, 0xe9b6c7aa ); /* 20 */
        GG( a, b, c, d, x[5], 5, 0xd62f105d ); /* 21 */
        GG( d, a, b, c, x[10], 9,  0x2441453 ); /* 22 */
        GG( c, d, a, b, x[15], 14, 0xd8a1e681 ); /* 23 */
        GG( b, c, d, a, x[4], 20, 0xe7d3fbc8 ); /* 24 */
        GG( a, b, c, d, x[9], 5, 0x21e1cde6 ); /* 25 */
        GG( d, a, b, c, x[14], 9, 0xc33707d6 ); /* 26 */
        GG( c, d, a, b, x[3], 14, 0xf4d50d87 ); /* 27 */

        GG( b, c, d, a, x[8], 20, 0x455a14ed ); /* 28 */
        GG( a, b, c, d, x[13], 5, 0xa9e3e905 ); /* 29 */
        GG( d, a, b, c, x[2], 9, 0xfcefa3f8 ); /* 30 */
        GG( c, d, a, b, x[7], 14, 0x676f02d9 ); /* 31 */
        GG( b, c, d, a, x[12], 20, 0x8d2a4c8a ); /* 32 */

        /* Round 3 */
        HH( a, b, c, d, x[5], 4, 0xfffa3942 ); /* 33 */
        HH( d, a, b, c, x[8], 11, 0x8771f681 ); /* 34 */
        HH( c, d, a, b, x[11], 16, 0x6d9d6122 ); /* 35 */
        HH( b, c, d, a, x[14], 23, 0xfde5380c ); /* 36 */
        HH( a, b, c, d, x[1], 4, 0xa4beea44 ); /* 37 */
        HH( d, a, b, c, x[4], 11, 0x4bdecfa9 ); /* 38 */
        HH( c, d, a, b, x[7], 16, 0xf6bb4b60 ); /* 39 */
        HH( b, c, d, a, x[10], 23, 0xbebfbc70 ); /* 40 */
        HH( a, b, c, d, x[13], 4, 0x289b7ec6 ); /* 41 */
        HH( d, a, b, c, x[0], 11, 0xeaa127fa ); /* 42 */
        HH( c, d, a, b, x[3], 16, 0xd4ef3085 ); /* 43 */
        HH( b, c, d, a, x[6], 23,  0x4881d05 ); /* 44 */
        HH( a, b, c, d, x[9], 4, 0xd9d4d039 ); /* 45 */
        HH( d, a, b, c, x[12], 11, 0xe6db99e5 ); /* 46 */
        HH( c, d, a, b, x[15], 16, 0x1fa27cf8 ); /* 47 */
        HH( b, c, d, a, x[2], 23, 0xc4ac5665 ); /* 48 */

        /* Round 4 */
        II( a, b, c, d, x[0], 6, 0xf4292244 ); /* 49 */
        II( d, a, b, c, x[7], 10, 0x432aff97 ); /* 50 */
        II( c, d, a, b, x[14], 15, 0xab9423a7 ); /* 51 */
        II( b, c, d, a, x[5], 21, 0xfc93a039 ); /* 52 */
        II( a, b, c, d, x[12], 6, 0x655b59c3 ); /* 53 */
        II( d, a, b, c, x[3], 10, 0x8f0ccc92 ); /* 54 */
        II( c, d, a, b, x[10], 15, 0xffeff47d ); /* 55 */
        II( b, c, d, a, x[1], 21, 0x85845dd1 ); /* 56 */
        II( a, b, c, d, x[8], 6, 0x6fa87e4f ); /* 57 */
        II( d, a, b, c, x[15], 10, 0xfe2ce6e0 ); /* 58 */
        II( c, d, a, b, x[6], 15, 0xa3014314 ); /* 59 */
        II( b, c, d, a, x[13], 21, 0x4e0811a1 ); /* 60 */
        II( a, b, c, d, x[4], 6, 0xf7537e82 ); /* 61 */
        II( d, a, b, c, x[11], 10, 0xbd3af235 ); /* 62 */
        II( c, d, a, b, x[2], 15, 0x2ad7d2bb ); /* 63 */
        II( b, c, d, a, x[9], 21, 0xeb86d391 ); /* 64 */

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;

        memset( x, 0, sizeof( x ) );
    }
    static void encode( unsigned char* output, const unsigned long *input, size_t len )
    {
        for( unsigned int i = 0, j = 0; j < len; ++i, j += 4)
        {
            output[j] = (unsigned char)( input[i] & 0xff);
            output[j+1] = (unsigned char)( (input[i] >> 8 ) & 0xff );
            output[j+2] = (unsigned char)( (input[i] >> 16 ) & 0xff );
            output[j+3] = (unsigned char)( (input[i] >> 24 ) & 0xff );
        }
    }
    static void decode( unsigned long* output, const unsigned char* input, size_t len )
    {
        for( unsigned int i = 0, j = 0; j < len; ++i, j += 4 )
            output[i] = input[j] | ( input[j+1] << 8) |
                ( input[j+2] << 16 ) | ( input[j+3] << 24 );
    }
};

}; // End of namespace CHINA_MD5
#endif
