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

	m_sBINFILELOAD_THREAD.m_bFORCE_TERMINATE = FALSE;	// ã�ٺ��� ���Դ�. �׽�Ʈ �ʿ�
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
	m_sBINFILELOAD_THREAD.m_bProgressInit = TRUE;	// �ʱ�ȭ�� �����϶�.

	// Note : �ٸ� Thread���� List�� �� ���� �� bProgressInit�� FLASE ������ �ٲ� ���̴�.
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

		//	Note : �ε��� ���̳ʸ� ���� ��������.
		EnterCriticalSection(&m_criticalLOAD);
		{
			if ( !m_listBIN_LOAD.empty() )
			{
				sLOAD_BIN = m_listBIN_LOAD.front();
				m_listBIN_LOAD.pop_front();
			}
		}
		LeaveCriticalSection(&m_criticalLOAD);

		// Note : �ʱ�ȭ �϶�� ����� ��������.
		if( pTHREAD->m_bProgressInit )
		{
			EnterCriticalSection(&m_criticalLOAD);
			{
				m_listBIN_LOAD.clear();
			}
			LeaveCriticalSection(&m_criticalLOAD);

			//	Note : �ε��� ���̳ʸ� ���� ����.
			sLOAD_BIN = SBIN();

			pTHREAD->m_bProgressInit = FALSE;

			continue;
		}

		Sleep( 1 );

		//	Note : �ε���.
		SAnimContainer::SANILIST listANI;
		SAnimContainer::SANILIST listANIUPBODY;
		HRESULT hr = SAnimContainer::LoadAnimFromFile ( sLOAD_BIN.m_strFILEPATH, sLOAD_BIN.m_pSkeleton, listANI, listANIUPBODY );

		if ( SUCCEEDED(hr) )
		{
			//	Note : �ε��� ���� ����.
			bool bOK = DxSkinAniMan::GetInstance().SetBinAnim ( sLOAD_BIN.m_strFILE.c_str(), listANI, listANIUPBODY );

			//	Note : ���� ���н� �ڷ� ����.
			if ( !bOK )
			{
				std::for_each(listANI.begin(),listANI.end(),std_afunc::DeleteObject());
			}
		}

		//	Note : �ε��� ���̳ʸ� ���� ����.
		sLOAD_BIN = SBIN();
   	}

	pTHREAD->m_bEND = TRUE;

	return 0;
}
