#include "pch.h"
#include <math.h>
#include <assert.h>
#include "minTea.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

minTea::minTea(const char* szKey)
{
	setKey (szKey);
}

minTea::minTea(void)
{	
	// Default Encrypt Key from SCV :-)
//	char* szKey = "Show me the money by SCV";
//	char* szKey = "Block Sheep Wall by Prob";
	char* szKey = "Steven Seagal Neck Break";

	setKey (szKey);
}

minTea::~minTea(void)
{
}

bool minTea::setKey (const char* szKey)
{
	assert (strlen(szKey) > TEA_KEY_LENGTH);

	memset (realKey, 0, TEA_KEY_LENGTH);

	UINT keyLen = (UINT) strlen (szKey);

	// Make the key TEA_KEY_LENGTH chars (128 bit) by repeating characters
	for (int n = 0; n < TEA_KEY_LENGTH; n++)
	{
		realKey[n] = szKey[n%keyLen];
	}

	return true;
}

void minTea::encrypt(char* szData, acCArray<char>& encryptedText, int nMaxLength)
{

	int keyLen = nMaxLength;
	while( keyLen > 0 )
	{
		if( szData[ keyLen-1 ] != 0 )
			break;
		--keyLen;
	}
	
	acCArray<char> text;
	text.Allocate ((int) keyLen+1, false);
	text.SetLength ((int) keyLen );

	memcpy( text.AddressOf(), szData, keyLen );

	// The text must be larger than 4 bytes
	while (text.GetLength() <= 4) 
	{
		text.PushLast(0);
	}

	// Padd text with nulls until even dividable with 4
	while (text.GetLength() & 3)
	{
		text.PushLast(0);
	}

	// Encrypt the message
	encrypt ((UINT*) text.AddressOf(),
		text.GetLength()/4,
		(UINT*) realKey);

	// Store the encrypted text for later decryption
	encryptedText.SetLength (text.GetLength());
	memcpy (encryptedText.AddressOf(),
		text.AddressOf(),
		text.GetLength());

	encryptedText.PushLast(0);
}

void minTea::decrypt(char* szData, acCArray<char>& decryptedText, int nMaxLength)
{
	// strlen으로는 문자열 길이 계산을 정확하게 할 수 없어서 수정
	/*
	// Get the encryption key from the text box
	UINT keyLen = (UINT) strlen (szData);

	// Retrieve the encrypted text for decryption	
	decryptedText.SetLength ((int) strlen(szData));

	memcpy (decryptedText.AddressOf(), szData, strlen(szData));
	*/

	// szData를 0으로 초기화하고 사용했다는 가정하에 길이 계산
	int keyLen = nMaxLength;
	while( keyLen > 0 )
	{
		if( szData[ keyLen-1 ] != 0 )
			break;
		--keyLen;
	}

	// encode할때와 같은 길이를 얻기 위해서 keyLen를 4의 배수로 맞춤
	while( keyLen & 3 )
		++keyLen;

	// Retrieve the encrypted text for decryption
	decryptedText.SetLength (keyLen);

	memcpy (decryptedText.AddressOf(), szData, keyLen);

	// Decrypt the message
	decrypt ((UINT*) decryptedText.AddressOf(),
		decryptedText.GetLength()/4,
		(UINT*) realKey);

	decryptedText.PushLast(0);
}

int minTea::encrypt(UINT *v, UINT n, UINT *k)
{
	if( n < 2 ) return -1;

	// TEA routine as per Wheeler & Needham, Oct 1998

	UINT z = v[n-1], y = v[0], delta = 0x9E3779B9;
	UINT mx, e, q = UINT(floor(6 + 52.0f/n)), sum = 0,p;

	while( q-- > 0 ) // 6 + 52/n operations gives between 6 & 32 mixes on each word
	{  
		sum += delta;
		e = sum>>2 & 3;
		for( p = 0; p < n-1; p++ ) 
		{
			y = v[p+1];
			mx = (z>>5 ^ y<<2) + (y>>3 ^ z<<4) ^ (sum^y) + (k[p&3 ^ e] ^ z);
			z = v[p] += mx;
		}
		y = v[0];
		mx = (z>>5 ^ y<<2) + (y>>3 ^ z<<4) ^ (sum^y) + (k[p&3 ^ e] ^ z);
		z = v[n-1] += mx;
	}

	return 0;
}

int minTea::decrypt(UINT *v, UINT n, UINT *k)
{
	if( n < 2 ) return -1;

	// TEA routine as per Wheeler & Needham, Oct 1998

	UINT z = v[n-1], y = v[0], delta = 0x9E3779B9;
	UINT mx, e, q = UINT(floor(6 + 52.0f/n)), sum = q*delta,p;

	while (sum != 0) 
	{
		e = sum>>2 & 3;
		for( p = n-1; p > 0; p-- ) 
		{
			z = v[p-1];
			mx = (z>>5 ^ y<<2) + (y>>3 ^ z<<4) ^ (sum^y) + (k[p&3 ^ e] ^ z);
			y = v[p] -= mx;
		}
		z = v[n-1];
		mx = (z>>5 ^ y<<2) + (y>>3 ^ z<<4) ^ (sum^y) + (k[p&3 ^ e] ^ z);
		y = v[0] -= mx;
		sum -= delta;
	}

	return 0;
}

bool minTea::encrypt (char* szData, int nMaxLength)
{
	acCArray<char> encryptedText;
	encrypt (szData, encryptedText, nMaxLength);	

	int nEncryptedLength = encryptedText.GetLength();
	if (nEncryptedLength > nMaxLength)
	{
		/// Encrypted length error
		return false;
	}
	else
	{
		memcpy (szData, encryptedText.AddressOf(), nEncryptedLength);
		return true;
	}
}

bool minTea::decrypt (char* szData, int nMaxLength)
{
	acCArray<char> decryptedText;
	decrypt (szData, decryptedText, nMaxLength);

	int nDecryptedLength = decryptedText.GetLength();

	if (nDecryptedLength > nMaxLength)
	{
		/// Decrypted length error
		return false;
	}
	else
	{
		memcpy (szData, decryptedText.AddressOf(), nDecryptedLength);
		return true;
	}
}