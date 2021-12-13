#include "stdafx.h"
#include "DaumGameCrypt.h"
#ifdef _WIN32
#include <comutil.h>

#pragma comment(lib, "comsupp.lib")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDaumGameCrypt::CDaumGameCrypt( void ) : m_bInit( FALSE )
{
	Init();
}

CDaumGameCrypt::~CDaumGameCrypt( void )
{
	Destroy();
}

BOOL CDaumGameCrypt::Init( void )
{
	if( m_bInit == TRUE )
		return FALSE;
#ifdef _WIN32
	CoInitialize( NULL );
	m_pEncrypt.CreateInstance( DAUM::CLSID_Encrypt, NULL, CLSCTX_INPROC_SERVER );
	m_pDecrypt.CreateInstance( DAUM::CLSID_Decrypt, NULL, CLSCTX_INPROC_SERVER );
#else
	m_pCryptor	= new ::daum_encrypt_type;
#endif
	m_bInit = TRUE;
	return TRUE;
}

void CDaumGameCrypt::Destroy( void )
{
	if( m_bInit == FALSE )
		return;
#ifdef _WIN32
	m_pEncrypt = NULL;
	m_pDecrypt = NULL;
	CoUninitialize();
#else
	if( m_pCryptor ) {
		delete m_pCryptor;
		m_pCryptor = NULL;
	}
#endif
	m_bInit = FALSE;
}

BOOL CDaumGameCrypt::SetKey( LPCTSTR szKey )
{
#ifdef _WIN32
	try {
		m_pEncrypt->Init( _bstr_t( szKey ) );
		m_pDecrypt->Init( _bstr_t( szKey ) );
	}
	catch( _com_error e ) {
		return FALSE;
	}
#else
	daum_encrypt_init( m_pCryptor, (LPTSTR) szKey );
#endif
	return TRUE;
}

BOOL CDaumGameCrypt::Encrypt( IN LPCTSTR szText, OUT LPTSTR szEncryptText, int nMaxSize )
{
#ifdef _WIN32
	try {
		m_pEncrypt->PutInput( _bstr_t( szText ) );
		_tcsncpy( szEncryptText, (LPCTSTR) _bstr_t( m_pEncrypt->GetOutput() ), nMaxSize );
	}
	catch( _com_error e ) {
		return FALSE;
	}
#else
	if( daum_encrypt( m_pCryptor, (LPTSTR) szText, _tcslen( szText ), szEncryptText, &nMaxSize ) != 0 )
			return FALSE;
#endif
	return TRUE;
}

BOOL CDaumGameCrypt::Decrypt( IN LPCTSTR szEncryptedText, OUT LPTSTR szText, int nMaxSize )
{
#ifdef _WIN32
	try {
		m_pDecrypt->PutInput( _bstr_t( szEncryptedText ) );
		_tcsncpy( szText, (LPCTSTR) _bstr_t( m_pDecrypt->GetOutput() ), nMaxSize );
	}
	catch( _com_error e ) {
		return FALSE;
	}
#else
	if( daum_decrypt( m_pCryptor, (LPTSTR) szEncryptedText, _tcslen( szEncryptedText ), szText, &nMaxSize ) != 0 )
			return FALSE;
#endif
	return TRUE;
}
