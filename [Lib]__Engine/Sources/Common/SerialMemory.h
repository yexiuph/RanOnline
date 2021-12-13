// by 경대
//
#pragma once

#include "basestream.h"

class SFileSystem;

class CSerialMemory : public basestream
{
protected:
	char		m_szZipFileName[_MAX_PATH];

	PBYTE		m_pBuffer;
	UINT		m_nBufSize;
	DWORD		m_dwOffSet;
	
protected:
	virtual void ReadFileType ();

public:
	virtual void GetFileType( char *szFileType, int nStrLen, DWORD &FileID );
	virtual void SetEncode ( bool bencode )		{ m_bencode = bencode; }

	BOOL OpenFile ( const char *_szZipFileName, 
					const char *_szFileName, 
					bool bencode = false, 
					BOOL bPack = FALSE );

	virtual BOOL IsOpen () { return m_IsFileOpened; }
	virtual void CloseFile ();

public:
	virtual DWORD GetDefaltOffSet () { return m_DefaultOffSet; }	//	기본 저장위치 옵셋을 가져옴.

public:
	virtual BOOL SetOffSet ( long _OffSet );				//	파일 저장 위치를 변경.
	virtual long GetfTell ();								//	현제 저장위치를 가져옴.

public:
	virtual BOOL BeginBlock ( EMSFILE_BLOCK emBLOCK=EMBLOCK_00 );
	virtual BOOL EndBlock ( EMSFILE_BLOCK emBLOCK=EMBLOCK_00 );

	virtual DWORD ReadBlockSize ();

protected:
	virtual size_t read ( void *pbuffer, DWORD dwSize );

// ------- 불러오기.
public:
	virtual BOOL operator >> ( short &Value );
	virtual BOOL operator >> ( int &Value );
	virtual BOOL operator >> ( WORD &Value );
	virtual BOOL operator >> ( DWORD &Value );
	virtual BOOL operator >> ( UINT &Value );

	virtual BOOL operator >> ( float &Value );
	virtual BOOL operator >> ( double &Value );

	virtual BOOL operator >> ( D3DVECTOR &Value );
	virtual BOOL operator >> ( D3DCOLORVALUE &Value );
	virtual BOOL operator >> ( D3DBLEND &Value );

	virtual BOOL operator >> ( BYTE &Value );
	virtual BOOL operator >> ( char &Value );
	virtual BOOL operator >> ( std::string &str );

	virtual BOOL operator >> ( bool &Value );

	template<typename TYPE>
	BOOL operator >> ( std::vector<TYPE> &vecVALUE );

	virtual BOOL ReadBuffer ( void* pBuffer, DWORD Size );

public:
	CSerialMemory ();
	CSerialMemory ( const char *_szZipFileName, const char *_szFileName, bool bencode = false );
	virtual ~CSerialMemory ();
};

template<typename TYPE>
BOOL CSerialMemory::operator >> ( std::vector<TYPE> &vecVALUE )
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