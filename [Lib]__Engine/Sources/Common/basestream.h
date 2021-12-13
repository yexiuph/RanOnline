#pragma once

#include "./gassert.h"
#include "Rijndael.h"
#include <string>
#include <vector>

#define CALLERROR GASSERT("0&&basestream::operator<<()는 호출되지 않습니다.")

enum FOPENTYPE
{
	FOT_READ,
	FOT_WRITE
};

#define FILETYPESIZE	128

enum EMSFILE_BLOCK
{
	EMBLOCK_00 = 0,
	EMBLOCK_01 = 1,
	EMBLOCK_02 = 2,
	EMBLOCK_03 = 3,
	EMBLOCK_04 = 4,

	EMBLOCK_05 = 5,
	EMBLOCK_06 = 6,
	EMBLOCK_07 = 7,
	EMBLOCK_08 = 8,
	EMBLOCK_09 = 9,

	EMBLOCK_SIZE = 10,
};

class basestream
{
protected:
	BOOL		m_IsFileOpened;
	FOPENTYPE	m_OpenType;
	bool		m_bencode;
	INT			m_nVersion;
	
	char		m_szFileName[_MAX_FNAME];
	char		m_szFileType[FILETYPESIZE];
	DWORD		m_FileID;

	DWORD		m_DefaultOffSet;
	CRijndael	m_oRijndael;

protected:
	virtual void ReadFileType (){CALLERROR;}
	virtual void WriteFileType (){CALLERROR;}

public:
	virtual void SetFileType ( const char *szFileType, DWORD FileID ){CALLERROR;}
	virtual void GetFileType ( char *szFileType, int nStrLen, DWORD &FileID ){CALLERROR;}
	virtual void SetEncode ( bool bencode ){CALLERROR;}

	virtual BOOL IsOpen (){CALLERROR; return true;}
	virtual void CloseFile (){CALLERROR;}
	virtual const char* GetFileName() { return m_szFileName; }

public:
	virtual DWORD GetDefaltOffSet (){CALLERROR; return UINT_MAX;}		//	기본 저장위치 옵셋을 가져옴.

protected:
	long	m_nMarkPos[EMBLOCK_SIZE];		// 마킹 되는 것이 10개가 넘어 버리면 문제가 생긴다.
	DWORD	m_dwBlockMarkPOS;

public:
	virtual BOOL BeginBlock ( EMSFILE_BLOCK emBLOCK=EMBLOCK_00 ){CALLERROR; return true;};
	virtual BOOL EndBlock ( EMSFILE_BLOCK emBLOCK=EMBLOCK_00 ){CALLERROR; return true;};

	virtual DWORD ReadBlockSize (){CALLERROR; return UINT_MAX;}

protected:
	virtual size_t read ( void *pbuffer, DWORD dwSize ){CALLERROR; return UINT_MAX;}
	virtual size_t write ( const void* pbuffer, DWORD dwSize ){return UINT_MAX;}

// ------- 저장하기.
public:
	virtual BOOL operator << ( short Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( int Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( WORD Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( DWORD Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( UINT Value ){CALLERROR; return TRUE;}

	virtual BOOL operator << ( float Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( double Value ){CALLERROR; return TRUE;}

	virtual BOOL operator << ( D3DVECTOR Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( D3DCOLORVALUE &Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( D3DBLEND &Value ){CALLERROR; return TRUE;}

	virtual BOOL operator << ( BYTE Value ){CALLERROR; return TRUE;}
	virtual BOOL operator << ( char Value ){CALLERROR; return TRUE;}

	virtual BOOL operator << ( bool Value ){CALLERROR; return TRUE;}

	virtual BOOL operator << ( const std::string &str ){CALLERROR;return TRUE;}

	template<typename TYPE>
		BOOL operator << ( const std::vector<TYPE> &vecVALUE );

	virtual BOOL WriteBuffer ( const void* pBuffer, DWORD Size ){CALLERROR;return TRUE;}

// ------- 불러오기.
public:
	virtual BOOL operator >> ( short &Value ) = 0;
	virtual BOOL operator >> ( int &Value ) = 0;
	virtual BOOL operator >> ( WORD &Value ) = 0;
	virtual BOOL operator >> ( DWORD &Value ) = 0;
	virtual BOOL operator >> ( UINT &Value ) = 0;

	virtual BOOL operator >> ( float &Value ) = 0;
	virtual BOOL operator >> ( double &Value ) = 0;

	virtual BOOL operator >> ( D3DVECTOR &Value ) = 0;
	virtual BOOL operator >> ( D3DCOLORVALUE &Value ) = 0;
	virtual BOOL operator >> ( D3DBLEND &Value ) = 0;

	virtual BOOL operator >> ( BYTE &Value ) = 0;
	virtual BOOL operator >> ( char &Value ) = 0;

	virtual BOOL operator >> ( bool &Value ) = 0;

	virtual BOOL operator >> ( std::string &str ) = 0;

	template<typename TYPE>
		BOOL operator >> ( std::vector<TYPE> &vecVALUE );

	virtual BOOL ReadBuffer ( void* pBuffer, DWORD Size ) = 0;

public:
	virtual BOOL SetOffSet ( long _OffSet ) = 0;
	virtual long GetfTell () = 0;

public:
	basestream() :
		m_IsFileOpened(FALSE),
		m_OpenType(FOT_READ),
		m_FileID(0),
		m_DefaultOffSet(0),
		m_dwBlockMarkPOS(0),
		m_bencode(false),
		m_nVersion(-1)
	{
		SecureZeroMemory ( m_szFileName, sizeof(m_szFileName) );
		SecureZeroMemory ( m_szFileType, sizeof(m_szFileType) );
		SecureZeroMemory ( m_nMarkPos, sizeof(m_nMarkPos) );
		StringCchCopy( m_szFileType, FILETYPESIZE, "default" );
	}

	virtual ~basestream(){}
};

template<typename TYPE>
BOOL basestream::operator << ( const std::vector<TYPE> &vecVALUE )
{
	BOOL bOK(FALSE);
	DWORD dwVecSize = (DWORD)vecVALUE.size();
	bOK = operator << ( dwVecSize );
	if ( !bOK )					return FALSE;

	if ( vecVALUE.empty() )		return TRUE;
	return WriteBuffer ( &(vecVALUE[0]), DWORD(sizeof(TYPE)*vecVALUE.size()) );
}

template<typename TYPE>
BOOL basestream::operator >> ( std::vector<TYPE> &vecVALUE )
{
	BOOL bOK(FALSE);
	DWORD dwSize(0);

	vecVALUE.clear();

	bOK = operator >> ( dwSize );
	if ( !bOK )					return FALSE;
	
	if ( dwSize==0 )			return TRUE;

	vecVALUE.reserve(dwSize);
	for ( DWORD i=0; i<dwSize; ++i )
	{
		TYPE tVALUE;
		bOK = ReadBuffer ( &tVALUE, DWORD(sizeof(TYPE)) );
		if ( !bOK )				return FALSE;

		vecVALUE.push_back ( tVALUE );
	}

	return TRUE;
}