#include "stdafx.h"
#include "DaumGameAuth.h"
#include <stdlib.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p)	if( p ) { delete (p); (p) = NULL; }
#endif

CDaumGameAuth::CDaumGameAuth( void ) : m_pCryptor( NULL )
{
}

CDaumGameAuth::~CDaumGameAuth( void )
{
	SAFE_DELETE( m_pCryptor );
}

BOOL CDaumGameAuth::Init( LPCTSTR szKey )
{
	SAFE_DELETE( m_pCryptor );
	m_pCryptor = new CDaumGameCrypt;
	m_pCryptor->SetKey( szKey );
	return TRUE;
}

BOOL CDaumGameAuth::SetSource( LPCTSTR szSourceString )
{
	if( m_pCryptor->Decrypt( szSourceString, m_szSourceString, MAX_SOURCE_LENGTH ) == FALSE )
		return FALSE;
	return TRUE;
}

BOOL CDaumGameAuth::GetData( IN LPCTSTR szKeyName, OUT LPTSTR szBuffer, int nMaxLength )
{
	TCHAR szFullKeyName[ MAX_KEY_LENGTH ];
	TCHAR *pData, *pDestData;
	int nBufferSize;

	*szBuffer = '\0';

	_tcscpy( szFullKeyName, szKeyName );
	_tcscat( szFullKeyName, "=" );

	pData	= _tcsstr( m_szSourceString, szFullKeyName ); 
	if( pData == NULL )
		return FALSE;
	pData	+= _tcslen( szFullKeyName );
	if( (pDestData = _tcschr( pData, '|' )) == NULL )
		pDestData = pData + _tcslen( pData );

	nBufferSize = (int) (INT_PTR) ((TCHAR *) pDestData - (TCHAR *) pData);

	if( nMaxLength <= nBufferSize )
		nBufferSize = nMaxLength - 1;
	_tcsncpy( szBuffer, pData, nBufferSize );
	*(szBuffer + nBufferSize) = '\0';

	return TRUE;
}

BOOL CDaumGameAuth::IsTimeExpired( void )
{
	TCHAR szTime[256];

	if( GetData( _T("TS"), szTime, 256 ) == FALSE )
		return TRUE;
	if( _ttoi( szTime ) <= (time( NULL ) - EXPIRED_SECS) )
		return TRUE;
	return FALSE;
}

