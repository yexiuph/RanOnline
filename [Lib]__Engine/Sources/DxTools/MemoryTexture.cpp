#include "pch.h"
#include "MemoryTexture.h"
#include "EncryptTextureDef.h"
#include "DebugSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMemoryTexture::CMemoryTexture()
{
	m_Buffer = NULL;
	m_nSize = 0;
}

CMemoryTexture::~CMemoryTexture()
{
	if (m_Buffer)	SAFE_DELETE_ARRAY( m_Buffer );
}

bool CMemoryTexture::LoadFileDec( const char* szFileName )
{
	// ���Ͽ���
	FILE* pFile = NULL;
	fopen_s(&pFile, szFileName, "rb");
	if ( !pFile )
	{
		CDebugSet::ToLogFile( "Texture File Open Error", szFileName );	
		return false;
	}

	if ( !DecryptTexture( pFile ) )
	{
		CDebugSet::ToLogFile( "Texture File Decrypt Error", szFileName );	
		return false;
	}

	fclose ( pFile );
	
	return true;
}

bool CMemoryTexture::SaveFileEnc( const char* szFileName )
{
	// ���Ͽ���
	FILE* pFile = NULL;
	fopen_s(&pFile, szFileName, "rb");
	if ( !pFile )
	{
		CDebugSet::ToLogFile( "Texture File Open Error", szFileName );	
		return false;
	}

	if ( !EncryptTexture( pFile, szFileName ) )
	{
		CDebugSet::ToLogFile( "Texture File Decrypt Error", szFileName );	
		return false;
	}

	fclose ( pFile );
	
	return true;
}

bool CMemoryTexture::DecryptTexture( FILE* pFile )
{
	fseek ( pFile, 0, SEEK_END );
	int nSize = ftell ( pFile );

	fseek ( pFile, 0, SEEK_SET );	
	
	// ����б�
	int nVersion = 0;
	int nReadSize = 0;
	int nFileType = 0;
	
	fread( &nVersion, sizeof(int), 1, pFile );
    fread( &nReadSize, sizeof(int), 1, pFile );
	fread( &nFileType, sizeof(int), 1, pFile );

	if ( nVersion != TEX_VERSION || nSize != nReadSize + TEX_HEADER_SIZE )
	{
		return false;
	}	
	
	m_nSize = nReadSize;
	
	m_Buffer = new BYTE[nReadSize];

	// �����б�
	fread( m_Buffer, sizeof(BYTE), nReadSize, pFile );
	
	// XOR��ȯ
	for ( int i = 0; i < nReadSize; ++i )
	{
		m_Buffer[i] += TEX_DIFF_DATA;
		m_Buffer[i] = m_Buffer[i] ^ TEX_XOR_DATA;
	}

	return true;
}

bool CMemoryTexture::EncryptTexture( FILE* pFile, const char* szFileName )
{
	fseek ( pFile, 0, SEEK_END );
	int nSize = ftell ( pFile );

	fseek ( pFile, 0, SEEK_SET );	
	
	// ����߰�	
	BYTE* pBuffer = new BYTE[TEX_HEADER_SIZE+nSize];

	int nVersion = TEX_VERSION;
	int nFileType = TEXTURE_DDS;

	CString strFileExt = szFileName;

	strFileExt = strFileExt.Right( strFileExt.GetLength() - strFileExt.ReverseFind( '.' ) );
	strFileExt.MakeLower();

	if ( !strcmp( strFileExt, g_szFileExt[TEXTURE_DDS] ) )	nFileType = TEXTURE_DDS;
	else if ( !strcmp( strFileExt, g_szFileExt[TEXTURE_TGA] ) ) nFileType = TEXTURE_TGA;
	
	memcpy( pBuffer, &nVersion, sizeof( int ) );
	memcpy( pBuffer+4 , &nSize, sizeof(int) );
	memcpy( pBuffer+8, &nFileType, sizeof(int) );	
	
	// ���̱�
	fread( pBuffer+TEX_HEADER_SIZE, sizeof(BYTE), nSize, pFile );
	fclose( pFile );

	DeleteFile( szFileName );

	// XOR��ȯ
	for ( int i = 0; i < nSize; ++i )
	{
		pBuffer[TEX_HEADER_SIZE+i] = pBuffer[TEX_HEADER_SIZE+i] ^ TEX_XOR_DATA;
		pBuffer[TEX_HEADER_SIZE+i] -= TEX_DIFF_DATA;
	}

	CString strOutFile = szFileName;
	strOutFile = strOutFile.Left( strOutFile.ReverseFind('.'));
	strOutFile += ".mtf";

	// ��������
	FILE* pFileOutput = NULL;
	fopen_s(&pFileOutput, strOutFile, "wb");
	if ( !pFileOutput )
	{
		delete[] pBuffer;
		return FALSE;
	}
	
	fwrite( pBuffer, sizeof(BYTE), nSize+TEX_HEADER_SIZE, pFileOutput );
	fclose( pFileOutput );

	delete[] pBuffer;
	return TRUE;
}