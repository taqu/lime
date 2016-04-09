#ifndef INC_LCORE_CRYPTOGRAPHY_H__
#define INC_LCORE_CRYPTOGRAPHY_H__
/**
@file Cryptography.h
@author t-sakai
@date 2014/01/06 create
*/
#include "lcore.h"

namespace lcore
{
    //----------------------------------------------------
    //---
    //--- PKCS5
    //---
    //----------------------------------------------------
    class PKCS5
    {
    public:
        /**
        @return パディング後のサイズ
        @param length
        @param blockSize ... ブロックサイズ
        */
        static s32 getPaddedLength(s32 length, u32 blockSize);

        /**
        @brief パディング
        @param data ... getPaddedLength(length)以上のサイズ
        @param length ... パディングなしのデータサイズ
        @param blockSize ... ブロックサイズ
        */
        static void padding(u8* data, s32 length, u32 blockSize);

        /**
        @brief パディング前のサイズを返す
        @param data ... パディング後のデータ
        @param length ... パディング後のサイズ
        */
        static s32 getLengthFromPadded(u8* data, s32 length);

        static bool checkLength(s32 length, u32 blockSize);
    };

    //----------------------------------------------------
    //---
    //--- ARCFOUR
    //---
    //----------------------------------------------------
    class ARCFOURContext;

    class ARCFOUR
    {
    public:
        static const u32 StateSize = 256;
        static const u32 StateMask = 0xFFU;

        static s32 encrypt(const ARCFOURContext& context, u8* data, s32 dataLength);
        static s32 decrypt(const ARCFOURContext& context, u8* cipher, s32 cipherLength);
    };


    class ARCFOURContext
    {
    public:
        ARCFOURContext();
        ARCFOURContext(u8* key, u32 length);
        ~ARCFOURContext();

        void initialize(u8* key, u32 length);

        u8 s_[ARCFOUR::StateSize];
    };


    //----------------------------------------------------
    //---
    //--- AES
    //---
    //----------------------------------------------------
    class AESContext;

    class AES
    {
    public:
        static const u32 Key128Bytes = 16;
        static const u32 Key192Bytes = 24;
        static const u32 Key256Bytes = 32;
        static const u32 BlockSizeInBytes = 16;

        enum Mode
        {
            Mode_None = -1,
            Mode_ECB = 0,
            Mode_CBC = 1,
        };

        /**
        @param context
        @param cipher ... dataLength以上のサイズ
        @param data ... 
        @param dataLength ... dataのバイトサイズ。16の倍数
        */
        static s32 encrypt(const AESContext& context, u8* cipher, const u8* data, s32 dataLength);

        /**
        @param context
        @param data ... cipherLength以上のサイズ
        @param cipher ... 
        @param cipherLength ... cipherのバイトサイズ。16の倍数
        */
        static s32 decrypt(const AESContext& context, u8* data, const u8* cipher, s32 cipherLength);
    };

    class AESContext
    {
    public:
        static const u32 BufferSize = 60;

        AESContext();
        AESContext(const u8* IV, const u8* key, u32 keyLength, AES::Mode mode);

        ~AESContext();

        bool isInitialized() const;

        bool initialize(const u8* IV, const u8* key, u32 keyLength, AES::Mode mode);

        u32 Ek_[BufferSize];
        u32 Dk_[BufferSize];
        u32 IV_[4];
        u8 Nr_;
        s8 mode_;
        u8 dummy0_;
        u8 dummy1_;
    };

    //----------------------------------------------------
    //---
    //--- BlowFish
    //---
    //----------------------------------------------------
    class BlowFishContext;

    class BlowFish
    {
    public:
        static const u32 MaxKeyBytes = 56;
        static const s32 NPass = 16;
        static const u32 BlockSizeInBytes = 8;

        struct Bytes
        {
            u8 byte3_;
            u8 byte2_;
            u8 byte1_;
            u8 byte0_;
        };

        union aword
        {
            u32 dword_;
            u8 bytes_[4];
            Bytes w_;
        };

        /**
        @param context
        @param cipher ... dataLength以上のサイズ
        @param data ... 
        @param dataLength ... dataのバイトサイズ。8の倍数
        */
        static void encrypt(const BlowFishContext& context, u8* cipher, const u8* data, s32 dataLength);

        /**
        @param context
        @param data ... cipherLength以上のサイズ
        @param cipher ... 
        @param cipherLength ... cipherのバイトサイズ。8の倍数
        */
        static void decrypt(const BlowFishContext& context, u8* data, const u8* cipher, s32 cipherLength);

    private:
        friend class BlowFishContext;

        static void encrypt(const BlowFishContext& context, u32* xl, u32* xr);
        static void decrypt(const BlowFishContext& context, u32* xl, u32* xr);
    };

    class BlowFishContext
    {
    public:
        void initialize(u8* key, s32 length);

    private:
        friend class BlowFish;

        u32 parray_[18];
        u32 sboxes_[4][256];
    };


#if 0
    /*
 *  RFC 1321 compliant MD5 implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 *  The MD5 algorithm was designed by Ron Rivest in 1991.
 *
 *  http://www.ietf.org/rfc/rfc1321.txt
 */
#endif

#if 0
    //Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating
var int[64] s, K

//s specifies the per-round shift amounts
s[ 0..15] := { 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22 }
s[16..31] := { 5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20 }
s[32..47] := { 4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23 }
s[48..63] := { 6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 }

//Use binary integer part of the sines of integers (Radians) as constants:
for i from 0 to 63
    K[i] := floor(232 × abs(sin(i + 1)))
end for
//(Or just use the following precomputed table):
K[ 0.. 3] := { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee }
K[ 4.. 7] := { 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 }
K[ 8..11] := { 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be }
K[12..15] := { 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 }
K[16..19] := { 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa }
K[20..23] := { 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 }
K[24..27] := { 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed }
K[28..31] := { 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a }
K[32..35] := { 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c }
K[36..39] := { 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 }
K[40..43] := { 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 }
K[44..47] := { 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 }
K[48..51] := { 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 }
K[52..55] := { 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 }
K[56..59] := { 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 }
K[60..63] := { 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 }

//Initialize variables:
var int a0 := 0x67452301   //A
var int b0 := 0xefcdab89   //B
var int c0 := 0x98badcfe   //C
var int d0 := 0x10325476   //D

//Pre-processing: adding a single 1 bit
append "1" bit to message    
/* Notice: the input bytes are considered as bits strings,
  where the first bit is the most significant bit of the byte.[48]*/
  

//Pre-processing: padding with zeros
append "0" bit until message length in bits ≡ 448 (mod 512)
append original length in bits mod (2 pow 64) to message


//Process the message in successive 512-bit chunks:
for each 512-bit chunk of message
    break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15
//Initialize hash value for this chunk:
    var int A := a0
    var int B := b0
    var int C := c0
    var int D := d0
//Main loop:
    for i from 0 to 63
        if 0 ≤ i ≤ 15 then
            F := (B and C) or ((not B) and D)
            g := i
        else if 16 ≤ i ≤ 31
            F := (D and B) or ((not D) and C)
            g := (5×i + 1) mod 16
        else if 32 ≤ i ≤ 47
            F := B xor C xor D
            g := (3×i + 5) mod 16
        else if 48 ≤ i ≤ 63
            F := C xor (B or (not D))
            g := (7×i) mod 16
        dTemp := D
        D := C
        C := B
        B := B + leftrotate((A + F + K[i] + M[g]), s[i])
        A := dTemp
    end for
//Add this chunk's hash to result so far:
    a0 := a0 + A
    b0 := b0 + B
    c0 := c0 + C
    d0 := d0 + D
end for

var char digest[16] := a0 append b0 append c0 append d0 //(Output is in little-endian)

//leftrotate function definition
leftrotate (x, c)
    return (x << c) binary or (x >> (32-c));

( 0 ≤ i ≤ 15): F := D xor (B and (C xor D))
(16 ≤ i ≤ 31): F := C xor (D and (B xor C))
    #endif

#if 0
#include <windows.h>
#include <stdio.h>

#define INFO_DISP	1	//	詳細情報が必要でない場合、この行をコメントアウトする。

/*
// _lrotl 関数がない環境で定義する。
// xを左へnビットローテートする
inline UINT32 _lrotl(UINT32 x,UINT32 n){
	return ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)));
}
*/

//以下は、windows.hをインクルードしない場合に定義してください。
/*
typedef unsigned long long UINT64;
typedef signed int INT32;
typedef unsigned int UINT32;
typedef unsigned char BYTE;
*/

union HASH_UINT128{
	UINT64	u64[2];
	UINT32	u32[4];
	BYTE	u8[16];
protected:
	UINT32 F(UINT32 x, UINT32 y, UINT32 z){
		return (x & y) | (~x & z);
	}
	UINT32 FF(UINT32 a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac){
		a += F(b, c, d) + x + ac;
		a = _lrotl(a, s);
		a += b;
		return a;
	}
	UINT32 G(UINT32 x, UINT32 y, UINT32 z){
		return (x & z) | (y & (~z));
	}
	UINT32 GG(UINT32 a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac){
		a += G(b, c, d) + x + ac;
		a = _lrotl(a, s);
		a += b;
		return a;
	}
	UINT32 H(UINT32 x, UINT32 y, UINT32 z){
		return x ^ y ^ z;
	}
	UINT32 HH(UINT32 a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac){
		a += H(b, c, d) + x + ac;
		a = _lrotl(a, s);
		a += b;
		return a;
	}
	UINT32 I(UINT32 x, UINT32 y, UINT32 z){
		return y ^ (x | (~z));
	}
	UINT32 II(UINT32 a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac){
		a += I(b, c, d) + x + ac;
		a = _lrotl(a, s);
		a += b;
		return a;
	}
	void rotate(void){
		UINT32 t = u32[3];
		u32[3] = u32[2];
		u32[2] = u32[1];
		u32[1] = u32[0];
		u32[0] = t;
	}
public:
	void init(void){
		u32[0] = 0x67452301;
		u32[1] = 0xefcdab89;
		u32[2] = 0x98badcfe;
		u32[3] = 0x10325476;
	}
	void hex(char* s){
		sprintf_s(s, 33, "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", u8[0], u8[1], u8[2], u8[3], u8[4], u8[5], u8[6], u8[7], u8[8], u8[9], u8[10], u8[11], u8[12], u8[13], u8[14], u8[15]);
	}
	void md5hash(char* msg, INT32 len);
};

void HASH_UINT128::md5hash(char* msg, INT32 len){
	//	テーブル T[n-1]=429467296*fabs(sin(n)); // 2^32=429467296
	static UINT32 T[] = {
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, // 01～04
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, // 05～08
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, // 09～12
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, // 13～16
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, // 17～20
		0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, // 21～24
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, // 25～28
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, // 29～32
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, // 33～36
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, // 37～40
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, // 41～44
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, // 45～48
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, // 49～52
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, // 53～56
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, // 57～60
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391  // 61～64
	};

	UINT32 x_buf[64 / 4];
	BYTE* block_buf=(BYTE *)x_buf;
	UINT32* x;
	BYTE* block;

	int n;
	UINT32 temp[4];

	UINT32 & a = u32[0];
	UINT32 & b = u32[1];
	UINT32 & c = u32[2];
	UINT32 & d = u32[3];

	UINT32 FF_S[] = { 7, 12, 17, 22 };	//	ローテート数
	UINT32 GG_S[] = { 5,  9, 14, 20 };	//	ローテート数
	UINT32 HH_S[] = { 4, 11, 16, 23 };	//	ローテート数
	UINT32 II_S[] = { 6, 10, 15, 21 };	//	ローテート数
	
	INT64 org_len = len;
	int msg_sz;
	bool end = false;
	int old_sz=0;	//	前回のメッセージ長
	do{
		temp[0] = u32[0];
		temp[1] = u32[1];
		temp[2] = u32[2];
		temp[3] = u32[3];
		//	メッセージサイズの調整
	
		if (512/8 <= len){	//	512bit 対象
			msg_sz = 512 / 8;
			x = (UINT32*)msg;
			block = (BYTE *)msg;
			old_sz=512;
		}
		else{
			x = x_buf;
			block = block_buf;
			UINT64* sz = (UINT64*)&x[14];
			msg_sz = (int)len;
			if (msg_sz){
				for (n = 0; n < msg_sz; n++){
					block[n] = msg[n];
				}
				if (n < 512 / 8 - 8){ // 56byteより小さいので終了ブロックである。
					block[n++] = 0x80;
					for (; n < (512 / 8) - 8; n++){
						block[n] = 0;
					}
					*sz = org_len * 8;
					end=true;
				}else{	//	56byteから63byteである 次のブロックで終了する
					n = msg_sz;
					block[n++] = 0x80;
					for (; n < 64; n++)
						block[n] = 0x00;
				}
			}else{	//	長さが0の場合　全部0で埋める　終了ブロックである。
				if(old_sz==512){
					x[0]=0x80;
					n=1;
				}else
					n=0;
				for (; n < 14; n++){
					x[n] = 0;
				}
				*sz=org_len*8;
				end = true;
			}
			old_sz=msg_sz;
		}
#ifdef INFO_DISP
		printf("----------------------------------------\n");
		for (n = 0; n < 16; n++){
			printf("x[%2d]=0x%0.8x; // 0x%0.2x 0x%0.2x 0x%0.2x 0x%0.2x%\n", n, x[n], block[n * 4], block[n * 4 + 1], block[n * 4 + 2], block[n * 4 + 3]);
		}
		printf("n ,func, A        ,B         ,C         ,D         ,x[n]            ,S ,T[n]      ,ans\n");
#endif
		UINT32 xp;
		for (n = 0; n < 16; n++){
#ifdef INFO_DISP
			printf("%2d,", n);
			printf("FF  ,0x%0.8x,0x%0.8x,0x%0.8x,0x%0.8x,", a, b, c, d);
			printf("x[%2d]=0x%0.8x,", n, x[n]);
			printf("%2d,0x%0.8x,", FF_S[n % 4], T[n]);
#endif
			a = FF(a, b, c, d, x[n], FF_S[n % 4], T[n]);
#ifdef INFO_DISP
			printf("0x%0.8x\n", a);
#endif
			rotate();
		}
		xp = 1;
		for (; n < 32; n++){
#ifdef INFO_DISP
			printf("%2d,", n);
			printf("GG  ,0x%0.8x,0x%0.8x,0x%0.8x,0x%0.8x,", a, b, c, d);
			printf("x[%2d]=0x%0.8x,", xp, x[xp]);
			printf("%2d,0x%0.8x,", GG_S[n % 4], T[n]);
#endif
			a = GG(a, b, c, d, x[xp], GG_S[n % 4], T[n]);
#ifdef INFO_DISP
			printf("0x%0.8x\n", a);
#endif
			xp = (xp + 5) % 16;
			rotate();
		}
		xp = 5;
		for (; n < 48; n++){
#ifdef INFO_DISP
			printf("%2d,", n);
			printf("HH  ,0x%0.8x,0x%0.8x,0x%0.8x,0x%0.8x,", a, b, c, d);
			printf("x[%2d]=0x%0.8x,", xp, x[xp]);
			printf("%2d,0x%0.8x,", HH_S[n % 4], T[n]);
#endif
			a = HH(a, b, c, d, x[xp], HH_S[n % 4], T[n]);
#ifdef INFO_DISP
			printf("0x%0.8x\n", a);
#endif
			xp = (xp + 3) % 16;
			rotate();
		}
		xp = 0;
		for (; n < 64; n++){
#ifdef INFO_DISP
			printf("%2d,", n);
			printf("II  ,0x%0.8x,0x%0.8x,0x%0.8x,0x%0.8x,", a, b, c, d);
			printf("x[%2d]=0x%0.8x,", xp, x[xp]);
			printf("%2d,0x%0.8x,", II_S[n % 4], T[n]);
#endif
			a = II(a, b, c, d, x[xp], II_S[n % 4], T[n]);
#ifdef INFO_DISP
			printf("0x%0.8x\n", a);
#endif
			xp = (xp + 7) % 16;
			rotate();
		}

		a += temp[0];
		b += temp[1];
		c += temp[2];
		d += temp[3];
		len -= msg_sz;
		msg += msg_sz;
	} while (0 <= len && end==false);
}

void main(void){
	HASH_UINT128 hash;
	char* pass = "abc";

	hash.init();
	hash.md5hash(pass, (int)strlen(pass));
	char hexs[36];
	hash.hex(hexs);
	printf("%s\nmd5 %s\n",pass,hexs);
}
#endif


    //----------------------------------------------------
    //---
    //--- MD5
    //---
    //----------------------------------------------------
    union MD5Hash
    {
        u64	u64_[2];
	    u32	u32_[4];
        u16	u16_[8];
	    u8  u8_[16];
    };

    /**
    The MD5 algorithm
    RFC document: http://www.ietf.org/rfc/rfc1321.txt
    */
    MD5Hash calcMD5(u32 length, const u8* data);
}
#endif //INC_LCORE_CRYPTOGRAPHY_H__
