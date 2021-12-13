#include "pch.h"
#include "./SerialMemory.h"
#include "./CompByte.h"
#include "./Unzipper.h"
#include "./SFileSystem.h"
#include "./GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSerialMemory::CSerialMemory(void) 
	: m_pBuffer(NULL)
	, m_nBufSize(0)
	, m_dwOffSet(0)
{
}

CSerialMemory::CSerialMemory ( const char *_szZipFileName, const char *_szFileName, bool bencode ) 
	: m_pBuffer(NULL)
	, m_nBufSize(0)
	, m_dwOffSet(0)
{
	GASSERT(_szFileName);
	GASSERT(_szZipFileName);

	OpenFile ( _szZipFileName, _szFileName, bencode );
}

CSerialMemory::~CSerialMemory(void)
{
	CloseFile ();
}

void CSerialMemory::GetFileType ( char *szFileType, int nStrLen, DWORD &FileID )
{
	StringCchCopy( szFileType, nStrLen, m_szFileType );
	FileID = m_FileID;
}

void CSerialMemory::CloseFile ()
{
	if ( m_pBuffer )
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	} // if ( m_pBuffer )

	m_DefaultOffSet = 0;
	m_nBufSize = 0;
	m_dwOffSet = 0;
	m_IsFileOpened = FALSE;
	m_FileID = 0;
}

BOOL CSerialMemory::OpenFile (	const char *_szZipFileName, 
								const char *_szFileName, 
								bool bencode, 
								BOOL bPack )
{
	//GASSERT(_szFileName);
	//GASSERT(_szZipFileName);
	if( !_szFileName || !_szZipFileName ) return FALSE;

	// 이미 열려 있을때 파일 닫음.
	if( IsOpen() ) CloseFile ();

	if( bPack )
	{
		CString strPath;
		strPath = _szZipFileName;
		strPath += _szFileName;

		SFileHandle * pFileHandle = GLOGIC::sFileSystem.OpenFile( strPath );
		if( pFileHandle )
		{
			FILECONTEXT * pFileContext = pFileHandle->GetFileContext();
			if( pFileContext )
			{
				BYTE buffer[4096]={0};
				DWORD dwRead;

				m_nBufSize = 0;
				m_pBuffer = new BYTE[pFileContext->lUncompressSize];

				do
				{
					dwRead = pFileHandle->Read( buffer, 4096 );
					
					memcpy( m_pBuffer+m_nBufSize, buffer, dwRead );
					m_nBufSize += dwRead;
				}
				while(dwRead > 0);
			}

			GLOGIC::sFileSystem.CloseFile( pFileHandle );
		}
	}
	else
	{
		CUnzipper cUnzip;
		m_nBufSize = cUnzip.UnzipToMemory( _szZipFileName, _szFileName, m_pBuffer );
	}

	// note :	파일 사이즈가 너무 커도 UINT_MAX가 넘어 옴. 약 17메가.
	//			파일을 1이라도 읽었다는걸 확신할수 있나?
	if( m_nBufSize == UINT_MAX || m_nBufSize == 0)
		return FALSE;

	m_OpenType = FOT_READ;
	m_bencode = bencode;
	StringCchCopy ( m_szFileName, _MAX_FNAME, _szFileName );
	StringCchCopy ( m_szZipFileName, _MAX_PATH, _szZipFileName );

	//if( m_bencode )
	//{
	//	int nVersion;
	//	memcpy( &nVersion, m_pBuffer, sizeof(int) );
	//	if( nVersion > 0 && CRijndael::VERSION >= nVersion )
	//	{
	//		m_nVersion = nVersion;
	//		m_oRijndael.Initialize( CRijndael::sm_Version[nVersion-1], CRijndael::DEFAULT_KEY_LENGTH );

	//		m_nBufSize -= sizeof(int);
	//		memcpy( m_pBuffer, m_pBuffer+sizeof(int), m_nBufSize );
	//		
	//		m_oRijndael.DecryptEx( (char*)m_pBuffer, m_nBufSize );
	//	}
	//}

	ReadFileType ();

	m_IsFileOpened = TRUE;
	return TRUE;
}

void CSerialMemory::ReadFileType ()
{
	int Num = 0;

	Num = (int)read( &m_szFileType, sizeof( char )*FILETYPESIZE );
	Num = (int)read( &m_FileID, sizeof( DWORD ) );

	m_DefaultOffSet = m_dwOffSet;
}

DWORD CSerialMemory::ReadBlockSize ()
{
	DWORD dwSize;
	*this >> dwSize;
	return dwSize;
}

inline size_t CSerialMemory::read ( void* pbuffer, DWORD dwSize )
{
	GASSERT(m_nBufSize&&"CSerialMemory::read()");
	GASSERT(pbuffer&&"CSerialMemory::read()");
	
	if ( (m_dwOffSet+dwSize) > m_nBufSize )		return 0;

	memcpy( pbuffer, m_pBuffer + m_dwOffSet, dwSize );
	m_dwOffSet += dwSize;

	if ( m_bencode )
	{
		//if( m_nVersion == -1 )
		//{
			compbyte::decode ( (BYTE*)pbuffer, dwSize );
		//}
	}

	// 별 의미없는 값
	return 1;
}

BOOL CSerialMemory::ReadBuffer ( void* pBuffer, DWORD Size )
{
	GASSERT(pBuffer);

	int Num = 0;

	Num = (int)read ( pBuffer, Size );

	return TRUE;
}

// Note : SerialFile의 기본 파일 HEADER영역을 제외한 곳 부터.
//		0 - Position.
//
BOOL CSerialMemory::SetOffSet ( long _OffSet )
{
	m_dwOffSet = _OffSet + m_DefaultOffSet;
	if( m_dwOffSet < 0 )
		return FALSE;
	
	return TRUE;
}

//	Note : 파일의 m_DefaultOffSet를 제외한 현제 위치를 가져온다.
//
long CSerialMemory::GetfTell ()
{
	return m_dwOffSet - m_DefaultOffSet;
}

BOOL CSerialMemory::BeginBlock ( EMSFILE_BLOCK _emBLOCK )
{
	//if ( emBLOCK > EMBLOCK_SIZE )	return FALSE;

	GASSERT(m_nMarkPos[m_dwBlockMarkPOS]==0&&"CSerialMemory::BeginBlock");

	m_nMarkPos[m_dwBlockMarkPOS] = GetfTell();	//	현재 위치 저장.
	*this << DWORD(-1);					//	MARK 저장.

	++m_dwBlockMarkPOS;

	return TRUE;
}

BOOL CSerialMemory::EndBlock ( EMSFILE_BLOCK _emBLOCK )
{
	//if ( emBLOCK > EMBLOCK_SIZE )	return FALSE;

	--m_dwBlockMarkPOS;

	GASSERT(m_nMarkPos[m_dwBlockMarkPOS]!=0&&"CSerialMemory::EndBlock");

	long nEndPos = GetfTell();
	DWORD dwTotalSize = DWORD ( ( nEndPos - m_nMarkPos[m_dwBlockMarkPOS] ) - sizeof(DWORD) );

	SetOffSet ( m_nMarkPos[m_dwBlockMarkPOS] );
	*this << dwTotalSize;

	SetOffSet ( nEndPos );

	m_nMarkPos[m_dwBlockMarkPOS] = 0;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// 수정 안해도 되는 함수

BOOL CSerialMemory::operator >> ( short &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( int &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( WORD &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( DWORD &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( UINT &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( float &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( double &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( D3DVECTOR &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( D3DCOLORVALUE &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( D3DBLEND &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( BYTE &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( char &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( bool &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialMemory::operator >> ( std::string &str )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	DWORD dwSize;
	*this >> dwSize;
	
	char *szBuffer = new char[dwSize];
	Num = (int)read ( szBuffer, sizeof(char)*(dwSize) );

	str = szBuffer;
	delete[] szBuffer;

	return TRUE;
}