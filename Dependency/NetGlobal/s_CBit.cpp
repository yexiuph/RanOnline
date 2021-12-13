///////////////////////////////////////////////////////////////////////////////
// s_CBit.cpp
//
// class CBit
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_cbit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SERVER_UTIL;

CBit::CBit()
{
}

CBit::~CBit()
{
}

//=============================================================================
// 버퍼의 해당 위치의 비트값을 읽어온다.
// bits : 버퍼
// pos : 값을 읽어올 position (0 부터 시작된다)
//=============================================================================
int  CBit::bit_get(const char *bits, int pos)
{
	if (bits == NULL) return 0;

	unsigned char mask;
	int	i=0;

	mask = 0x80; // bin : 1000 0000

	for (i=0; i < (pos % 8); ++i)
		mask = mask >> 1;

	// return (((mask & bits[(int)(pos / 8)]) == mask) ? 1 : 0);
	return (((mask & *(bits+(int)(pos / 8))) == mask) ? 1 : 0);
}

//=============================================================================
// 버퍼의 해당 위치에 비트값을 세팅한다.
// bits : buffer
// pos : 세팅될 buffer 에서의 포지션 (최초 포지션은 0)
// state : 세팅될 값. 0 or 1
//=============================================================================
void CBit::bit_set(char *bits, int pos, int state)
{
	if (bits == NULL) return;

	unsigned char mask;
	int i=0;

	mask = 0x80; // bin : 1000 0000

	for (i = 0; i < (pos % 8); ++i)
		mask = mask >> 1;
	
	/*  Set the bit */
	if (state)
		// bits[pos / 8] = bits[pos / 8] | mask;
		*(bits+(int)(pos / 8)) = *(bits+(int)(pos / 8)) | mask;
	else
		// bits[pos / 8] = bits[pos / 8] & (~mask);
		*(bits+(int)(pos / 8)) = *(bits+(int)(pos / 8)) & (~mask);
	return;	
}

//=============================================================================
// bits1 과 bits2 를 XOR 해서 bitsx 로 반환한다.
// bits1 : xor 에 필요한 buffer
// bits2 : xor 에 필요한 buffer
// bitsx : return 되는 buffer
// size : buffer 의 bit size
//=============================================================================
void CBit::bit_xor(const char *bits1, 
			 const char *bits2, 
			 char *bitsx, 
			 int size)
{
	if (bits1 == NULL || bits2 == NULL || bitsx == NULL)
		return;

	int i=0;
	
	/* Compute the bitwise XOR (exclusive OR) of the two buffers. */
	for (i = 0; i < size; ++i) 
	{
		if (bit_get(bits1, i) != bit_get(bits2, i))
			bit_set(bitsx, i, 1);
		else
			bit_set(bitsx, i, 0);
	}
	return;
}

//=============================================================================
// 지정된 수만큼의 buffer를 왼쪽으로 회전 시킨다.
// bits : shift 될 buffer
// size : buffer 의 bit size
// count : shift 될 횟수
//=============================================================================
void CBit::bit_rot_left(char *bits, int size, int count)
{
	int fbit=0, lbit=0;
	int i=0, j=0;

	// Rotate the buffer to the left the specified number of bits. 
	if (size > 0) 
	{
		for (j = 0; j < count; ++j) 
		{
			for (i = 0; i <= ((size - 1) / 8); ++i) 
			{
				// Get the bit about to be shifted off the current byte. 
				// lbit = bit_get(&bits[i], 0);
				lbit = bit_get((bits+i), 0);
				if (i == 0) 
				{
					// Save the bit shifted off the first byte for later. 
					fbit = lbit;
				}
				else 
				{
					// Set the rightmost bit of the previous byte to 
					// the leftmost bit about to be shifted off 
					// the current byte.
					// bit_set(&bits[i - 1], 7, lbit);
					bit_set((bits+i-1), 7, lbit);
				}
				// Shift the current byte to the left.
				// bits[i] = bits[i] << 1;
				*(bits+i) = *(bits+i) << 1;
			}
			// Set the rightmost bit of the buffer
			bit_set(bits, size - 1, fbit);
		}
	}
	return;
}

//=============================================================================
// 지정된 수만큼의 버퍼를 오른쪽으로 회전 시킨다.
// bits : right shift 될 buffer
// size : buffer 의 bit size
// count : shift 횟수
//=============================================================================
void CBit::bit_rot_right(char *bits, int size, int count)
{
	int ebit=0, rbit=0;
	int maxbyte=0;
	int cbyte=0;
	int i=0, j=0;

	// Rotate the buffer to the right the specified number of bits. 
	if (size > 0) 
	{
		maxbyte = size / 8;		
		for (j = 0; j < count; ++j) 
		{
			cbyte = maxbyte-1;
			for (i = 0; i <= ((size - 1) / 8); ++i) 
			{				
				// 가장 오른쪽 비트를 저장한다.
				// rbit = bit_get(&bits[cbyte], 7);
				rbit = bit_get((bits+cbyte), 7);
				if (i == 0)
				{
					ebit = rbit;
				}
				else
				{
					// bit_set(&bits[cbyte+1], 0, rbit);
					bit_set((bits+cbyte+1), 0, rbit);
				}				
				// Shift right current byte
				// bits[cbyte] = bits[cbyte] >> 1;
				*(bits+cbyte) = *(bits+cbyte) >> 1;
				--cbyte;
			}
			// Set the leftmost bit of the buffer 
			bit_set(bits, 0, ebit);			
		}			
	}
	return;
}

//=============================================================================
// Buffer 의 내용을 XOR 한다.
// bits1 : XOR 할 버퍼
// sizeByte : 버퍼의 Byte Size
//=============================================================================
void CBit::byte_xor(char *bits1, int sizeByte)
{
	int i=0;
	unsigned char mask;

	mask = 0xFF; // bin : 1111 1111
	
	// Compute the bytewise XOR (exclusive OR)
	for (i = 0; i < sizeByte; ++i) 
	{
		// bits1[i] = bits1[i] ^ mask;
		*(bits1+i) = *(bits1+i) ^ mask;
	}
	return;
}

void CBit::_buf_encode (char *cbuffer,
	int nSize,
	int nDirection,
	int nAmount)
{
	if ( !cbuffer )		return;
	if ( nAmount == 0 )	return;

	if (nDirection == ROT_RIGHT)
		bit_rot_right(cbuffer, nSize*8, nAmount);
	else if (nDirection == ROT_LEFT)
		bit_rot_left(cbuffer, nSize*8, nAmount);
	else
		return;

	byte_xor(cbuffer, nSize);
}

void CBit::_buf_decode(char *cbuffer,
	int nSize,
	int nDirection,
	int nAmount)
{
	if ( !cbuffer )		return;
	if ( nAmount == 0 )	return;

	if (nDirection == ROT_RIGHT)
		bit_rot_left(cbuffer, nSize*8, nAmount);
	else if (nDirection == ROT_LEFT)
		bit_rot_right(cbuffer, nSize*8, nAmount);
	else
		return;

	byte_xor(cbuffer, nSize);
}

//=============================================================================
// Buffer 의 내용을 규칙에 따라 capsule 화 한다.
// buffer_encapsule 과 연동.
//=============================================================================
void CBit::buf_encode(char *cbuffer, 
						  int nSize, 
						  int nDirection, 
						  int nAmount)
{
	_buf_encode ( cbuffer, nSize, nDirection, nAmount );

	return;
}

//=============================================================================
// Buffer 의 내용을 규칙에 따라 encapsule 화 한다.
// buffer_capsule 과 연동.
//=============================================================================
void CBit::buf_decode(char *cbuffer,
							int nSize,
							int nDirection,
							int nAmount)
{	
	_buf_decode ( cbuffer, nSize, nDirection, nAmount );

	return;
}

//                 XXXX
//                X    XX
//               X  ***  X                XXXXX
//              X  *****  X            XXX     XX
//           XXXX ******* XXX      XXXX          XX
//         XX   X ******  XXXXXXXXX                XX XXX
//       XX      X ****  X                           X** X
//      X        XX    XX     X                      X***X
//     X         //XXXX       X                      XXXX
//    X         //   X                             XX
//   X         //    X          XXXXXXXXXXXXXXXXXX/
//   X     XXX//    X          X
//   X    X   X     X         X
//   X    X    X    X        X
//    X   X    X    X        X                    XX
//    X    X   X    X        X                 XXX  XX
//     X    XXX      X        X               X  X X  X
//     X             X         X              XX X  XXXX
//      X             X         XXXXXXXX\     XX   XX  X
//       XX            XX              X     X    X  XX
//         XX            XXXX   XXXXXX/     X     XXXX
//           XXX             XX***         X     X
//              XXXXXXXXXXXXX *   *       X     X
//                           *---* X     X     X
//                          *-* *   XXX X     X
//                          *- *       XXX   X
//                         *- *X          XXX
//                         *- *X  X          XXX
//                        *- *X    X            XX
//                        *- *XX    X             X
//                       *  *X* X    X             X
//                       *  *X * X    X             X
//                      *  * X**  X   XXXX          X
//                      *  * X**  XX     X          X
//                     *  ** X** X     XX          X
//                     *  **  X*  XXX   X         X
//                    *  **    XX   XXXX       XXX
//                   *  * *      XXXX      X     X
//                  *   * *          X     X     X
//    =======*******   * *           X     X      XXXXXXXX\
//           *         * *      /XXXXX      XXXXXXXX\      )
//      =====**********  *     X                     )  \  )
//        ====*         *     X               \  \   )XXXXX
//   =========**********       XXXXXXXXXXXXXXXXXXXXXX
// oops!
// jgkim
