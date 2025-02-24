#include "pch.h"
#include "./IniLoader.h"
#include "./BaseString.h"
#include "./gassert.h"
#include "./CharSet.h"
#include "./GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIniLoader::CIniLoader( STRING sep )
	: m_fieldsep(sep)
	, m_bZipFile(FALSE)
	, m_bUseErrorMsgBox(TRUE)
{
}

CIniLoader::~CIniLoader()
{
	for( MAPSECTION_ITER s_pos = m_mapSection.begin(); s_pos != m_mapSection.end(); ++s_pos )
	{
		MAPKEY * pMapKey(NULL);
		pMapKey = s_pos->second;
		if( !pMapKey ) continue;

		for( MAPKEY_ITER k_pos = pMapKey->begin(); k_pos != pMapKey->end(); ++k_pos )
		{
			delete k_pos->second;
			k_pos->second = NULL;
		}

		pMapKey->clear();
		delete pMapKey;
		pMapKey = NULL;
	}

	m_mapSection.clear();
}

void CIniLoader::ErrorMegBox( const TCHAR * section, const TCHAR * szText )
{
	if( !m_bUseErrorMsgBox ) return;
	if( !section )	return;
	if( !szText )	return;

	TCHAR szErrorMsg[255] = {0};

	StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[FILE:%s, SECTION:%s]\n%s",
					m_strFileName.c_str(), section, szText );

	MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );
}

void CIniLoader::ErrorMegBox( const TCHAR * section, const TCHAR * key, const TCHAR * szText )
{
	if( !m_bUseErrorMsgBox ) return;
	if( !section )	return;
	if( !szText )	return;

	TCHAR szErrorMsg[255] = {0};

	StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[FILE:%s, SECTION:%s, KEY:%s]\n%s",
					m_strFileName.c_str(), section, key, szText );

	MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );
}

void CIniLoader::ErrorMegBox( const TCHAR * section, const TCHAR * key, const DWORD dwLine, const TCHAR * szText )
{
	if( !m_bUseErrorMsgBox ) return;
	if( !section )	return;
	if( !szText )	return;

	TCHAR szErrorMsg[255] = {0};

	StringCchPrintf( szErrorMsg, sizeof(szErrorMsg), "[FILE:%s, SECTION:%s, KEY:%s, LINE:%d]\n%s",
					m_strFileName.c_str(), section, key, dwLine, szText );

	MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );
}

CIniLoader::MAPKEY* CIniLoader::FindSection( const STRING section )
{
	MAPSECTION_ITER iter_sec = m_mapSection.find( section );
	if( iter_sec != m_mapSection.end() )
	{
		return iter_sec->second;
	}

	ErrorMegBox( section.c_str(), "A Section which does not exist." );
	return NULL;
}

CIniLoader::VECSTRING* CIniLoader::FindKey( const STRING section, const std::string key )
{
	MAPSECTION_ITER iter_sec = m_mapSection.find( section );
	if( iter_sec != m_mapSection.end() )
	{
		MAPKEY * pMapKey = iter_sec->second;

		MAPKEY_ITER iter_key = pMapKey->find( key );
		if( iter_key != pMapKey->end() )
		{
			return iter_key->second;
		}
		else
		{
			// ErrorMegBox( section.c_str(), key.c_str(), "A Key which does not exist." );
			return NULL;
		}
	}

	ErrorMegBox( section.c_str(), "A Section which does not exist." );
	return NULL;
}

CIniLoader::VECSTRING* CIniLoader::validflag( const STRING section, const STRING key, const int nIDX, const int nSIZE )
{
	VECSTRING * pVecStr = FindKey( section, key );
	if( !pVecStr ) return NULL;
	
	if( nSIZE != pVecStr->size() )
	{
			ErrorMegBox( section.c_str(), key.c_str(), "It was wrong size of Input Data." );
			return NULL;
	}
	
	return pVecStr;
}

CIniLoader::VECSTRING* CIniLoader::validflag( const DWORD dwITER, STRING section, const STRING key, const int nIDX, const int nSIZE )
{
	MAPKEY * pMapKey = FindSection( section );
	if( !pMapKey ) return NULL;

	MAPKEY_PAIR pair = pMapKey->equal_range( key );
	if( pair.first == pair.second )
	{
		// ErrorMegBox( section.c_str(), key.c_str(), "A Key which does not exist." );
		return NULL;
	}

	MAPKEY_ITER iter = pair.first;
	for( DWORD i=0; i<dwITER; ++i )
		++iter;

	VECSTRING * pFOUND_VEC = (*iter).second;

	DWORD dwPARAMNUM = (DWORD)pFOUND_VEC->size();
	if( dwPARAMNUM != nSIZE )
	{
		ErrorMegBox( section.c_str(), key.c_str(), dwITER+1, "It was wrong size of Input Data." );
		return NULL;
	}

	return pFOUND_VEC;
}

bool CIniLoader::IsNotes( const CString & line )
{
	int nLen = line.GetLength();

	for( int i = 0; i < nLen; ++i )
	{
		if( line[i] == _T(' ') || line[i] == _T('\t') ) continue;
		if( line[i] == _T(';') )
			return true;
		else
			return false;
	}

	return false;
}

int CIniLoader::GetKeyData( const CString & line, STRING & key, VECSTRING * pVec )
{
	CString strTemp( line );
	CString resToken;
	int curPos = 0;

	resToken = strTemp.Tokenize( "=", curPos );
	if( resToken == "" ) // Notes : 키값만 존재한다.
	{
		strTemp.Trim( _T(" \t") );
		key = strTemp;

		return 0;
	}

	int nTokenLen = resToken.GetLength();
	CString strData( strTemp.Right( strTemp.GetLength() - nTokenLen - 1) );
	strData.Trim( _T(" \t") );

	resToken.Trim( _T(" \t") );
	key = resToken;

	int nRet = split( STRING(strData), pVec );

	return nRet;
}

bool CIniLoader::IsSection( const CString & line, STRING & sec )
{
	int nStart(0);
	int nLen = line.GetLength();

	for( int i = 0; i < nLen; ++i )
	{
		if( line[i] == ' ' || line[i] == '\t' ) continue;
		if( line[i] != '[' ) return false;
		else
		{
			nStart = i;
			break;
		}
	}

	int nEnd = nLen - 1;

	for( int i = nEnd; i > 0; --i )
	{
		if( line[i] == _T(' ') || line[i] == '\t' ) continue;
		if( line[i] != ']' ) return false;
		else
		{
			nEnd = i;
			break;
		}
	}

	if( nStart > nEnd ) return false;

	sec = line.Mid( ++nStart, --nEnd );

	return true;
}

bool CIniLoader::open( const STRING strFile, bool bDEC, BOOL bPack )
{
	m_strFileName = strFile;

	char fname[_MAX_FNAME]={0};
	char ext[_MAX_EXT]={0};
	_splitpath_s( m_strFileName.c_str(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT );
	StringCchCat( fname, _MAX_FNAME, ext );

	// C++ 20 Standards Update : Auto PTR -> Unqiue Ptr - YeXiuPH
	std::unique_ptr<CBaseString> pBString( GLOGIC::openfile_basestring(	m_bZipFile, 
																		m_strZipFileName.c_str(), 
                                                                        m_strFileName.c_str(), 
																		fname, 
																		bDEC,
																		bPack ) );

	if( !pBString.get() )
	{
		CDebugSet::ToLogFile ( "ERROR : CIniLoader::open(), File Open %s", fname );
		return false;
	}

	CBaseString &cBString = *pBString;

	m_bZipFile = FALSE;

	MAPKEY * pMapKey(NULL);
	STRING sectionback;

	CString strLine;
	while( cBString.GetNextLine(strLine) )
	{
		if( !IsNotes( strLine ) ) // Notes : 주석처리
		{
			if( pMapKey == NULL )
				pMapKey = new MAPKEY;

			STRING section;

			if( !IsSection( strLine, section ) ) // Notes : 키와 데이터 처리, 처음에 무조건 섹션이 나와야 한다.
			{
				VECSTRING * pVecStr = new VECSTRING;

				// Notes : 한 라인을 키와 데이터로 구분한다. 키는 스트링, 값은 스티링 벡터에 저장한다.
				STRING key;
				GetKeyData( strLine, key, pVecStr ); 

				//std::pair< MAPKEY_ITER, bool > pr;
				//pr = pMapKey->insert( std::make_pair( key, pVecStr ) );

				pMapKey->insert( std::make_pair( key, pVecStr ) );

				//if( pr.second != true )   
				//{
				//	ErrorMegBox( section.c_str(), key.c_str(), "The Key already exists." );
				//}
			}
			else
			{
				// Note : 섹션처리
				STRING strTemp(section);

				if( sectionback.empty() )
				{
					sectionback = section;
				}

				if( !sectionback.empty() && !pMapKey->empty() )
				{
					std::pair< MAPSECTION_ITER, bool > pr;
					pr = m_mapSection.insert( std::make_pair( sectionback, pMapKey ) );
					if( pr.second != true )   
					{
						ErrorMegBox( sectionback.c_str(), "The Section already exists." );
					}

					sectionback = strTemp;
					pMapKey = NULL;
				}
			}
		}
	}

	if( !sectionback.empty() && !pMapKey->empty() )
	{
		std::pair< MAPSECTION_ITER, bool > pr;
		pr = m_mapSection.insert( std::make_pair( sectionback, pMapKey ) );
		if( pr.second != true )   
		{
			ErrorMegBox( sectionback.c_str(), "The Section already exists." );
		}
	}

	return true;
}

int CIniLoader::split( const STRING & line, VECSTRING * pVec )
{
	STRING fld;
	UINT i(0),j(0);
	UINT nfield(0);

	if( (UINT)line.length() == 0 )
		return 0;

	do
	{
		if( i < (UINT)line.length() && line[i] == _T('"') )
			j = advquoted( line, fld, ++i); // Notes : 인용부호 처리 생략
		else
			j = advplain( line, fld, i );

		if( j > i )
		{
			if( nfield >= (UINT)pVec->size() )
				pVec->push_back(fld);
			else
				(*pVec)[nfield] = fld;

			nfield++;
		}

		i = j + 1;
	} while( j < (UINT)line.length() );

	return nfield;
}

// Notes : 인용부호로 둘러 쌓인 필드
int CIniLoader::advquoted(const STRING & s, STRING & fld, UINT i)
{
	UINT j(0);
	fld = _T("");

	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	for( j = i; j < (unsigned int)s.length(); j++ )
	{
		if( s[j] == _T('"') && s[++j] != _T('"') )
		{
			indexCh = findfirstof( s, m_fieldsep, j );

			if( indexCh == npos ) // Notes : 구분자가 없는 경우
				indexCh = s.length();

			for( indexCh -= j; indexCh-- > 0; )
			{
				fld += s[j++];
			}

			break;
		}

		fld += s[j];
	}

	return j;
}

// Notes : 인용부호로 둘러싸이지 않은 필드
int CIniLoader::advplain(const STRING & s, STRING & fld, UINT i ) 
{
	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	indexCh = findfirstof( s, m_fieldsep, i );

	if( indexCh == npos ) // Notes : 구분자가 없는 경우
		indexCh = s.length();

	fld = STRING( s, i, indexCh-i);

	return (int)indexCh;
}

UINT CIniLoader::GetKeyDataSize( const STRING section, const STRING key )
{
	VECSTRING * pVecStr = FindKey( section, key );
	if( !pVecStr ) return 0;

	return (UINT)pVecStr->size();
}

DWORD CIniLoader::GetKeySize( const STRING section, const STRING key )
{
	MAPKEY * pMapKey = FindSection( section );
	if( !pMapKey ) return 0;

	MAPKEY_PAIR pair = pMapKey->equal_range( key );
	if( pair.first == pair.second )
	{
		// ErrorMegBox( section.c_str(), key.c_str(), "A Key which does not exist." );
		return 0;
	}

	return (DWORD)std::distance( pair.first, pair.second );
}

int CIniLoader::findfirstof( const STRING & line, const STRING & sep, UINT i )
{
	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	for( int index=i; index<(int)line.length(); )
	{
		if( CHARSET::IsLeadChar( line[index] ) )
		{
			index += 2;
			continue;
		}

		indexCh = sep.find( line[index] );
		if( indexCh != npos )
			return index;

		++index;
	}

	return npos;
}