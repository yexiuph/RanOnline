#include "pch.h"

#include <algorithm>
#include "./SerialFile.h"
#include "./StringUtils.h"
#include "./StlFunctions.h"

#include "DxSkinAniMan.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxSkinAniMan::LISTBINLOAD	DxSkinAniMan::m_listBIN_LOAD;
CRITICAL_SECTION			DxSkinAniMan::m_criticalLOAD;
DxSkinAniMan::SBINFILELOAD	DxSkinAniMan::m_sBINFILELOAD_THREAD;

BOOL DxSkinAniMan::StartBinFileLoadThread ()
{
	if ( m_hBinFileThread )		return FALSE;

	m_sBINFILELOAD_THREAD.m_bFORCE_TERMINATE = FALSE;	// 찾다보니 나왔다. 테스트 필요
	//m_hBinFileThread = CreateThread ( NULL, 0, BinFileLoadThread, &m_sBINFILELOAD_THREAD, 0, &m_dwBinFileThreadID );
	m_hBinFileThread = (HANDLE) ::_beginthreadex(
									NULL,
									0,
									BinFileLoadThread,
									&m_sBINFILELOAD_THREAD,
									0,
									(unsigned int*) &m_dwBinFileThreadID );
	if ( !m_hBinFileThread )	return FALSE;

	return TRUE;
}

void DxSkinAniMan::EndBinFileLoadThread ()
{
	ClearBinLoadingData ();

	m_sBINFILELOAD_THREAD.m_bFORCE_TERMINATE = TRUE;

	while ( m_sBINFILELOAD_THREAD.m_bEND==FALSE )
	{
		Sleep( 1 );
	}

	CloseHandle( m_hBinFileThread );
	m_hBinFileThread = NULL;
	m_dwBinFileThreadID = 0;
}

void DxSkinAniMan::ClearBinLoadingData ()
{
	m_sBINFILELOAD_THREAD.m_bProgressInit = TRUE;	// 초기화를 시작하라.

	// Note : 다른 Thread에서 List를 다 지운 후 bProgressInit을 FLASE 값으로 바꿀 것이다.
	while( m_sBINFILELOAD_THREAD.m_bProgressInit )
	{
		Sleep( 1 );
	}

	EnterCriticalSection(&m_criticalLOAD);
	{
		m_listBIN_LOAD.clear();
	}
	LeaveCriticalSection(&m_criticalLOAD);
}

void DxSkinAniMan::AddBinFIleLoad ( std::string strFILE, DxSkeleton *pSkeleton )
{
	CString strBinFile;
	STRUTIL::ChangeExt ( strFILE.c_str(), ".x", strBinFile, ".bin" );

	std::string strPath;
	strPath += DxSkinAniMan::GetInstance().GetPath ();
	strPath += strBinFile.GetString();

	SBIN sBIN;
	sBIN.m_pSkeleton = pSkeleton;
	sBIN.m_strFILE = strFILE;
	sBIN.m_strFILEPATH = strPath;

	EnterCriticalSection(&m_criticalLOAD);
	{
		m_listBIN_LOAD.push_back ( sBIN );
	}
	LeaveCriticalSection(&m_criticalLOAD);

	CDebugSet::ToListView ( "load ani_bin [%s]", strFILE.c_str() );
}

unsigned int WINAPI DxSkinAniMan::BinFileLoadThread(
	LPVOID pData )
{
	SBINFILELOAD *pTHREAD = (SBINFILELOAD *) pData;

	pTHREAD->m_bEND = FALSE;

	SBIN sLOAD_BIN;
	while ( pTHREAD->m_bFORCE_TERMINATE==FALSE )
	{
		Sleep( 1 );

		//	Note : 로드할 바이너리 정보 가져오기.
		EnterCriticalSection(&m_criticalLOAD);
		{
			if ( !m_listBIN_LOAD.empty() )
			{
				sLOAD_BIN = m_listBIN_LOAD.front();
				m_listBIN_LOAD.pop_front();
			}
		}
		LeaveCriticalSection(&m_criticalLOAD);

		// Note : 초기화 하라는 명령이 떨어졌다.
		if( pTHREAD->m_bProgressInit )
		{
			EnterCriticalSection(&m_criticalLOAD);
			{
				m_listBIN_LOAD.clear();
			}
			LeaveCriticalSection(&m_criticalLOAD);

			//	Note : 로드할 바이너리 정보 리셋.
			sLOAD_BIN = SBIN();

			pTHREAD->m_bProgressInit = FALSE;

			continue;
		}

		Sleep( 1 );

		//	Note : 로드함.
		SAnimContainer::SANILIST listANI;
		SAnimContainer::SANILIST listANIUPBODY;
		HRESULT hr = SAnimContainer::LoadAnimFromFile ( sLOAD_BIN.m_strFILEPATH, sLOAD_BIN.m_pSkeleton, listANI, listANIUPBODY );

		if ( SUCCEEDED(hr) )
		{
			//	Note : 로드한 정보 설정.
			bool bOK = DxSkinAniMan::GetInstance().SetBinAnim ( sLOAD_BIN.m_strFILE.c_str(), listANI, listANIUPBODY );

			//	Note : 설정 실패시 자료 삭제.
			if ( !bOK )
			{
				std::for_each(listANI.begin(),listANI.end(),std_afunc::DeleteObject());
			}
		}

		//	Note : 로드할 바이너리 정보 리셋.
		sLOAD_BIN = SBIN();
   	}

	pTHREAD->m_bEND = TRUE;

	return 0;
}
