
//Rijndael.h

#ifndef __RIJNDAEL_H__
#define __RIJNDAEL_H__

//Typical DISCLAIMER:
//The code in this project is Copyright (C) 2003 by George Anescu. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.

#include "Method.h"

//Rijndael (pronounced Reindaal) is a block cipher, designed by Joan Daemen and Vincent Rijmen as a candidate algorithm for the AES.
//The cipher has a variable block length and key length. The authors currently specify how to use keys with a length
//of 128, 192, or 256 bits to encrypt blocks with al length of 128, 192 or 256 bits (all nine combinations of
//key length and block length are possible). Both block length and key length can be extended very easily to
// multiples of 32 bits.
//Rijndael can be implemented very efficiently on a wide range of processors and in hardware. 
//This implementation is based on the Java Implementation used with the Cryptix toolkit found at:
//http://www.esat.kuleuven.ac.be/~rijmen/rijndael/rijndael.zip
//Java code authors: Raif S. Naffah, Paulo S. L. M. Barreto
//This Implementation was tested against KAT test published by the authors of the method and the
//results were identical.
//TEST
//KEYSIZE=128
//KEY=000102030405060708090A0B0C0D0E0F
//PT=000102030405060708090A0B0C0D0E0F
//CT=0A940BB5416EF045F1C39458C653EA5A
//
//KEYSIZE=192
//KEY=000102030405060708090A0B0C0D0E0F1011121314151617
//PT=000102030405060708090A0B0C0D0E0F
//CT=0060BFFE46834BB8DA5CF9A61FF220AE
//
//KEYSIZE=256
//KEY=000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
//PT=000102030405060708090A0B0C0D0E0F
//CT=5A6E045708FB7196F02E553D02C3A692
//
//BLOCK SIZE 16
// KEY                TEXT                              RESULT
// abcdefghabcdefgh   aaaaaaaabbbbbbbb                  5E0A63B893A5372E37D3243F1F2406CE
// In CBC mode
// abcdefghabcdefgh   aaaaaaaabbbbbbbbaaaaaaaabbbbbbbb  5E0A63B893A5372E37D3243F1F2406CE4A6CC9DE9E9A97C0FA3A1C438FC7523C

#define RANSEDIT_VERSION 0x0005

class CRijndael : public IMethod
{
public:
	//Key Lengths
	enum { KL16=16, KL24=24, KL32=32 };
	//Block Sizes
	enum { BS16=16, BS24=24, BS32=32 };
	//keylength  - 16, 24 or 32 bytes
	enum { DEFAULT_KEY_LENGTH = 32 };
	//blockSize  - 16, 24 or 32 bytes
	enum { DEFAULT_BLOCK_SIZE = 16 };

	enum { VERSION = RANSEDIT_VERSION, VERSION_DATE = 20171011 };

private:
	enum { DATA_LEN=384, BUFF_LEN=1024 };
	enum { MAX_ROUNDS=14, MAX_KC=8, MAX_BC=8 };

	//Auxiliary Functions
	//Multiply two elements of GF(2^m)
	static int Mul(int a, int b)
	{
		return (a != 0 && b != 0) ? sm_alog[(sm_log[a & 0xFF] + sm_log[b & 0xFF]) % 255] : 0;
	}

	//Convenience method used in generating Transposition Boxes
	static int Mul4(int a, char b[])
	{
		if(a == 0)
			return 0;
		a = sm_log[a & 0xFF];
		int a0 = (b[0] != 0) ? sm_alog[(a + sm_log[b[0] & 0xFF]) % 255] & 0xFF : 0;
		int a1 = (b[1] != 0) ? sm_alog[(a + sm_log[b[1] & 0xFF]) % 255] & 0xFF : 0;
		int a2 = (b[2] != 0) ? sm_alog[(a + sm_log[b[2] & 0xFF]) % 255] & 0xFF : 0;
		int a3 = (b[3] != 0) ? sm_alog[(a + sm_log[b[3] & 0xFF]) % 255] & 0xFF : 0;
		return a0 << 24 | a1 << 16 | a2 << 8 | a3;
	}

public:
	//CONSTRUCTOR
	CRijndael();

	//DESTRUCTOR
	virtual ~CRijndael();

	//Expand a user-supplied key material into a session key.
	// key        - The 128/192/256-bit user-key to use.
	// chain      - initial chain block for CBC and CFB modes.
	// keylength  - 16, 24 or 32 bytes
	// blockSize  - 16, 24 or 32 bytes
	// iMode=ECB  - Operation Mode
	// iPadding   - Padding Mode
	void Initialize(char const* keydata, int keydatalength, int versiondate, int version, char const* chain=sm_chain0, int keylength=DEFAULT_KEY_LENGTH,
		int blockSize=DEFAULT_BLOCK_SIZE, int iMode=ECB, int iPadding=ZEROES);

	//Resetting the Initialization Vector
	void ResetChain();
	//Encryption for a string of chars
	void Encrypt(char const* in, char* result, size_t n);
	//Decryption for a string of chars
	void Decrypt(char const* in, char* result, size_t n);
	//Encryption for a File
	void EncryptFile(string const& rostrFileIn, string const& rostrFileOut);
	//Decryption for a File
	void DecryptFile(string const& rostrFileIn, string const& rostrFileOut);

	void EncryptEx( const char * in, char * out, int n );
	void DecryptEx( const char * in, char * out, int n );
	void DecryptEx( char * in, int n );

private:
	//Convenience method to encrypt exactly one block of plaintext, assuming
	//Rijndael's default block size (128-bit, 16-bytes).
	// in         - The plaintext
	// result     - The ciphertext generated from a plaintext using the key
	void DefEncryptBlock(char const* in, char* result);

	//Convenience method to decrypt exactly one block of plaintext, assuming
	//Rijndael's default block size (128-bit, 16-bytes).
	// in         - The ciphertext.
	// result     - The plaintext generated from a ciphertext using the session key.
	void DefDecryptBlock(char const* in, char* result);

	//Compute Signature
	void Signature(char* pcSig);

	//Compute key string
	bool ComputeKeyString(int nVer, int nYear, int nLength, char const* data, char* output);

	//Compute base64 encoded key
	void Base64EncodeKey(unsigned char const* in, char* result, int size);

	//Encrypt exactly one block of plaintext.
	// in           - The plaintext.
    // result       - The ciphertext generated from a plaintext using the key.
    void EncryptBlock(char const* in, char* result);
	
	//Decrypt exactly one block of ciphertext.
	// in         - The ciphertext.
	// result     - The plaintext generated from a ciphertext using the session key.
	void DecryptBlock(char const* in, char* result);

	static const int sm_alog[256];
	static const int sm_log[256];
	static const char sm_S[256];
    static const char sm_Si[256];
    static const int sm_T1[256];
    static const int sm_T2[256];
    static const int sm_T3[256];
    static const int sm_T4[256];
    static const int sm_T5[256];
    static const int sm_T6[256];
    static const int sm_T7[256];
    static const int sm_T8[256];
    static const int sm_U1[256];
    static const int sm_U2[256];
    static const int sm_U3[256];
    static const int sm_U4[256];
    static const char sm_rcon[30];
    static const int sm_shifts[3][4][2];
	//The Key
	char m_acKey[KL32];
	//Encryption (m_Ke) round key
	int m_Ke[MAX_ROUNDS][MAX_BC];
	//Decryption (m_Kd) round key
    int m_Kd[MAX_ROUNDS+1][MAX_BC];
	//Number of Rounds
	int m_iROUNDS;
	//Chain Block
	char m_chain0[BS32];
	char m_chain[BS32];
	//Auxiliary private use buffers
	int tk[MAX_KC];
	int a[MAX_BC];
	int t[MAX_BC];

public:
	//Null chain
	static char const sm_chain0[BS32]; //Max blocksize assumed
	static string const sm_Version[VERSION];
	static DWORD  const sm_KeyLength[VERSION];
};

#endif // __RIJNDAEL_H__

