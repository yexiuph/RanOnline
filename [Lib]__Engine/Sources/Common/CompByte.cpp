#include "pch.h"
#include "compbyte.h"
#include <strstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace compbyte
{
	UCHAR ARRAY[256] = { 0xCD, 0xF0, 0xA2, 0xE2, 0xED, 0x88, 0xE3, 0x98, 0xBC, 0x66, 0xC1, 0x7C, 0xF4, 0xDB, 0x47, 0x96, 0xF6,
						0x6C, 0x5E, 0x11, 0xAF, 0x2F, 0x40, 0x42, 0x41, 0x07, 0xDE, 0x4C, 0x8A, 0x63, 0x4D, 0x51, 0xC0, 0x9B,
						0x38, 0x27, 0x19, 0x03, 0x97, 0x65, 0x3D, 0x44, 0xAC, 0xA7, 0x18, 0xA0, 0x61, 0x13, 0xB3, 0xB4, 0xC6,
						0x21, 0x15, 0xE0, 0xC5, 0x0F, 0x78, 0xC4, 0xEF, 0x2C, 0x53, 0x26, 0x2E, 0x67, 0x54, 0x5F, 0xD5, 0xC8,
						0xAA, 0x17, 0x46, 0x95, 0xA3, 0x94, 0xFB, 0xBA, 0xD3, 0xBD, 0x64, 0x2A, 0xBF, 0x34, 0x48, 0x35, 0x43,
						0xD7, 0xF5, 0xCF, 0x90, 0x92, 0x2D, 0xB5, 0x5D, 0x93, 0x99, 0x50, 0x74, 0x72, 0x31, 0x04, 0x58, 0x10,
						0x5A, 0x7F, 0xFF, 0xCA, 0x55, 0x37, 0xB2, 0xDD, 0xE5, 0x0A, 0x0D, 0x69, 0xB1, 0x3A, 0x00, 0x3C, 0xEA,
						0x22, 0x32, 0x8D, 0xF2, 0x9C, 0x86, 0x1C, 0xB0, 0x76, 0x30, 0x01, 0xD2, 0x06, 0xBB, 0x77, 0xF3, 0x80,
						0xE8, 0xA6, 0x05, 0xEC, 0x89, 0x49, 0xFD, 0xD9, 0xD6, 0xD4, 0x45, 0x6F, 0x4F, 0xB8, 0x33, 0x57, 0xD8,
						0x87, 0xA8, 0x9E, 0xF9, 0x5C, 0x23, 0xB6, 0x6B, 0xEB, 0x7E, 0x1F, 0x02, 0xFE, 0x85, 0xE9, 0x12, 0xC9,
						0xAE, 0x08, 0x9F, 0x52, 0x25, 0x71, 0x09, 0x3F, 0x29, 0x68, 0x3B, 0x1A, 0xE7, 0x91, 0x59, 0x7A, 0x6E,
						0x8E, 0x56, 0xA4, 0x1D, 0x1E, 0xA9, 0x9A, 0xDF, 0x70, 0x8C, 0x4E, 0x4B, 0xDC, 0xEE, 0x36, 0x8F, 0xC3,
						0x83, 0x82, 0xE4, 0x8B, 0x79, 0x6D, 0x0C, 0xA1, 0x7D, 0x39, 0x4A, 0xBE, 0xFA, 0xAB, 0xD1, 0xC7, 0x28,
						0x7B, 0xCC, 0xF7, 0xB7, 0xAD, 0x62, 0xB9, 0xD0, 0xF8, 0xF1, 0x0E, 0x1B, 0xCB, 0xDA, 0xE6, 0x2B, 0x60,
						0x16, 0xC2, 0x81, 0x0B, 0x73, 0xA5, 0x20, 0x84, 0x5B, 0x24, 0x9D, 0x75, 0xE1, 0xCE, 0x14, 0x6A, 0x3E,
						0xFC };

	BOOL bDEARRAY;
	UCHAR DEARRAY[256];

	void encode ( BYTE* pbuffer, DWORD dwSize )
	{
		GASSERT ( pbuffer && "compbyte::encode() -> error pbuffer==NULL" );
		for ( DWORD i=0; i<dwSize; ++i )		pbuffer[i] = ARRAY[static_cast<DWORD>(pbuffer[i])];
	}

	void decode ( BYTE* pbuffer, DWORD dwSize )
	{
		GASSERT ( pbuffer && "compbyte::encode() -> error pbuffer==NULL" );

		if ( !bDEARRAY )
		{
			bDEARRAY = TRUE;
			for ( DWORD i=0; i<256; ++i )		DEARRAY[static_cast<DWORD>(ARRAY[i])] = UCHAR(i);
		}

		for ( DWORD i=0; i<dwSize; ++i )		pbuffer[i] = DEARRAY[static_cast<DWORD>(pbuffer[i])];
	}

	//! RC4를 사용한 암호화 함수
	//! \param pbKey 는 암호화 하는데 사용되는 키 버퍼
	//! \param dwKeySize 는 암호화 하는데 사용되는 키 버퍼의 크기
	//! \param pbPlaintText 는 암호화 하는데 사용되는 원문
	//! \param pbCipherText 는 암호화 된 문장이 저장될 버퍼(원문의 크기 이상이어야 함)
	//! \param dwHowMuch 는 암호화 하는데 사용되는 원문의 크기
	//! \return GetLastError의 반환값, 성공한 경우 0 리턴
	//! \remark 모든 버퍼는 문자열이 아닌 바이트 스트림으로 간주해야 함.

	DWORD encrypt ( UCHAR *pbKey, DWORD dwKeySize, const UCHAR *pbPlaintext, UCHAR *pbCipherText, DWORD dwHowMuch )
	{
		HCRYPTPROV	hProv = NULL;
		HCRYPTKEY	hKey = NULL;
		HCRYPTHASH	hHash = NULL;

		DWORD		dwBuff = dwHowMuch;
		BOOL		dwRet = 0;
		
		memcpy(pbCipherText, pbPlaintext, dwHowMuch);

		try
		{
			if(CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE)
				throw GetLastError();

			if(CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
				throw GetLastError();

			if(CryptHashData(hHash, pbKey, dwKeySize, 0) == FALSE)
				throw GetLastError();

			if(CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey) == FALSE)
				throw GetLastError();

			if(CryptEncrypt(hKey, 0, TRUE, 0, pbCipherText, &dwBuff, dwHowMuch) == FALSE)
				throw GetLastError();
		}
		catch(const DWORD dwLastError)
		{
			dwRet = dwLastError;
		}
		
		if(hKey)	CryptDestroyKey(hKey);
		if(hHash)	CryptDestroyHash(hHash);
		if(hProv)	CryptReleaseContext(hProv, 0);

		return dwRet;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//! RC4를 사용한 암호 해독 함수
	//! \param pbKey 는 암호화 하는데 사용되는 키 버퍼
	//! \param dwKeySize 는 암호화 하는데 사용되는 키 버퍼의 크기
	//! \param pbCipherText 는 암호화된 버퍼
	//! \param pbPlaintText 는 암호가 해독된 원문(암호화된 버퍼 크기 이상이어야 함)
	//! \param dwHowMuch 는 암호화된 버퍼 크기
	//! \return GetLastError의 반환값, 성공한 경우 0 리턴
	//! \remark 모든 버퍼는 문자열이 아닌 바이트 스트림으로 간주해야 함.

	DWORD decrypt ( UCHAR *pbKey, DWORD dwKeySize, const UCHAR *pbCipherText, UCHAR *pbPlaintext, DWORD dwHowMuch )
	{
		HCRYPTPROV	hProv = NULL;
		HCRYPTKEY	hKey = NULL;
		HCRYPTHASH	hHash = NULL;
		
		DWORD		dwBuff = dwHowMuch;
		DWORD		dwRet = 0;
		
		memcpy(pbPlaintext, pbCipherText, dwHowMuch);
		
		try
		{
			if(CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE)
				throw GetLastError();
			
			if(CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
				throw GetLastError();
			
			if(CryptHashData(hHash, pbKey, dwKeySize, 0) == FALSE)
				throw GetLastError();
			
			if(CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey) == FALSE)
				throw GetLastError();
			
			if(CryptDecrypt(hKey, 0, TRUE, 0, pbPlaintext, &dwBuff) == FALSE)
				throw GetLastError();
		}
		catch(const DWORD dwLastError)
		{
			dwRet = dwLastError;
		}

		if(hKey)	CryptDestroyKey(hKey);
		if(hHash)	CryptDestroyHash(hHash);
		if(hProv)	CryptReleaseContext(hProv, 0);

		return dwRet;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//! 바이트 버퍼를 헥사값으로 출력하는 함수
	//! \param pbText 는 출력될 바이트 버퍼
	//! \param dwLength 는 출력될 바이트 버퍼 크기
	//! \return 은 없음
	//! \remark 이 함수는 출력을 완료한 후 개행함

	void hex2string ( const UCHAR *pbText, const DWORD dwLength, std::string& strDestName )
	{
		std::strstream strBuffer;

		DWORD	i;
		CHAR	*szHexaMap = "0123456789ABCDEF";
		
		for ( i=0; i<dwLength; ++i )
		{
			strBuffer << szHexaMap[pbText[i]>>4] << szHexaMap[pbText[i]&0x0f];
		}
		strBuffer << std::ends;
		
		strDestName = "";				// Note : 초기화
		strDestName = strBuffer.str();	// Note : 값을 저장
		strBuffer.freeze( false );		// Note : 동결을 없앰
	}

	UCHAR char2hex ( char cValue )
	{
		switch ( cValue )
		{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;

		case 'A': return 10;
		case 'B': return 11;
		case 'C': return 12;
		case 'D': return 13;
		case 'E': return 14;
		case 'F': return 15;

		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;

		default: GASSERT(0&&"헥사 텍스트의 문자는 0~9, A~F, a~f 사이의 값이어야함.");
		};

		return 0;
	}

	bool string2hex ( const UCHAR *pbText, UCHAR *pbHEX, const DWORD dwLength )
	{
		if( !pbText )	return false;

		bool bvalid = pbText!=NULL && pbHEX!=NULL;
		GASSERT ( bvalid && "pbText, pbHEX 버퍼가 무효한 값이다." );

		bvalid = (strlen((char*)pbText)%2)==0;
		GASSERT ( bvalid && "헥사 텍스트는 반드시 짝수의 길이를 가져야 한다." );

		DWORD j = 0;
		for ( DWORD i=0, j=0; i<dwLength; i+=2, ++j )
		{
			pbHEX[j] = ( char2hex(pbText[i]) << 4 ) | char2hex(pbText[i+1]);
		}

		pbHEX[j] = NULL;

		return true;
	}

	bool wstring2hex ( const UCHAR *pbText, WCHAR *pbHEX, const DWORD dwLength )
	{
		if( !pbText )	return false;

		bool bvalid = pbText!=NULL && pbHEX!=NULL;
		GASSERT( bvalid && "pbText, pbHEX 버퍼가 무효한 값이다." );

		bvalid = (strlen((char*)pbText)%2)==0;
		GASSERT( bvalid && "헥사 텍스트는 반드시 짝수의 길이를 가져야 한다." );

		DWORD j = 0;
		for( DWORD i=0, j=0; i<dwLength; i+=4, ++j )
		{
			pbHEX[j] = (char2hex(pbText[i]) << 12) | (char2hex(pbText[i+1]) << 8) | (char2hex(pbText[i+2]) << 4) | char2hex(pbText[i+3]) ;
		}

		pbHEX[j] = NULL;

		return true;
	}

	void Char2Hex(unsigned char ch, char* szHex)
	{
		static unsigned char saucHex[] = "0123456789ABCDEF";
		szHex[0] = saucHex[ch >> 4];
		szHex[1] = saucHex[ch&0xF];
		szHex[2] = 0;
	}

	//Function to convert a Hex string of length 2 to an unsigned char
	bool Hex2Char(char const* szHex, unsigned char& rch)
	{
		if(*szHex >= '0' && *szHex <= '9')
			rch = *szHex - '0';
		else if(*szHex >= 'A' && *szHex <= 'F')
			rch = *szHex - 55; //-'A' + 10
		else
			//Is not really a Hex string
			return false; 
		szHex++;
		if(*szHex >= '0' && *szHex <= '9')
			(rch <<= 4) += *szHex - '0';
		else if(*szHex >= 'A' && *szHex <= 'F')
			(rch <<= 4) += *szHex - 55; //-'A' + 10;
		else
			//Is not really a Hex string
			return false;
		return true;
	}

	//Function to convert binary string to hex string
	void Binary2Hex(unsigned char const* pucBinStr, int iBinSize, char* pszHexStr)
	{
		int i;
		char szHex[3];
		unsigned char const* pucBinStr1 = pucBinStr;
		*pszHexStr = 0;
		for(i=0; i<iBinSize; i++,pucBinStr1++)
		{
			Char2Hex(*pucBinStr1, szHex);
			strcat(pszHexStr, szHex);
		}
	}

	//Function to convert hex string to binary string
	bool Hex2Binary(char const* pszHexStr, unsigned char* pucBinStr, int iBinSize)
	{
		int i;
		unsigned char ch;
		for(i=0; i<iBinSize; i++,pszHexStr+=2,pucBinStr++)
		{
			if(false == Hex2Char(pszHexStr, ch))
				return false;
			*pucBinStr = ch;
		}
		return true;
	}
}