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
// ������ �ش� ��ġ�� ��Ʈ���� �о�´�.
// bits : ����
// pos : ���� �о�� position (0 ���� ���۵ȴ�)
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
// ������ �ش� ��ġ�� ��Ʈ���� �����Ѵ�.
// bits : buffer
// pos : ���õ� buffer ������ ������ (���� �������� 0)
// state : ���õ� ��. 0 or 1
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
// bits1 �� bits2 �� XOR �ؼ� bitsx �� ��ȯ�Ѵ�.
// bits1 : xor �� �ʿ��� buffer
// bits2 : xor �� �ʿ��� buffer
// bitsx : return �Ǵ� buffer
// size : buffer �� bit size
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
// ������ ����ŭ�� buffer�� �������� ȸ�� ��Ų��.
// bits : shift �� buffer
// size : buffer �� bit size
// count : shift �� Ƚ��
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
// ������ ����ŭ�� ���۸� ���������� ȸ�� ��Ų��.
// bits : right shift �� buffer
// size : buffer �� bit size
// count : shift Ƚ��
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
				// ���� ������ ��Ʈ�� �����Ѵ�.
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
// Buffer �� ������ XOR �Ѵ�.
// bits1 : XOR �� ����
// sizeByte : ������ Byte Size
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
// Buffer �� ������ ��Ģ�� ���� capsule ȭ �Ѵ�.
// buffer_encapsule �� ����.
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
// Buffer �� ������ ��Ģ�� ���� encapsule ȭ �Ѵ�.
// buffer_capsule �� ����.
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
