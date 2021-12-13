// Free for all implementation of the MD4 message-digest algorithm
// by Dominik Reichl
// Based on RSA's MD4C.C and MD4.h files.

// Original header in MD4C.C and MD4.h:

// MD4C.C - RSA Data Security, Inc., MD4 message-digest algorithm

/*
	Copyright (C) 1990-2, RSA Data Security, Inc. All rights reserved.

	License to copy and use this software is granted provided that it
	is identified as the "RSA Data Security, Inc. MD4 Message-Digest
	Algorithm" in all material mentioning or referencing this software
	or this function.

	License is also granted to make and use derivative works provided
	that such works are identified as "derived from the RSA Data
	Security, Inc. MD4 Message-Digest Algorithm" in all material
	mentioning or referencing the derived work.  

	RSA Data Security, Inc. makes no representations concerning either
	the merchantability of this software or the suitability of this
	software for any particular purpose. It is provided "as is"
	without express or implied warranty of any kind.  

	These notices must be retained in any copies of any part of this
	documentation and/or software.  
*/

#ifndef ___MD4_H___
#define ___MD4_H___

#ifndef MD4_POINTER
typedef unsigned char * MD4_POINTER;
#endif

#ifndef UINT4
typedef unsigned long UINT4;
#endif

namespace HASH
{

// MD4 context
typedef struct {
	UINT4 state[4];                                   // state (ABCD)
	UINT4 count[2];        // number of bits, modulo 2^64 (lsb first)
	unsigned char buffer[64];                         // input buffer
} MD4_CTX;

void MD4Init(MD4_CTX *context);
void MD4Update(MD4_CTX *context, unsigned char *input, unsigned int inputLen);
void MD4Final(unsigned char *digest, MD4_CTX *context);

//#include "md4.cpp"

}; // End of namespace HASH

#endif // ___MD4_H___
