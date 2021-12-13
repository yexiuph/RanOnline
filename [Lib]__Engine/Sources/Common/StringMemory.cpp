// StringFile.cpp: implementation of the CStringFile class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "./CompByte.h"
#include "./StringMemory.h"
#include "./Unzipper.h"
#include "./SFileSystem.h"
#include "./GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CStringMemory::CStringMemory ( UINT nBufSize ) 
	: m_dwOffSet(0)
	, m_nBufSize(0)
	, m_pBufferMem(NULL)
	, m_nBufSizeTemp(0)
{
	m_dwRead = nBufSize;
	m_nBufferSize = nBufSize;
	m_pBuffer = new BYTE[nBufSize];
}

CStringMemory::~CStringMemory ()
{
	delete[] m_pBuffer;
	Close ();
}

BOOL CStringMemory::Open (	LPCSTR _szZipFileName, 
							LPCSTR _szFileName, 
							BOOL bDecode, 
							BOOL bPack )
{
	GASSERT(m_pBufferMem==NULL&&"CStringMemory::Open ()");

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
				m_pBufferMem = new BYTE[pFileContext->lUncompressSize];

				do
				{
					dwRead = pFileHandle->Read( buffer, 4096 );

					memcpy( m_pBufferMem+m_nBufSize, buffer, dwRead );
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
		m_nBufSize = cUnzip.UnzipToMemory( _szZipFileName, _szFileName, m_pBufferMem );
	}
	
	// note :	파일 사이즈가 너무 커도 UINT_MAX가 넘어 옴. 약 17메가.
	//			파일을 1이라도 읽었다는걸 확신할수 있나?
	if( m_nBufSize == UINT_MAX || m_nBufSize == 0)
		return FALSE;

	m_dwOffSet = 0;
	m_nBufSizeTemp = m_nBufSize;
	m_bDecode = bDecode;
	StringCchCopy( m_szFileName, MAX_PATH, _szFileName );

	if( m_bDecode )
	{
		int nVersion;
		memcpy( &nVersion, m_pBufferMem, sizeof(int) );
		if( nVersion > 0 && CRijndael::VERSION >= nVersion )
//		if( CRijndael::VERSION == nVersion )
		{
			m_nVersion = nVersion;
			int nDateVersion = nVersion >= CRijndael::VERSION ? CRijndael::VERSION_DATE : 0;
			m_oRijndael.Initialize( CRijndael::sm_Version[nVersion-1].c_str(), CRijndael::sm_KeyLength[nVersion-1], nDateVersion, nVersion,
									CRijndael::sm_chain0, CRijndael::sm_KeyLength[nVersion-1] );

			m_nBufSize -= sizeof(int);
			m_nBufSizeTemp = m_nBufSize;
			BYTE * pTemp = m_pBufferMem + sizeof(int);
			memcpy( m_pBufferMem, pTemp, m_nBufSize );

			m_oRijndael.DecryptEx( (char*)m_pBufferMem, m_nBufSize );
		}else{
			return FALSE;
		}
	}

	return TRUE;
}

void CStringMemory::Close ()
{
	if( m_pBufferMem )
	{
		delete [] m_pBufferMem;
		m_pBufferMem = NULL;
	}

	m_dwIndex = 0;
	m_dwLine = 0;

	m_dwOffSet = 0;
	m_nBufSize = 0;
	m_nBufSizeTemp = 0;
}

DWORD CStringMemory::GetNextLine ( CString &strString )
{
	if( m_pBufferMem == NULL ) return 0;

	char *szString = strString.GetBuffer ( m_nBufferSize );
	DWORD dwReturn = this->GetNextLine ( szString, m_nBufferSize );
	if ( dwReturn == 0 )	strString.Empty();

	strString.ReleaseBuffer ();

	return dwReturn;
}

DWORD CStringMemory::GetNextLine ( PSTR &szString )
{
	if( m_pBufferMem == NULL ) return 0;

	SAFE_DELETE(szString);

	szString = new CHAR[m_nBufferSize];
	DWORD dwReturn = this->GetNextLine ( szString, m_nBufferSize );

	SAFE_DELETE(szString);
	
	return dwReturn;
}

DWORD CStringMemory::GetNextLine ( LPSTR szLine, UINT iLineSize )
{
	if( m_pBufferMem == NULL ) return 0;

	BYTE	*chTemp;
	BOOL	bStop=FALSE;
	int		nOut;

	chTemp = (LPBYTE) szLine;
	*chTemp = 0;
	nOut = 0;

	while ( !bStop )
	{
		if ( !m_dwLine || m_dwIndex==m_dwRead )
		{
			if ( m_dwOffSet >= m_nBufSize ) m_dwRead = 0;
			else
			{
				if( m_nBufSizeTemp < m_nBufferSize )
				{
					memcpy( m_pBuffer, m_pBufferMem + m_dwOffSet, sizeof(BYTE)*m_nBufSizeTemp );
					m_dwRead = m_nBufSizeTemp;
					m_dwOffSet += sizeof(BYTE)*m_nBufSizeTemp;
				}
				else
				{
					memcpy( m_pBuffer, m_pBufferMem + m_dwOffSet, sizeof(BYTE)*m_nBufferSize );
					m_nBufSizeTemp -= m_nBufferSize;
					m_dwRead = m_nBufferSize;
					m_dwOffSet += sizeof(BYTE)*m_nBufferSize;
				}

				if ( m_bDecode )	
				{
					if( m_nVersion == -1 )
					{
						compbyte::decode ( m_pBuffer, sizeof(BYTE)*m_dwRead );
					}
				}
			}

			m_dwIndex = 0;
			if ( m_dwRead == 0 )
			{
				bStop = TRUE; // Error during readfile or END-OF-FILE encountered

				if ( nOut>0 )
				{
					chTemp[nOut++] = (char) 0;
					return m_dwLine;	
				}
				else return m_dwLine = 0; //nix gelezen
			}
			else
			{
				if ( m_dwRead != (DWORD)m_nBufferSize )
					bStop = TRUE;	//END-OF-FILE
			}
		}

		for ( ; m_dwIndex<m_dwRead; m_dwIndex++ )
		{
			if ( (nOut+1) == iLineSize )
			{
				TCHAR szError[260] = {0};
				StringCchPrintf( szError, 260, "CStringMemory::m_pBuffer Not enough. File:%s, Line:%u(Over Char Number:%d)",
								m_szFileName, ++m_dwLine, iLineSize );
				::MessageBox ( NULL, szError, "Warning !", MB_ICONEXCLAMATION|MB_OK );
				chTemp[nOut] = '\0';
				return m_dwLine;
			}

			switch ( m_pBuffer[m_dwIndex] )
			{
			case 0x0d://End of Line encountered
			case 0x0a:
				if ( (m_dwIndex+1) < m_dwRead ) // Check we're not on end of m_pBuffer ???
				{
					if ( m_pBuffer[m_dwIndex+1] == '\n' || m_pBuffer[m_dwIndex+1] == '\r' )
					{
						if ( !*chTemp )	m_dwLine++;
						m_dwIndex++;
					}
				}

				if ( *chTemp )
				{
					chTemp[nOut++] = '\0';
					m_dwLine++;

					return m_dwLine;
				}

				break;

			default:
				chTemp[nOut++] = m_pBuffer[m_dwIndex];
				break;
			};
		}
	}

	if ( nOut > 0 )
	{
		chTemp[nOut++] = '\0';
		
		if ( m_dwLine==0 )	m_dwLine++;

		return m_dwLine;
	}

	return m_dwLine = 0; //nix gelezen
}

void CStringMemory::Reset ()
{
	if( m_pBufferMem == NULL ) return;

	m_dwIndex = 0;
	m_dwLine = 0;

	m_dwOffSet = 0;
	m_nBufSizeTemp = m_nBufSize;
}
