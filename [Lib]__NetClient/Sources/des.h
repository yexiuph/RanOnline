#ifndef __DES_H__
#define __DES_H__

#include <stdlib.h>

namespace ApexChina
{

class   CDES
{
public:
	// Encrypt/decrypt the data in "data", according to the "key".
	// Caller is responsible for confirming the buffer size of "data"
	// points to is 8*"blocks" bytes.
	// The data encrypted/decrypted is stored in data.
	// The return code is 1:success, other:failed.
	bool encrypt( unsigned char key[8], unsigned char* data, int blocks = 1 );
	bool decrypt( unsigned char key[8], unsigned char* data, int blocks = 1 );

	// key
	inline void	encrypt8_setkey( unsigned char key[8] )
	{
		deskey( key, ENCRYPT );
	}

	inline void	encrypt8( unsigned char *data )
	{
		des_block(data, data);
	}

	inline void	decrypt8_setkey( unsigned char key[8] )
	{
		deskey( key, DECRYPT );
	}

	inline void	decrypt8( unsigned char *data )
	{
		des_block(data, data);
	}

	// Encrypt/decrypt any size data,according to a special method.
	// Before calling yencrypt, copy data to a new buffer with size
	// calculated by extend.
	bool yencrypt( unsigned char key[8], unsigned char* data, int size );
	bool ydecrypt( unsigned char key[8], unsigned char* in, int blocks, int* size = 0 );
	int	 extend( int size ) { return (size/8+1)*8; };

public:
	void des( unsigned char* in, unsigned char* out, int blocks );
	void des_block( unsigned char* in, unsigned char* out );

public:
	unsigned long KnL[32];
	enum Mode { ENCRYPT, DECRYPT };
	void deskey( unsigned char key[8], Mode md );
	void usekey( unsigned long * );
	void cookey( unsigned long * );

private:
	void scrunch(unsigned char *, unsigned long *);
	void unscrun(unsigned long *, unsigned char *);
	void desfunc(unsigned long *, unsigned long *);

private:
	static unsigned char Df_Key[24];
	static unsigned short bytebit[8];
	static unsigned long bigbyte[24];
	static unsigned char pc1[56];
	static unsigned char totrot[16];
	static unsigned char pc2[48];
	static unsigned long SP1[64];
	static unsigned long SP2[64];
	static unsigned long SP3[64];
	static unsigned long SP4[64];
	static unsigned long SP5[64];
	static unsigned long SP6[64];
	static unsigned long SP7[64];
	static unsigned long SP8[64];
};

class CCFBDes	: public CDES
{
public:
	bool	encrypt( unsigned char * in, unsigned char* out, size_t size, unsigned char * key, unsigned char *iv );
	bool	decrypt( unsigned char * in, unsigned char* out, size_t size, unsigned char * key, unsigned char *iv );
};
// �򵥵ģ����䳤�ļ��ܣ������8�Ĳ��ֲ���des����ֱ�����������룩
class   CSimpleDES	: public CDES
{
public:
	// ���ֱ����������Ϳ�����
	unsigned char	m_key[8];
protected:
	// �����Ҫͨ����������
	const char		*m_pMask;
	int				m_nMaskLen;
public:
	CSimpleDES();
	bool	encrypt(unsigned char* data, int len);
	bool	decrypt(unsigned char* data, int len);
	inline void	setmask(const char *pMask, int nMaskLen)
	{
		m_pMask		= pMask;
		m_nMaskLen	= nMaskLen;
	}
	inline void	clrmask()
	{
		m_pMask		= NULL;
		m_nMaskLen	= 0;
	}
};

// �ۻ���DES���ܣ��ȶ���Ϣ�����ܡ�ǰһ����Ϣ�ļ��ܽ����Ӱ���һ����Ϣ�ļ��ܡ�
class   CMessageStreamDES
{
public:
	enum
	{
		KEY_UNIT_SIZE	= 8,
		MAX_UNIT_NUM	= 128,
		MAX_KEY_SIZE	= KEY_UNIT_SIZE*MAX_UNIT_NUM,
	};
private:
	// 8λ������
	int				m_nUnitNum;
	// �ܹ����ܵĸ���(��shortֻ��Ϊ�˿��Ծ���ı�)
	unsigned short	m_nCount;
	// ��Կ
	unsigned char	m_key[MAX_KEY_SIZE];
	// �ۻ������ʼֵ
	unsigned char	m_Seed0[KEY_UNIT_SIZE];
	// ʹ���е��ۻ�����
	unsigned char	m_SeedInUse[KEY_UNIT_SIZE];
	// ����������ܶ���
	CDES			m_iDES1[MAX_UNIT_NUM];
	// ��ǰ��key�����
	int				m_nCurKeyIdx;
private:
	inline void	GoCurKeyToNext()
	{
		m_nCurKeyIdx	++;
		if( m_nCurKeyIdx>=m_nUnitNum )
		{
			m_nCurKeyIdx= 0;
		}
	}
public:
	CMessageStreamDES();
	bool	SetSeed(unsigned char *pSeed0);
	// ���¿�ʼ
	void	Reset();
	// ����Ч�ʵļӽ���
	bool	SetKey(unsigned char *pKey, int nUnitNum, bool bEncrypt);
	// ����
	bool	Encrypt(unsigned char *pMsgData, int nLen);
	// ����
	bool	Decrypt(unsigned char *pMsgData, int nLen);
};


// ��һ�����ⳤ�ȵ����ݴ����һ���̵Ķ������ݴ�
// ���bAppendΪ�棬�򸽼ӵ�szTo�ϣ�����szTo�ʼ�����㣩
bool	DES_ConvertToFixed(const char *szFrom, int nFrom, char *szTo, int nTo, bool bAppend=false);
// ��һ�����ݴ�ͨ��һ���̵Ķ���������/����
bool	DES_XOR(char *szDst, int nDst, const char *szKey, int nKey);
// ���ۻ��ļ��ܽ���(AC��ʾAccumulate)
bool	DES_XOR_AC_EN(unsigned char *szDst, int nDst, const unsigned char *szKey, int nKey);
bool	DES_XOR_AC_DE(unsigned char *szDst, int nDst, const unsigned char *szKey, int nKey);
// ��һ���ڴ�cszSrc�����һ��szDst
void	DES_MEM_XOR(unsigned char *szDst, const unsigned char *cszSrc, int nLen);

// ���ݴ�λ
void	DES_BYTE_PERMUTE(unsigned char *szDst, int nLen);
}		// EOF namespace ApexSec

#endif  // EOF __DES_H__
