//
//	[class CStringFile], (2002.11.26), JDH.
//
//	UPDATE : (2002.12.07), JDH, 메소드 호출시에 파일이 열리지 않았을때 예외 처리를
//			하여 오류 발생을 방지.
//
//
#pragma once

#include "BaseString.h"

class CStringFile : public CBaseString
{
public:
	CStringFile ( UINT nBufSize = 1024 );
	virtual ~CStringFile ();
	
public:
	BOOL Open ( LPCSTR szFile, BOOL bDecode=FALSE );
	virtual void Close ();

public:
	virtual void Reset (void);

public:
	virtual DWORD GetNextLine ( LPSTR szLine,UINT iLineSize );
	virtual DWORD GetNextLine ( PSTR &szString );
	virtual DWORD GetNextLine ( CString &strString );

protected:
	DWORD	m_nMaxSize;
	DWORD	m_dwMasterIndex;
	int		m_nSectionCount;
	FILE*	m_fFile;
};

