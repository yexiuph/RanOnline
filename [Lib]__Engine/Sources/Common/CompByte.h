#pragma once
#include <string>

namespace compbyte
{
	void encode ( BYTE* pbuffer, DWORD dwSize );
	void decode ( BYTE* pbuffer, DWORD dwSize );

	DWORD encrypt ( UCHAR *pbKey, DWORD dwKeySize, const UCHAR *pbPlaintext, UCHAR *pbCipherText, DWORD dwHowMuch );
	DWORD decrypt ( UCHAR *pbKey, DWORD dwKeySize, const UCHAR *pbCipherText, UCHAR *pbPlaintext, DWORD dwHowMuch );

	void hex2string ( const UCHAR *pbText, const DWORD dwLength, std::string& strDestName );
	bool string2hex ( const UCHAR *pbText, UCHAR *pbHEX, const DWORD dwLength );
	bool wstring2hex ( const UCHAR *pbText, WCHAR *pbHEX, const DWORD dwLength );

	void Char2Hex(unsigned char ch, char* szHex);
	//Function to convert a Hex string of length 2 to an unsigned char
	bool Hex2Char(char const* szHex, unsigned char& rch);
	//Function to convert binary string to hex string
	void Binary2Hex(unsigned char const* pucBinStr, int iBinSize, char* pszHexStr);
	//Function to convert hex string to binary string
	bool Hex2Binary(char const* pszHexStr, unsigned char* pucBinStr, int iBinSize);
};
