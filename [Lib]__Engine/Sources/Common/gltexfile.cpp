#include "pch.h"
#include "./GLTexFile.h"
#include "./BaseString.h"
#include "./gassert.h"
#include "./StlFunctions.h"
#include "./GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

gltexfile::gltexfile(void) :
	m_pFOUND_VEC(NULL),
	m_bZipFile(FALSE)
{
}

gltexfile::~gltexfile(void)
{
	clean ();
}

void gltexfile::reg_sep ( char cVAL )
{
	m_cSeperator.Register ( cVAL );
}

bool gltexfile::open ( const std::string strFile, bool bDEC, BOOL bPack )
{
	m_strFileName = strFile;
	m_pFOUND_VEC = NULL;

	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s( m_strFileName.c_str(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT );
	StringCchCat( fname, _MAX_FNAME, ext );

	// C++ 20 Standards Update : Auto PTR -> Unqiue Ptr - YeXiuPH
	std::unique_ptr<CBaseString> pBString( GLOGIC::openfile_basestring(	m_bZipFile, 
																		m_strZipFileName.c_str(),
																		m_strFileName.c_str(), 
																		fname, 
																		bDEC, 
																		bPack ) );

	if ( !pBString.get() )
	{
		//CDebugSet::ToLogFile ( "ERROR : gltexfile::open(), File Open %s", fname );
		return false;
	}

	CBaseString &cBString = *pBString;

	m_bZipFile = FALSE;
	
	CString strLine;
	while ( cBString.GetNextLine(strLine) )
	{
		VECSTRING* pVECSTR = new VECSTRING;
		m_cSeperator.DoSeparate ( strLine.GetString(), *pVECSTR );

		VECSTRING_ITER iter = pVECSTR->begin();
		VECSTRING_ITER iter_end = pVECSTR->end();
		for ( ; iter!=iter_end; ++iter )
		{
			STRING &strVAL = (*iter);
			STRING_SIZE nPOS = strVAL.find ( "//" );
			
			//	문자열 앞부분 부터 주석 부분일 때.
			if ( 0==nPOS )
			{
				//	이 부분 부터 끝가지 지워줌.
				pVECSTR->erase ( iter, iter_end );
				break;
			}

			//	문자열 중간에 주석 부가 시작될 때.
			if ( STRING::npos!=nPOS )
			{
				STRING strTEMP(strVAL.begin(),strVAL.begin()+nPOS);
				strVAL = strTEMP;	//	주석 부분을 짤라낸다.

				pVECSTR->erase ( iter+1, iter_end );
				break;
			}
		}

		if ( true==pVECSTR->empty() )
		{
			delete pVECSTR;
			continue;
		}

		DWORD dwNUM = (DWORD)pVECSTR->size();
		m_mapFlags.insert ( std::make_pair((*pVECSTR->begin()),pVECSTR) );
	}

	return true;
}

void gltexfile::clean ()
{
	m_pFOUND_VEC = NULL;
	m_cSeperator.Clear();
	std::for_each(m_mapFlags.begin(),m_mapFlags.end(), std_afunc::DeleteMapObject() );
	m_mapFlags.clear();
}

DWORD gltexfile::getflagnum ( const std::string strFLAG )
{
	MAPFLAG_PAIR pair = m_mapFlags.equal_range ( strFLAG );
	if ( pair.first == pair.second )	return 0;

	return (DWORD)std::distance ( pair.first, pair.second );
}

DWORD gltexfile::getparamnum ( const std::string strFLAG )
{
	m_pFOUND_VEC = findflag ( strFLAG );
	if ( NULL==m_pFOUND_VEC )
	{
		//	여기서는 오류 출력을 하지 않는다.
		return NULL;
	}

	return (DWORD)m_pFOUND_VEC->size();
}

DWORD gltexfile::getparamnum ( const DWORD dwITER, const std::string strFLAG )
{
	MAPFLAG_PAIR pair = m_mapFlags.equal_range ( strFLAG );

	MAPFLAG_ITER iter = pair.first;
	for ( DWORD i=0; i<dwITER; ++i )	++iter;

	m_pFOUND_VEC = (*iter).second;

	return (DWORD)m_pFOUND_VEC->size();
}

gltexfile::VECSTRING* gltexfile::findflag ( const std::string strFlag )
{
	MAPFLAG_ITER iter = m_mapFlags.find ( strFlag );
	if ( iter!=m_mapFlags.end() )
	{
		m_pFOUND_VEC = (*iter).second;
		return m_pFOUND_VEC;
	}

	return NULL;
}

gltexfile::VECSTRING* gltexfile::validflag ( const std::string strFLAG, const int nIDX, const int nSIZE )
{
	if ( nIDX>=(nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(), strFLAG.c_str() );

		MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );

		return NULL;
	}

	//	찾아놓은 스트링 백터가 지금 요청한 FLAG와 같은지 확인.
	if ( NULL!=m_pFOUND_VEC )
	{
		STRING &strVAL = (*m_pFOUND_VEC->begin());
		if ( strFLAG != strVAL )	m_pFOUND_VEC = NULL;
	}

	//	스트링 백터가 무효할 경우 다시 찾음.
	if ( NULL==m_pFOUND_VEC )
	{
		m_pFOUND_VEC = findflag ( strFLAG );
		if ( NULL==m_pFOUND_VEC )
		{
			//	여기서는 오류 출력을 하지 않는다.
			return NULL;
		}
	}

	if ( m_pFOUND_VEC->size() != (nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(), strFLAG.c_str() );

		MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );
		return NULL;
	}

	return m_pFOUND_VEC;
}

gltexfile::VECSTRING* gltexfile::validflag ( const DWORD dwITER, const std::string strFLAG, const int nIDX, const int nSIZE )
{
	if ( nIDX>=(nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(), strFLAG.c_str() );

		MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );

		return NULL;
	}

	DWORD dwNUM = getflagnum ( strFLAG );
	if ( dwNUM<=dwITER )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(), strFLAG.c_str() );

		MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );

		return NULL;
	}

	MAPFLAG_PAIR pair = m_mapFlags.equal_range ( strFLAG );
	
	MAPFLAG_ITER iter = pair.first;
	for ( DWORD i=0; i<dwITER; ++i )	++iter;

	m_pFOUND_VEC = (*iter).second;

	DWORD dwPARAMNUM = (DWORD) m_pFOUND_VEC->size();
	if ( dwPARAMNUM != (nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(), strFLAG.c_str() );

		MessageBox ( NULL, szErrorMsg, "ERROR", MB_OK );
		return NULL;
	}

	return m_pFOUND_VEC;
}