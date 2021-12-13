#include "pch.h"
#include "DaumGameParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define	DGP_NAME	"DaumGameParameterFormat"

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p)				if( NULL != p ) { delete p; p = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(p)		if( NULL != p ) { delete [] p; p = NULL; }
#endif

CDaumGameParameter::CDaumGameParameter( void ) : m_pMemoryBlock(NULL), m_bModified(FALSE)
{
	Init();
}

CDaumGameParameter::~CDaumGameParameter( void )
{
	Destroy();
}

BOOL CDaumGameParameter::Init( void )
{
	m_uDGPFormat = ::RegisterClipboardFormat( DGP_NAME );
	return TRUE;
}

BOOL CDaumGameParameter::Flush( void )
{
	if( m_bModified == FALSE )
		return FALSE;
	if( m_pMemoryBlock == NULL )
		return FALSE;
	if( ::OpenClipboard( NULL ) == FALSE )
		return FALSE;
	::SetClipboardData( m_uDGPFormat, m_pMemoryBlock->CreateHandle() );
	::CloseClipboard();
	m_bModified = FALSE;
	return TRUE;
}

void CDaumGameParameter::Empty( void )
{
	if( ::OpenClipboard( NULL ) == FALSE )
		return;
	::SetClipboardData( m_uDGPFormat, NULL );
	::CloseClipboard();

	m_bModified = FALSE;
	SAFE_DELETE( m_pMemoryBlock );
}

void CDaumGameParameter::Destroy( void )
{
	Flush();
	SAFE_DELETE( m_pMemoryBlock );
}

BOOL CDaumGameParameter::Set( LPCSTR lpszKeyName, LPSTR lpBuffer )
{
	if( m_pMemoryBlock == NULL )
		m_pMemoryBlock = new _CDaumGameMemoryBlock;
	m_pMemoryBlock->Put( lpszKeyName, lpBuffer );
	m_bModified = TRUE;
	return TRUE;
}

BOOL CDaumGameParameter::Set( LPSTR lpBuffer )
{
	if( m_pMemoryBlock == NULL )
		m_pMemoryBlock = new _CDaumGameMemoryBlock;
	m_pMemoryBlock->Put( lpBuffer );
	m_bModified = TRUE;
	return TRUE;
}

BOOL CDaumGameParameter::Get( LPCSTR lpszKeyName, LPSTR lpBuffer, DWORD nSize )
{
	if( m_pMemoryBlock == NULL ) {
		if( ::OpenClipboard( NULL ) == FALSE )
			return FALSE;

		HANDLE hClipboardMemory = ::GetClipboardData( m_uDGPFormat );

		m_pMemoryBlock = new _CDaumGameMemoryBlock;
		m_pMemoryBlock->Assign( hClipboardMemory );

		::CloseClipboard();
	}

	return m_pMemoryBlock->Get( lpszKeyName, lpBuffer, nSize );
}

_CDaumGameMemoryBlock::_CDaumGameMemoryBlock( void )
{
}

_CDaumGameMemoryBlock::~_CDaumGameMemoryBlock( void )
{
	for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ) {
		LPSTR	lpszText = *itr;
		SAFE_DELETE_ARRAY( lpszText );
		itr = m_StrList.erase( itr );
	}
}

BOOL _CDaumGameMemoryBlock::Assign( HANDLE hBlock )
{
	if( hBlock == NULL || hBlock == INVALID_HANDLE_VALUE )
		return FALSE;
	LPBYTE lpBuffer = (LPBYTE) ::GlobalLock( hBlock );

	if( lpBuffer == NULL )
		return FALSE;
	DWORD dwCount = *((LPDWORD) lpBuffer);				lpBuffer += sizeof( DWORD );
	for( DWORD dwI=0; dwI < dwCount; dwI++ ) {
		WORD	wLen = *((LPWORD) lpBuffer);			lpBuffer += sizeof( WORD );

		LPSTR	lpszText = new CHAR[ wLen + 1 ];
		::CopyMemory( lpszText, lpBuffer, wLen );
		lpszText[ wLen ] = '\0';

		lpBuffer += wLen;

		Put( lpszText );
	}
	::GlobalUnlock( hBlock ); 
	return TRUE;
}

HANDLE _CDaumGameMemoryBlock::CreateHandle( void )
{
	if( m_StrList.size() == 0 )
		return INVALID_HANDLE_VALUE;
	DWORD dwBlockSize = sizeof( DWORD );
	// Calculation for Memory Block Size
	for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
		dwBlockSize += sizeof( WORD );
		dwBlockSize += (DWORD) strlen( *itr );
	}

	HANDLE hBlock = ::GlobalAlloc( GMEM_ZEROINIT | GMEM_MOVEABLE, dwBlockSize );
	if( hBlock == NULL )
		return INVALID_HANDLE_VALUE;
	LPBYTE lpBuffer = (LPBYTE) ::GlobalLock( hBlock );
	if( lpBuffer == NULL ) {
		::GlobalFree( hBlock );
		return INVALID_HANDLE_VALUE;
	}
	*((LPDWORD) lpBuffer) = (DWORD) m_StrList.size();	lpBuffer += sizeof( DWORD );
	for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
		*((LPWORD) lpBuffer) = (WORD) strlen( *itr );	lpBuffer += sizeof( WORD );
		::CopyMemory( lpBuffer, *itr, strlen( *itr ) );	lpBuffer += strlen( *itr );
	}
	::GlobalUnlock( hBlock );
	return hBlock;
}

LPSTR _CDaumGameMemoryBlock::Find( LPCSTR lpszKeyName )
{
	for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
		LPSTR	lpszText = *itr;

		if( _strnicmp( lpszText, lpszKeyName, strlen( lpszKeyName ) ) != 0 )
			continue;
		if( *(lpszText + strlen( lpszKeyName )) != '=' )
			continue;
		return lpszText;
	}
	return NULL;
}

BOOL _CDaumGameMemoryBlock::Put( LPCSTR lpszKeyName, LPSTR lpBuffer )
{
	LPSTR	lpszText;

	if( (lpszText = Find( lpszKeyName )) != NULL ) {
		for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
			if( lpszText == *itr ) {
				SAFE_DELETE_ARRAY( lpszText );
				m_StrList.erase( itr );
				break;
			}
		}
	}
	size_t nStrLen	= strlen( lpszKeyName ) + 1 /* '=' */ + strlen( lpBuffer );
	lpszText = new CHAR[ nStrLen + 1 ];
	::CopyMemory( lpszText, lpszKeyName, strlen( lpszKeyName ) );
	*(lpszText + strlen( lpszKeyName )) = '=';
	::CopyMemory( lpszText + strlen( lpszKeyName ) + 1, lpBuffer, strlen( lpBuffer ) );
	*(lpszText + nStrLen) = '\0';

	m_StrList.push_back( lpszText );
	return TRUE;
}

BOOL _CDaumGameMemoryBlock::Put( LPSTR lpBuffer )
{
	LPSTR	lpszText;

	size_t nStrLen	= strlen( lpBuffer );
	lpszText = new CHAR[ nStrLen + 1 ];
	::CopyMemory( lpszText, lpBuffer, strlen( lpBuffer ) );
	*(lpszText + nStrLen) = '\0';

	m_StrList.push_back( lpszText );
	return TRUE;
}

BOOL _CDaumGameMemoryBlock::Get( LPCSTR lpszKeyName, LPSTR lpBuffer, DWORD nSize )
{
	LPSTR lpszText = Find( lpszKeyName );
	if( lpszText == NULL )
		return FALSE;
	lpszText += (strlen( lpszKeyName ) + 1);
	::SecureZeroMemory( lpBuffer, nSize );
	strncpy_s( lpBuffer, nSize, lpszText, (nSize < strlen(lpszText)) ? nSize : strlen(lpszText) );
	return TRUE;
}
