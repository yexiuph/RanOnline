#include "pch.h"
#include "des.h"
#include <string.h>
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace ApexChina
{

bool DES_ConvertToFixed(const char *szFrom, int nFrom, char *szTo, int nTo, bool bAppend)
{
	int	len = nFrom;
	if( len<nTo )
	{
		len	= nTo;
	}

	int	idx;
	int	i=0,j=0;
	memset(szTo,0,nTo);
	for(idx=0;idx<len;idx++)
	{
		szTo[j++]	^= szFrom[i++];
		if( i>=nFrom )	i=0;
		if( j>=nTo )	j=0;
	}

	return	true;
}

bool DES_XOR(char *szDst, int nDst, const char *szKey, int nKey)
{
	int	len = nDst;
	if( len<nKey )
	{
		len	= nKey;
	}

	int	idx;
	int	i=0,j=0;
	for(idx=0;idx<len;idx++)
	{
		szDst[i++]	^= szKey[j++];
		if( i>=nDst )	i=0;
		if( j>=nKey )	j=0;
	}

	return	true;
}
bool	DES_XOR_AC_EN(unsigned char *szDst, int nDst, const unsigned char *szKey, int nKey)
{
	int	len = nDst;
	if( len<nKey )
	{
		assert(0);
		return	false;
	}

	int		idx;
	int		i=0,j=0;
	char	cSeed=0;
	for(idx=0;idx<len;idx++)
	{
		szDst[i]	^= cSeed^szKey[j];
		cSeed		= szDst[i];
		++	i;
		++	j;
		if( i>=nDst )	i=0;
		if( j>=nKey )	j=0;
	}

	return	true;
}
bool	DES_XOR_AC_DE(unsigned char *szDst, int nDst, const unsigned char *szKey, int nKey)
{
	int	len = nDst;
	if( len<nKey )
	{
		assert(0);
		return	false;
	}

	int		idx;
	int		i=0,j=0;
	char	cSeed0=0,cSeed1=0;
	for(idx=0;idx<len;idx++)
	{
		cSeed1		= szDst[i];
		szDst[i]	^= cSeed0^szKey[j];
		cSeed0		= cSeed1;
		++	i;
		++	j;
		if( i>=nDst )	i=0;
		if( j>=nKey )	j=0;
	}

	return	true;
}
void	DES_MEM_XOR(unsigned char *szDst, const unsigned char *cszSrc, int nLen)
{
	for(int i=0;i<nLen;i++)
	{
		szDst[i]	^= cszSrc[i];
	}
}
void	DES_BYTE_PERMUTE(unsigned char *szDst, int nLen)
{
	int		nCount	= nLen/2;
	int		nB		= (szDst[0]+szDst[nLen-1])%2;
	for(int i=0;i<nCount;i++)
	{
		// 如果对应两个的的和和需要的一致，则交换之
		if( (szDst[i]+szDst[nLen-1-i])%2 == nB )
		{
			unsigned char	c;
			c				= szDst[i];
			szDst[i]		= szDst[nLen-1-i];
			szDst[nLen-1-i]	= c;
		}
	}
}

// using namespace ApexSec;

////////////////////////////////////////////////////////////////////
// CDES
////////////////////////////////////////////////////////////////////
bool	CDES::encrypt ( unsigned char key[8], unsigned char* data, int blocks )
{
   if ((!data)||(blocks<1))
	  return	false;
   deskey ( key, ENCRYPT );
   des ( data, data, blocks);
   return		true;
};

bool	CDES::decrypt ( unsigned char key[8], unsigned char* data, int blocks )
{
   if ((!data)||(blocks<1))
	  return	false;
   deskey ( key, DECRYPT );
   des ( data, data, blocks);
   return		true;
};

bool	CDES::yencrypt ( unsigned char key[8], unsigned char* data, int size )
{
   if ((!data)||(size<1))
      return	false;

   // The last char of data is bitwise complemented and filled the rest
   // buffer.If size is 16, it will extend to 24,and 17 still 24.
   char lastChar = *(data+size-1);
   int  blocks = size/8+1;
   memset (data+size, ~lastChar, blocks*8-size);
   deskey ( key, ENCRYPT );
   return encrypt ( data, data, blocks);
};

bool	CDES::ydecrypt ( unsigned char key[8], unsigned char* data, int blocks, int* size )
{
   if ( (!data) || (blocks<1) )
      return	false;

   deskey ( key, DECRYPT );
   if ( !decrypt ( data, data, blocks) )
      return	false;
   if ( size != 0 )
   {
      int pos = blocks*8-1;
      char endChar = data[pos];
      while ((pos>0)&&(data[pos]==endChar))
            pos--;
      if ( data[pos] != ~endChar )
         return	false;
      *size = pos+1;
   }
   return		true;
};

// -----------------------------------------------------------------------
// des
//      Encrpts/Decrypts(according to the key currently loaded int the
//      internal key register) SOME blocks of eight bytes at address 'in'
//      into the block at address 'out'. They can be the same.
//
//      "in"
//      "out"
//      "block"  Number of blocks.
// -----------------------------------------------------------------------
void CDES::des ( unsigned char* in, unsigned char* out, int blocks )
{
  for (int i = 0; i < blocks; i++,in+=8,out+=8)
      des_block(in,out);
};

// -----------------------------------------------------------------------
// des_block
//      Encrpts/Decrypts(according to the key currently loaded int the
//      internal key register) one block of eight bytes at address 'in'
//      into the block at address 'out'. They can be the same.
//
//      "in"
//      "out"
// -----------------------------------------------------------------------
void CDES::des_block(unsigned char *in, unsigned char *out)
{
  unsigned long work[2];

  scrunch(in, work);
  desfunc(work, KnL);
  unscrun(work, out);
}

// ----------------------------------------------------------------------
// deskey
//       Sets the internal key register (KnR) according to the hexadecimal
//       key contained in the 8 bytes of hexkey, according to the CDES,
//       for encryption or decrytion according to MODE
//
//       "key" is the 64 bits key.
//       "md"  means encryption or decryption.
// ----------------------------------------------------------------------
void CDES::deskey(unsigned char key[8], Mode md)  /* Thanks to James Gillogly &am
p; Phil Karn! */
{
  register int i, j, l, m, n;
  unsigned char pc1m[56], pcr[56];
  unsigned long kn[32];

  for (j = 0; j < 56; j++) {
    l = pc1[j];
    m = l & 07;
    pc1m[j] = (key[l >> 3] & bytebit[m]) ? 1:0;
  }
  for (i = 0; i < 16; i++) {
    if (md == DECRYPT) m = (15 - i) << 1;
      else m = i << 1;
      n = m + 1;
      kn[m] = kn[n] = 0L;
      for (j = 0; j < 28; j++) {
        l = j + totrot[i];
        if (l < 28) pcr[j] = pc1m[l];
          else pcr[j] = pc1m[l - 28];
      }
      for (j = 28; j < 56; j++) {
        l = j + totrot[i];
        if (l < 56) pcr[j] = pc1m[l];
          else pcr[j] = pc1m[l - 28];
      }
      for (j = 0; j < 24; j++) {
        if (pcr[ pc2[j] ]) kn[m] |= bigbyte[j];
        if (pcr[ pc2[j+24] ]) kn[n] |= bigbyte[j];
      }
  }
  cookey(kn);
  return;
}

// ----------------------------------------------------------------------
// cookey
//       Only called by deskey.
// -----------------------------------------------------------------------
void CDES::cookey(register unsigned long *raw1)
{
  register unsigned long *cook, *raw0;
  unsigned long dough[32];
  register int i;

  cook = dough;
  for (i = 0; i < 16; i++, raw1++) {
    raw0 = raw1++;
    *cook = (*raw0 & 0x00fc0000L) << 6;
    *cook |= (*raw0 & 0x00000fc0L) << 10;
    *cook |= (*raw1 & 0x00fc0000L) >> 10;
    *cook++ |= (*raw1 & 0x00000fc0L) >> 6;
    *cook = (*raw0 & 0x0003f000L) << 12;
    *cook |= (*raw0 & 0x0000003fL) << 16;
    *cook |= (*raw1 & 0x0003f000L) >> 4;
    *cook++ |= (*raw1 & 0x0000003fL);
  }
  usekey(dough);
  return;
}

// ----------------------------------------------------------------------
// usekey
//       Only called by cookey.
//       Loads the interal key register with the data in cookedkey.
// -----------------------------------------------------------------------
void CDES::usekey(register unsigned long *from)
{
  register unsigned long *to, *endp;

  to = KnL, endp = &KnL[32];
  while (to < endp) *to++ = *from++;
  return;
}

void CDES::scrunch(register unsigned char *outof, register unsigned long *into )
{
  *into = (*outof++ & 0xffL) << 24;
  *into |= (*outof++ & 0xffL) << 16;
  *into |= (*outof++ & 0xffL) << 8;
  *into++ |= (*outof++ & 0xffL);
  *into = (*outof++ & 0xffL) << 24;
  *into |= (*outof++ & 0xffL) << 16;
  *into |= (*outof++ & 0xffL) << 8;
  *into |= (*outof & 0xffL);
  return;
}

void CDES::unscrun(register unsigned long *outof, register unsigned char *into)
{
  *into++ = (unsigned char)( (*outof >> 24) & 0xffL );
  *into++ = (unsigned char)( (*outof >> 16) & 0xffL );
  *into++ = (unsigned char)( (*outof >> 8) & 0xffL );
  *into++ = (unsigned char)( *outof++ & 0xffL );
  *into++ = (unsigned char)( (*outof >> 24) & 0xffL );
  *into++ = (unsigned char)( (*outof >> 16) & 0xffL );
  *into++ = (unsigned char)( (*outof >> 8) & 0xffL );
  *into   = (unsigned char)( *outof & 0xffL );
  return;
}

void CDES::desfunc(register unsigned long *block,register unsigned long *keys)
{
  register unsigned long fval, work, right, leftt;
  register int round;

  leftt = block[0];
  right = block[1];
  work = ((leftt >> 4) ^ right) & 0x0f0f0f0fL;
  right ^= work;
  leftt ^= (work << 4);
  work = ((leftt >> 16) ^ right) & 0x0000ffffL;
  right ^= work;
  leftt ^= (work << 16);
  work = ((right >> 2) ^ leftt) & 0x33333333L;
  leftt ^= work;
  right ^= (work << 2);
  work = ((right >> 8) ^ leftt) & 0x00ff00ffL;
  leftt ^= work;
  right ^= (work << 8);
  right = ((right << 1) | ((right >> 31) & 1L)) & 0xffffffffL;
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = ((leftt << 1) | ((leftt >> 31) & 1L)) & 0xffffffffL;

  for (round = 0; round < 8; round++) {
	work = (right << 28) | (right >> 4);
	work ^= *keys++;
	fval  = SP7[work         & 0x3fL];
	fval |= SP5[(work >>  8) & 0x3fL];
	fval |= SP3[(work >> 16) & 0x3fL];
	fval |= SP1[(work >> 24) & 0x3fL];
	work = right ^ *keys++;
	fval |= SP8[work         & 0x3fL];
	fval |= SP6[(work >>  8) & 0x3fL];
	fval |= SP4[(work >> 16) & 0x3fL];
	fval |= SP2[(work >> 24) & 0x3fL];
	leftt ^= fval;
	work = (leftt << 28) | (leftt >> 4);
	work ^= *keys++;
	fval  = SP7[work         & 0x3fL];
	fval |= SP5[(work >>  8) & 0x3fL];
	fval |= SP3[(work >> 16) & 0x3fL];
	fval |= SP1[(work >> 24) & 0x3fL];
	work = leftt ^ *keys++;
	fval |= SP8[work         & 0x3fL];
	fval |= SP6[(work >>  8) & 0x3fL];
	fval |= SP4[(work >> 16) & 0x3fL];
    fval |= SP2[(work >> 24) & 0x3fL];
    right ^= fval;
  }
  right = (right << 31) | (right >> 1);
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = (leftt << 31) | ( leftt >> 1);
  work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
  right ^= work;
  leftt ^= (work << 8);
  work = ((leftt >> 2) ^ right) & 0x33333333L;
  right ^= work;
  leftt ^= (work << 2);
  work = ((right >> 16) ^ leftt) & 0x0000ffffL;
  leftt ^= work;
  right ^= (work << 16);
  work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
  leftt ^= work;
  right ^= (work << 4);
  *block++ = right;
  *block = leftt;
  return;
}

// -----------------------------------------------------------------------
// Initial of static data members. These data will be used by all the
// instances of class,and can not be changed.
// -----------------------------------------------------------------------
unsigned char CDES::Df_Key[24] = {
       0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
       0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
       0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67 };

unsigned short CDES::bytebit[8] = {
       0200, 0100, 040, 020, 010, 04, 02, 01 };

unsigned long CDES::bigbyte[24] = {
       0x800000L, 0x400000L, 0x200000L, 0x100000L,
       0x80000L,  0x40000L,  0x20000L,  0x10000L,
       0x8000L,   0x4000L,   0x2000L,   0x1000L,
       0x800L,    0x400L,    0x200L,    0x100L,
       0x80L,     0x40L,     0x20L,     0x10L,
       0x8L,      0x4L,      0x2L,      0x1L        };

unsigned char CDES::pc1[56] = {
       56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,
        9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35,
       62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
       13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11, 3   };

unsigned char CDES::totrot[16] = {
       1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28 };

unsigned char CDES::pc2[48] = {
       13, 16, 10, 23,  0,  4,      2, 27, 14,  5, 20,  9,
       22, 18, 11,  3, 25,  7,     15,  6, 26, 19, 12,  1,
       40, 51, 30, 36, 46, 54,     29, 39, 50, 44, 32, 47,
       43, 48, 38, 55, 33, 52,     45, 41, 49, 35, 28, 31   };

unsigned long CDES::SP1[64] = {
       0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
       0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
       0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
       0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
       0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
       0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
       0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
       0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
       0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
       0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
       0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
       0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
       0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
       0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
       0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
       0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L  };

unsigned long CDES::SP2[64] = {
       0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
       0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
       0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
       0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
       0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
       0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
       0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
       0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
       0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
       0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
       0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
       0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
       0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
       0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
       0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
       0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L  };

unsigned long CDES::SP3[64] = {
       0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
       0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
       0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
       0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
       0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
       0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
       0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
       0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
       0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
       0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
       0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
       0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
       0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
       0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
       0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
       0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L  };

unsigned long CDES::SP4[64] = {
       0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
       0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
       0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
       0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
       0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
       0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
       0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
       0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
       0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
       0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
       0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
       0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
       0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
       0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
       0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
       0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L  };

unsigned long CDES::SP5[64] = {
       0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
       0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
       0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
       0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
       0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
       0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
       0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
       0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
       0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
       0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
       0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
       0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
       0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
       0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
       0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
       0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L  };

unsigned long CDES::SP6[64] = {
       0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
       0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
       0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
       0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
       0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
       0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
       0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
       0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
       0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
       0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
       0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
       0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
       0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
       0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
       0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
       0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L  };

unsigned long CDES::SP7[64] = {
       0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
       0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
       0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
       0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
       0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
       0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
       0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
       0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
       0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
       0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
       0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
       0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
       0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
       0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
       0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
       0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L  };

unsigned long CDES::SP8[64] = {
       0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
       0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
       0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
       0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
       0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
       0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
       0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
       0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
       0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
       0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
       0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
       0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
       0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
       0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
       0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
       0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L };

bool CCFBDes::encrypt( unsigned char * in, unsigned char* out, size_t size, unsigned char * key, unsigned char *iv )
{
	unsigned char *pin,*presult;
	size_t i;
	int lsize;
	unsigned char tpin[9] = "\0\0\0\0\0\0\0\0";
	unsigned char tpresult[9] = "\0\0\0\0\0\0\0\0";

	deskey ( key, ENCRYPT );
	for(i=0,pin=in,presult=out; i<size/8; i++)
	{
		memcpy(tpin,pin,8);
		//		cipher_idea((word16*)iv,(word16*)presult,key);
		des_block( iv, presult );
		//		Xor(presult, tpin);
		DES_XOR( (char *)presult, 8, (const char *)tpin, 8 );
		memcpy(iv, presult, 8);
		pin += 8;
		presult += 8;
	}
	lsize=(int)size%8;
	if(lsize>0)
	{
		//		cipher_idea((const char *)iv,(word16*) tpresult,key);
		des_block( iv, tpresult );
		memcpy(tpin,pin,lsize);
		DES_XOR( (char *)tpresult, lsize, (const char *)tpin, lsize );
		//		Xor(tpresult, tpin);
		memcpy(presult,tpresult,lsize);
	}

	return true;
}

////////////////////////////////////////////////////////////////////
// CCFBDes
////////////////////////////////////////////////////////////////////
bool CCFBDes::decrypt( unsigned char * in, unsigned char* out, size_t size, unsigned char * key, unsigned char *iv )
{
	unsigned char *pin,*presult;
	size_t i;
	int lsize;
	unsigned char tpin[9] = "\0\0\0\0\0\0\0\0";
	unsigned char tpresult[9] = "\0\0\0\0\0\0\0\0";

	deskey ( key, ENCRYPT );
	for(i=0,pin=in,presult=out; i<size/8; i++)
	{
		memcpy(tpin,pin,8);
		des_block( iv, presult );
		DES_XOR( (char *)presult, 8, (const char *)tpin, 8 );
		memcpy(iv, tpin, 8);
		pin += 8;
		presult += 8;
	}
	lsize=(int)size%8;
	if(lsize>0)
	{
		des_block( iv, tpresult );
		memcpy(tpin,pin,lsize);
		DES_XOR( (char *)tpresult, lsize, (const char *)tpin, lsize );
		memcpy(presult,tpresult,lsize);
	}
	return true;
};
	  
////////////////////////////////////////////////////////////////////
// CSimpleDES
////////////////////////////////////////////////////////////////////
CSimpleDES::CSimpleDES()
{
	memset(m_key, 0, sizeof(m_key));
	m_pMask		= NULL;
	m_nMaskLen	= 0;
}
bool	CSimpleDES::encrypt(unsigned char* data, int len)
{
	int		nBlock	= len/8;
	bool	bRst	= true;
	if( nBlock>0 )
	{
		bRst		= CDES::encrypt(m_key, data, nBlock);
	}
	if( len>0 )
	{
		DES_XOR((char *)data, len, m_pMask, m_nMaskLen);
	}
	return	true;
}
bool	CSimpleDES::decrypt(unsigned char* data, int len)
{
	int		nBlock	= len/8;
	bool	bRst	= true;
	if( len>0 )
	{
		DES_XOR((char *)data, len, m_pMask, m_nMaskLen);
	}
	if( nBlock>0 )
	{
		bRst		= CDES::decrypt(m_key, data, nBlock);
	}
	return	true;
}

CMessageStreamDES::CMessageStreamDES()
: m_nUnitNum(0)
, m_nCount(0)
, m_nCurKeyIdx(0)
{
	memset(m_key, 0, sizeof(m_key));
	memset(m_Seed0, 0, sizeof(m_Seed0));
	memset(m_SeedInUse, 0, sizeof(m_SeedInUse));
}
bool	CMessageStreamDES::SetSeed(unsigned char *pSeed0)
{
	memcpy(m_Seed0, pSeed0, sizeof(m_Seed0));
	Reset();
	return	true;
}
void	CMessageStreamDES::Reset()
{
	m_nCount		= 0;
	m_nCurKeyIdx	= 0;
	memcpy(m_SeedInUse, m_Seed0, sizeof(m_Seed0));
}
bool	CMessageStreamDES::SetKey(unsigned char *pKey, int nUnitNum, bool bEncrypt)
{
	if( nUnitNum>MAX_UNIT_NUM )
	{
		assert(0);
		return	false;
	}
	m_nUnitNum	= nUnitNum;
	memcpy(m_key, pKey, KEY_UNIT_SIZE*m_nUnitNum);

	for(int i=0;i<nUnitNum;i++)
	{
		if( bEncrypt )
		{
			m_iDES1[i].encrypt8_setkey(m_key+i*KEY_UNIT_SIZE);
		}
		else
		{
			m_iDES1[i].decrypt8_setkey(m_key+i*KEY_UNIT_SIZE);
		}
	}
	m_nCurKeyIdx= 0;

	return	true;
}
bool	CMessageStreamDES::Encrypt(unsigned char *pMsgData, int nLen)
{
	int		nBlock	= nLen/KEY_UNIT_SIZE;
	nLen	%= KEY_UNIT_SIZE;
	for(int i=0;i<nBlock;i++)
	{
		// 先把当前数据段和Seed异或
		DES_MEM_XOR(m_SeedInUse, pMsgData, KEY_UNIT_SIZE);
		// 然后加密
		m_iDES1[m_nCurKeyIdx].encrypt8(m_SeedInUse);
		GoCurKeyToNext();
		// 然后放进去
		memcpy(pMsgData, m_SeedInUse, KEY_UNIT_SIZE);
		// 指针后移
		pMsgData	+= 8;
	}
	if( nLen>0 )
	{
		// 最后一段不足8字节的直接通过XOR加密
		DES_MEM_XOR(m_SeedInUse, pMsgData, nLen);
		DES_XOR_AC_EN(m_SeedInUse, nLen, m_key, nLen);
		DES_BYTE_PERMUTE(m_SeedInUse, nLen);
		memcpy(pMsgData, m_SeedInUse, nLen);
	}
	m_nCount	++;
	DES_XOR((char*)m_SeedInUse, sizeof(m_SeedInUse), (char *)&m_nCount, sizeof(m_nCount));
	return	true;
}
bool	CMessageStreamDES::Decrypt(unsigned char *pMsgData, int nLen)
{
	int		nBlock	= nLen/8;
	nLen	%= 8;
	unsigned char	NextSeed[8];
	for(int i=0;i<nBlock;i++)
	{
		memcpy(NextSeed, pMsgData, 8);
		// 解密
		m_iDES1[m_nCurKeyIdx].decrypt8(pMsgData);
		GoCurKeyToNext();
		// 再把当前数据段和Seed异或
		DES_MEM_XOR(pMsgData, m_SeedInUse, 8);
		// 给下一个的key
		memcpy(m_SeedInUse, NextSeed, 8);
		// 指针后移
		pMsgData	+= 8;
	}
	if( nLen>0 )
	{
		// 最后一段不足8字节的直接通过XOR解密
		memcpy(NextSeed, pMsgData, nLen);
		DES_BYTE_PERMUTE(pMsgData, nLen);
		DES_XOR_AC_DE(pMsgData, nLen, m_key, nLen);
		DES_MEM_XOR(pMsgData, m_SeedInUse, nLen);
		memcpy(m_SeedInUse, NextSeed, nLen);
	}
	m_nCount	++;
	DES_XOR((char*)m_SeedInUse, sizeof(m_SeedInUse), (char *)&m_nCount, sizeof(m_nCount));
	return	true;
}

}	// EOF namespace ApexChina