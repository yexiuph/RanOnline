// SerialFile.cpp: implementation of the CSerialFile class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "./SerialFile.h"
#include "./CompByte.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <fcntl.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialFile::CSerialFile() :
	m_FileStream(NULL)
{
}

CSerialFile::CSerialFile ( FOPENTYPE _Type, const char *_szFileName, bool bencode ) :
	m_FileStream(NULL)
{
	GASSERT(_szFileName);

	OpenFile ( _Type, _szFileName, bencode );
}

CSerialFile::~CSerialFile()
{
	CloseFile ();
}

void CSerialFile::SetFileType( const char *szFileType, DWORD FileID )
{
	StringCchCopy( m_szFileType, FILETYPESIZE, szFileType );
	m_FileID = FileID;
}

void CSerialFile::GetFileType( char *szFileType, int nStrLen, DWORD &FileID )
{
	StringCchCopy( szFileType, nStrLen, m_szFileType );
	FileID = m_FileID;
}

void CSerialFile::ReadFileType ()
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)fread( &m_szFileType, sizeof( char ), FILETYPESIZE, m_FileStream );
	Num = (int)fread( &m_FileID, sizeof( DWORD ), 1, m_FileStream );

	m_DefaultOffSet = ftell ( m_FileStream );
}

void CSerialFile::WriteFileType ()
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)fwrite( &m_szFileType, sizeof( char ), FILETYPESIZE, m_FileStream );
	Num = (int)fwrite( &m_FileID, sizeof( DWORD ), 1, m_FileStream );

	m_DefaultOffSet = ftell ( m_FileStream );
}

BOOL CSerialFile::NewFile ( const char* pName )
{
	int fh;

	_sopen_s(&fh, pName, _O_CREAT | _O_TRUNC, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if( fh == -1 )	return FALSE;	//perror( "Couldn't create data file" );
	else			
	{
		//printf( "Created data file.\n" );
		_close( fh );
		return TRUE;
	}
}

BOOL CSerialFile::OpenFile ( const FOPENTYPE _Type, const char *_szFileName, bool bencode )
{
	GASSERT(_szFileName);

	m_OpenType = _Type;
	m_bencode = bencode;
	StringCchCopy( m_szFileName, _MAX_FNAME, _szFileName );

	// 이미 열려 있을때 파일 닫음.
	CloseFile ();

	switch ( m_OpenType )
	{
	case FOT_READ:
		fopen_s(&m_FileStream, m_szFileName, "rb");
		if ( !m_FileStream ) break;

		ReadFileType ();
		break;

	case FOT_WRITE:
		fopen_s(&m_FileStream, m_szFileName, "wb");
		if ( !m_FileStream )
		{
			if( NewFile( _szFileName ) )	// New File
			{
				fopen_s(&m_FileStream, m_szFileName, "wb");
				if ( !m_FileStream )	break;
			}
			else	break;					// New File Not Create
		}

		WriteFileType ();
		break;
	};

	if ( m_FileStream )
	{
		m_IsFileOpened = TRUE;
		return TRUE;
	}

	int nErrNo = errno;
	return FALSE;
}

void CSerialFile::CloseFile ()
{
	if ( m_FileStream )		fclose( m_FileStream );

	m_FileStream	= NULL;
	m_IsFileOpened	= FALSE;
	m_DefaultOffSet = 0;
}

// Note : SerialFile의 기본 파일 HEADER영역을 제외한 곳 부터.
//		0 - Position.
//
BOOL CSerialFile::SetOffSet ( long _OffSet )
{
	if ( fseek ( m_FileStream, _OffSet + m_DefaultOffSet, SEEK_SET ) ) return FALSE;

	return TRUE;
}

//	Note : 파일의 m_DefaultOffSet를 제외한 현제 위치를 가져온다.
//
long CSerialFile::GetfTell ()
{
	return ftell ( m_FileStream ) - m_DefaultOffSet;
}

BOOL CSerialFile::BeginBlock ( EMSFILE_BLOCK _emBLOCK )
{
//	if ( m_dwBlockMarkPOS >= EMBLOCK_SIZE )	return FALSE;

	GASSERT(m_nMarkPos[m_dwBlockMarkPOS]==0&&"CSerialFile::BeginBlock");

	m_nMarkPos[m_dwBlockMarkPOS] = GetfTell();	//	현재 위치 저장.
	*this << DWORD(-1);					//	MARK 저장.

	++m_dwBlockMarkPOS;

	return TRUE;
}

BOOL CSerialFile::EndBlock ( EMSFILE_BLOCK _emBLOCK )
{
//	if ( m_dwBlockMarkPOS > EMBLOCK_SIZE )	return FALSE;

	--m_dwBlockMarkPOS;

	GASSERT(m_nMarkPos[m_dwBlockMarkPOS]!=0&&"CSerialFile::EndBlock");

	long nEndPos = GetfTell();
	DWORD dwTotalSize = DWORD ( ( nEndPos - m_nMarkPos[m_dwBlockMarkPOS] ) - sizeof(DWORD) );

	SetOffSet ( m_nMarkPos[m_dwBlockMarkPOS] );
	*this << dwTotalSize;

	SetOffSet ( nEndPos );

	m_nMarkPos[m_dwBlockMarkPOS] = 0;

	return TRUE;
}

DWORD CSerialFile::ReadBlockSize ()
{
	DWORD dwSize;
	*this >> dwSize;
	return dwSize;
}

inline size_t CSerialFile::read ( void* pbuffer, DWORD dwSize )
{
	GASSERT(m_FileStream&&"CSerialFile::read()");
	GASSERT(pbuffer&&"CSerialFile::read()");
	
	size_t rdcount;

	if ( m_bencode )
	{
		rdcount = fread ( pbuffer, dwSize, 1, m_FileStream );
		compbyte::decode ( (BYTE*)pbuffer, dwSize );
	}
	else
	{
		rdcount = fread ( pbuffer, dwSize, 1, m_FileStream );
	}

	GASSERT(rdcount==1&&"CSerialFile::read()");

	if ( rdcount!=1 )
	{
		CDebugSet::ToLogFile ( "file read error : %s", m_szFileName );
	}

	return rdcount;
}

inline size_t CSerialFile::write ( const void* pbuffer, DWORD dwSize )
{
	GASSERT(m_FileStream&&"CSerialFile::write()");
	GASSERT(pbuffer&&"CSerialFile::write()");

	size_t wtcount(0);
	if ( m_bencode )
	{

		BYTE *ptemp = new BYTE[dwSize];
		memcpy ( ptemp, pbuffer, sizeof(BYTE)*dwSize );
		compbyte::encode ( (BYTE*)ptemp, dwSize );


		wtcount = fwrite ( ptemp, dwSize, 1, m_FileStream );
		GASSERT(wtcount==1&&"CSerialFile::write()");

		if ( wtcount!=1 )
		{
			CDebugSet::ToLogFile ( "file write error : %s", m_szFileName );
		}

		delete[] ptemp;
	}
	else
	{
		wtcount = fwrite ( pbuffer, dwSize, 1, m_FileStream );
		GASSERT(wtcount==1&&"CSerialFile::write()");
	}

	return wtcount;
}

BOOL CSerialFile::WriteBuffer ( const void *pBuffer, DWORD Size )
{
	GASSERT(m_OpenType==FOT_WRITE);
	GASSERT(pBuffer);

	int Num = 0;

	Num = (int)write( pBuffer, Size );

	return TRUE;
}

BOOL CSerialFile::ReadBuffer ( void* pBuffer, DWORD Size )
{
	GASSERT(m_OpenType==FOT_READ);
	GASSERT(pBuffer);

	int Num = 0;

	Num = (int)read ( pBuffer, Size );

	return TRUE;
}

BOOL CSerialFile::operator << ( short Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( int Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( WORD Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( DWORD Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( UINT Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( float Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( double Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( D3DVECTOR Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( D3DCOLORVALUE &Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( D3DBLEND &Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( BYTE Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( char Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( bool Value )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	Num = (int)write ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator << ( const std::string &str )
{
	GASSERT(m_OpenType==FOT_WRITE);
	int Num = 0;

	*this << DWORD(str.length()+1);
	
	Num = (int)write ( str.c_str(), sizeof(char)*(DWORD(str.length())+1) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( short &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( int &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( WORD &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( DWORD &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( UINT &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( float &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( double &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( D3DVECTOR &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( D3DCOLORVALUE &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( D3DBLEND &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( BYTE &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( char &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( bool &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Value = FALSE;
	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL CSerialFile::operator >> ( std::string &str )
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

// X64 Architecture Support : Operator Update - YeXiuPH
#ifdef _M_X64
BOOL CSerialFile::operator >> (uint64_t &Value)
{
	GASSERT(m_OpenType == FOT_READ);
	Value = 0;
	int Num = (int)read(&Value, sizeof(Value));
	return TRUE;
}

BOOL CSerialFile::operator << (uint64_t Value)
{
	GASSERT(m_OpenType == FOT_WRITE);
	int Num = (int)write(&Value, sizeof(Value));
	return TRUE;
}
#endif