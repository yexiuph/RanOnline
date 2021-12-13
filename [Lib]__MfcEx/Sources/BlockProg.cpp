#include "pch.h"

#include "afxmt.h"

#include "RANPARAM.h"
#include "DebugSet.h"
#include "gltexfile.h"
#include "HPro.h"
#include "ExceptionHandler.h"
#include "compbyte.h"

#include "BlockProg.h"
#include <process.h> // _beginthreadex()

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CBLOCK_PROG::LOAD ( std::string strROOT, std::string strFile )
{
	m_vecBLOCK_LIST.clear();

	m_strLIST_FILE = strROOT;
	m_strLIST_FILE += std::string("\\") + strFile;

	gltexfile cFILE;
	cFILE.reg_sep ( '\t' );
	cFILE.reg_sep ( ',' );

	if ( !cFILE.open(m_strLIST_FILE.c_str(),true) )	return false;

	DWORD dwNUM(0);
	bool bOK = cFILE.getflag ( "BINARY_IMAGE_NUM", 1, 1, dwNUM );
	if ( !bOK )											return false;
	if ( dwNUM==0 )										return false;	

	m_vecBLOCK_LIST.reserve ( dwNUM );

	DWORD dwFLAG_NUM = cFILE.getflagnum ( "INFO" );
	if ( dwNUM!=dwFLAG_NUM )							return false;

	for ( DWORD i=0; i<dwNUM; ++i )
	{
		SBLOCK_INFO sINFO;
		cFILE.getflag ( i, "INFO", 1, 7, sINFO.bBLOCK );
		cFILE.getflag ( i, "INFO", 2, 7, sINFO.strName );
		cFILE.getflag ( i, "INFO", 3, 7, sINFO.strWindowText );
		cFILE.getflag ( i, "INFO", 4, 7, sINFO.strProductName );
		cFILE.getflag ( i, "INFO", 5, 7, sINFO.strFileDescription );
		cFILE.getflag ( i, "INFO", 6, 7, sINFO.strLoadDll );
		cFILE.getflag ( i, "INFO", 7, 7, sINFO.strFileStream );
		
		if ( sINFO.strFileStream!="null" )
		{
			size_t nLength = sINFO.strFileStream.size()/2;
			BYTE *pSTREAM = (BYTE *) _alloca ( nLength );
			compbyte::string2hex ( (const UCHAR *)sINFO.strFileStream.c_str(), pSTREAM, DWORD(nLength*2) );
			
			sINFO.vecFileStream.reserve(nLength);
			for ( size_t n=0; n<nLength; ++n )	sINFO.vecFileStream.push_back ( pSTREAM[n] );
		}

		sINFO.bVALID = sINFO.valid();
		
		m_vecBLOCK_LIST.push_back ( sINFO );
	}

	return true;
}

CBLOCK_PROG::CBLOCK_PROG () :
	m_pHPro(NULL),
	m_bDETECT(true)
{
	m_pHPro = new CHPro;
}

CBLOCK_PROG::~CBLOCK_PROG ()
{
	SAFE_DELETE(m_pHPro);
}

bool CBLOCK_PROG::DoDETECT ()
{
	if ( !m_pHPro )				return false;

	VECDETECT vecDETECT;
	vecDETECT = m_pHPro->ProcessDetection ( GetVecBlock() );
	if ( vecDETECT.empty() )	return true;

	m_criticalSECT.Lock();
	{
		//	검출된 모듈이 있을때.
		m_bDETECT = true;

		for ( unsigned int i=0; i<vecDETECT.size(); ++i )
		{
			m_vecDETECT.push_back ( vecDETECT[i] );
		}
	}
	m_criticalSECT.Unlock();

	return true;
}

CBLOCK_PROG::SDETECT CBLOCK_PROG::NextDETECT ()
{
	SDETECT sDETECT;
	if ( !m_bDETECT )	return sDETECT;
	
	m_criticalSECT.Lock();
	{
		bool bDETECT = false;
		for ( unsigned int i=0; i<m_vecDETECT.size(); ++i )
		{
			if ( !m_vecDETECT[i].bREPORT )
			{
				m_vecDETECT[i].bREPORT = true;
				sDETECT = m_vecDETECT[i];
				break;
			}
		}

		if ( !bDETECT )		m_bDETECT = false;
	}
	m_criticalSECT.Unlock();

	return sDETECT;
}

CBLOCK_PROG::STHREAD	CBLOCK_PROG::m_sTHREAD;
CCriticalSection		CBLOCK_PROG::m_criticalSECT;

BOOL CBLOCK_PROG::DoSTARTTHREAD ()
{
	if ( m_hThread )		return FALSE;

	m_sTHREAD.m_pPROCESS = this;

	//m_hThread = ::CreateThread ( NULL, 0, ThreadMain, &m_sTHREAD, 0, &m_dwThreadID );
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL,
								0,
								CBLOCK_PROG::ThreadMain,
								&m_sTHREAD,
								0,
								(unsigned *) &m_dwThreadID );
	if ( !m_hThread )	return FALSE;

	return TRUE;
}

void CBLOCK_PROG::DoENDTHREAD ()
{
	m_sTHREAD.m_bFORCE_TERMINATE = TRUE;
	while ( m_sTHREAD.m_bEND==FALSE )	{ Sleep(1);	};

	CloseHandle( m_hThread );
	m_hThread = NULL;
	m_dwThreadID = 0;
}

unsigned int WINAPI CBLOCK_PROG::ThreadMain(
	LPVOID pData )
{
	//	Note : 덤프의 옵션을 조절한다.
	//
	_SETDUMPTYPE ( MiniDumpNormal );

	__try
	{
		STHREAD *pTHREAD = (STHREAD *) pData;
		CBLOCK_PROG *pPROG = (CBLOCK_PROG*) pTHREAD->m_pPROCESS;
		if ( !pPROG )
		{
			pTHREAD->m_bEND = TRUE;
			return 0;
		}

		while ( pTHREAD->m_bFORCE_TERMINATE==FALSE )
		{
			Sleep( 3000 );

			bool bOK = pPROG->DoDETECT();
			if ( !bOK )
			{
				Sleep( 5000 );
				break;
			}

			Sleep( 5000 );
		};

		pTHREAD->m_bEND = TRUE;
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("BLOCK_PROG.cpp - ThreadMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}

	return 0;
}
