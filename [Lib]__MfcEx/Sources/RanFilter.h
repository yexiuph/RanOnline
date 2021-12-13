// By 경대
#pragma once
#include <string>
#include <vector>
#include "Slangfilter.h"

class CBaseString;

class CRanFilter
{
	enum
	{
		EM_ERROR_CODE = 0,
		EM_UNICODE,
		EM_HEX,
		EM_CHAR
	};

protected:
	SlangFilter m_ChatFilter;
	SlangFilter m_NameFilter;

	CString m_strFilted;

	SlangFilter* m_pCurrentFilter;

	BOOL m_bZipFile;
	BOOL m_bPackFile;

protected:
	UINT m_nCodePage; // 유니코드 코드페이지

public:
	BOOL Init( CONST BOOL bZipFile, CONST BOOL bPackFile, CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	BOOL ChatFilter( CONST CString & );
	BOOL NameFilter( CONST CString & );
	CONST CString & GetProverb() { return m_strFilted; }

	VOID SetCodePage( UINT nCodePage ) { m_nCodePage = nCodePage; }
	UINT GetCodePage() { return m_nCodePage; }

private:
	BOOL ReadToFile( CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	BOOL Filter( CONST CString &, CONST SlangFilter & );
	//INT Nrand(); // 난수 발생

private:
	BOOL LOAD( CONST TCHAR* szZipFile, CONST TCHAR* szPath, CONST TCHAR* szFile );
	BOOL LOADCHAT( CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	BOOL LOADNAME( CONST TCHAR* szZipFile, CONST TCHAR* szPath );
	//bool LOADPROVERB( const BOOL bZipFile, const TCHAR* szZipFile, const TCHAR* szPath );

	UINT GetSectionCode( CONST CString & strSection );

	VOID InputStringHEX( CONST CString & strBUFFER );
	VOID InputStringCHAR( CONST CString & strBUFFER );
	VOID InputStringUNICODE( CONST CString & strBUFFER );

private:
	CRanFilter();
	~CRanFilter();

public:
	static CRanFilter& GetInstance();
};