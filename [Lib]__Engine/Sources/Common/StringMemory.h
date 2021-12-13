#pragma once

#include "BaseString.h"

class SFileSystem;

class CStringMemory : public CBaseString
{
public:
	CStringMemory ( UINT nBufSize = 1024 ); // 버퍼를 1024로 제안해야 하는가...?
	virtual ~CStringMemory ();
	
public:
	BOOL Open ( LPCSTR _szZipFileName, 
				LPCSTR _szFileName, 
				BOOL bDecode = FALSE, 
				BOOL bPack = FALSE );

	virtual void Close ();

public:
	virtual void Reset (void);

public:
	virtual DWORD GetNextLine ( LPSTR szLine,UINT iLineSize );
	virtual DWORD GetNextLine ( PSTR &szString );
	virtual DWORD GetNextLine ( CString &strString );

protected:
	DWORD		m_dwOffSet; // 메모리 Offset;
	UINT		m_nBufSize;
	BYTE*		m_pBufferMem;
	UINT		m_nBufSizeTemp;
};
