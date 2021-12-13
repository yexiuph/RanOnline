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

	std::auto_ptr<CBaseString> pBString( GLOGIC::openfile_basestring(	m_bZipFile, 
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
			
			//	���ڿ� �պκ� ���� �ּ� �κ��� ��.
			if ( 0==nPOS )
			{
				//	�� �κ� ���� ������ ������.
				pVECSTR->erase ( iter, iter_end );
				break;
			}

			//	���ڿ� �߰��� �ּ� �ΰ� ���۵� ��.
			if ( STRING::npos!=nPOS )
			{
				STRING strTEMP(strVAL.begin(),strVAL.begin()+nPOS);
				strVAL = strTEMP;	//	�ּ� �κ��� ©�󳽴�.

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
		//	���⼭�� ���� ����� ���� �ʴ´�.
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

	//	ã�Ƴ��� ��Ʈ�� ���Ͱ� ���� ��û�� FLAG�� ������ Ȯ��.
	if ( NULL!=m_pFOUND_VEC )
	{
		STRING &strVAL = (*m_pFOUND_VEC->begin());
		if ( strFLAG != strVAL )	m_pFOUND_VEC = NULL;
	}

	//	��Ʈ�� ���Ͱ� ��ȿ�� ��� �ٽ� ã��.
	if ( NULL==m_pFOUND_VEC )
	{
		m_pFOUND_VEC = findflag ( strFLAG );
		if ( NULL==m_pFOUND_VEC )
		{
			//	���⼭�� ���� ����� ���� �ʴ´�.
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