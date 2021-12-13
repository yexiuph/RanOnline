#pragma once

#include "Rijndael.h" //{Lib]__Engine/Sources/Common

class CBaseString  
{
public:
	CBaseString ();
	virtual ~CBaseString ();
	
public:
	virtual void Close () = 0;

public:
	virtual void Reset (void) = 0;

public:
	virtual DWORD GetNextLine ( LPSTR szLine,UINT iLineSize ) = 0;
	virtual DWORD GetNextLine ( PSTR &szString ) = 0;
	virtual DWORD GetNextLine ( CString &strString ) = 0;

protected:
	UINT		m_nBufferSize;
	DWORD		m_dwRead;
	DWORD		m_dwLine;
	DWORD		m_dwIndex;
	BYTE*		m_pBuffer;
	BOOL		m_bDecode;
	INT			m_nVersion;
	TCHAR		m_szFileName[MAX_PATH];
	CRijndael	m_oRijndael;
};
