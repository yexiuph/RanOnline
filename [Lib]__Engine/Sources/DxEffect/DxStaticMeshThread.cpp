#include "pch.h"
#include <process.h>
#include "./NsOCTree.h"
#include "./DxStaticMeshThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//										DxStaticMesh	_	THREAD
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMesh_THREAD::~DxStaticMesh_THREAD()
{
}

BOOL DxStaticMesh_THREAD::StartThread(
	LPDIRECT3DDEVICEQ pd3dDevice,
	CSerialFile* pSFile,
	LOADINGDATALIST* plistLoadingData,
	CRITICAL_SECTION* pCSLockLoading )
{
	// 쓰레드가 켜져 있다면 또 생성하지 않음
	if ( m_sData.bEnable )	return TRUE;

	// Note : 파일이 열려져 있는지 체크
	if ( !pSFile->IsOpen() )	return FALSE;

	// 초기화
	m_sData.pd3dDevice = pd3dDevice;
	m_sData.bEnable = TRUE;
	m_sData.bDelete = FALSE;
	m_sData.pSFile = pSFile;
	m_sData.plistLoadingData = plistLoadingData;
	m_sData.pCSLockLoading = pCSLockLoading;

	// 생성
	//hThread = CreateThread ( NULL, 0, LoadThread, &m_sData, 0, &dwThreadID );
	hThread = (HANDLE) ::_beginthreadex(
							NULL,
							0,
							LoadThread,
							&m_sData,
							0,
							(unsigned int*) &dwThreadID );
	if ( !hThread ) return FALSE;

	return TRUE;
}

void DxStaticMesh_THREAD::EndThread()
{
	// 쓰레드가 꺼져 있다면 또 지우지 않음.
	if( !m_sData.bEnable )	return;

	// 쓰레드 끄는것을 예약
	m_sData.bEnable = FALSE;

	// Note : Data 로딩이 Thread 에서 이루어짐으로 Thread 가 종료 후에 삭제를 한다.
	while(1)
	{
		Sleep( 0 );

		if ( m_sData.bDelete )
		{
			EnterCriticalSection( m_sData.pCSLockLoading );
			{
				m_sData.plistLoadingData->clear();
			}
			LeaveCriticalSection( m_sData.pCSLockLoading );

			break;
		}
	}

	CloseHandle( hThread );
	hThread = NULL;
}

unsigned int WINAPI DxStaticMesh_THREAD::LoadThread(
	LPVOID pData )
{
	DATA* _pData = (DATA*)pData;

	while (1)
	{
		Sleep( 1 );

		// OBJOCTree의 주소를 얻는다.
		OBJOCTree* OcTree = NULL;
		EnterCriticalSection(_pData->pCSLockLoading);
		{
			LOADINGDATALIST_ITER iter = _pData->plistLoadingData->begin();
			if( iter!=_pData->plistLoadingData->end() )
			{
				OcTree = (OBJOCTree*)( (*iter) );

				if( OcTree && OcTree->m_bLoad )
				{
					_pData->plistLoadingData->erase( iter );
					LeaveCriticalSection(_pData->pCSLockLoading);
					goto _RETURN;
				}
			}
		}
		LeaveCriticalSection(_pData->pCSLockLoading);

		// SFile을 모르거나 파일이 열려 있지 않을 경우 그냥 넘어간다.
		if( !_pData->pSFile || !_pData->pSFile->IsOpen() )
		{
			goto _RETURN;
		}

		// OBJOCTree의 주소를 얻었다면 이제 로딩을 한다.
		if( OcTree )	OcTree->DynamicLoad( _pData->pd3dDevice, *(_pData->pSFile), TRUE );

_RETURN:
		if ( !_pData->bEnable )
		{
			_pData->bDelete = TRUE;

			return 0;
		}
	}

	return 0;
}


// -----------------------------------------------------------------------------------------------------------------------------------------
//										DxStaticMesh	_	Color	_	THREAD
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMeshColor_THREAD& DxStaticMeshColor_THREAD::GetInstance()
{
	static DxStaticMeshColor_THREAD Instance;
	return Instance;
}

 DxStaticMeshColor_THREAD::DxStaticMeshColor_THREAD() :
	dwThreadID( UINT_MAX ),
	hThread( NULL ),
	hEventTerminate( NULL ),
	m_bGetListData(FALSE),
	m_bOctreeMode(TRUE)
{
	InitializeCriticalSection(&m_CSLockColor);
};

DxStaticMeshColor_THREAD::~DxStaticMeshColor_THREAD()
{
	DeleteCriticalSection(&m_CSLockColor);
}

BOOL DxStaticMeshColor_THREAD::StartThread( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 쓰레드가 켜져 있다면 또 생성하지 않음
	if( m_sData.bEnable )	return TRUE;

	// 초기화
	m_sData.pd3dDevice = pd3dDevice;
	m_sData.bEnable = TRUE;
	m_sData.bDelete = FALSE;
	m_sData.plistLoadingData = &m_listColorData;
	m_sData.pCSLockVB = &m_CSLockColor;

	// 생성
	//hThread = CreateThread ( NULL, 0, LoadThread, &m_sData, 0, &dwThreadID );
	hThread = (HANDLE) ::_beginthreadex(
							NULL,
							0,
							LoadThread,
							&m_sData,
							0,
							(unsigned int*) &dwThreadID );
	if ( !hThread ) return FALSE;

	return TRUE;
}

void DxStaticMeshColor_THREAD::EndThread()
{
	// 쓰레드가 꺼져 있다면 또 지우지 않음.
	if( !m_sData.bEnable )	return;

	// 쓰레드 끄는것을 예약
	m_sData.bEnable = FALSE;

	// Note : Data 로딩이 Thread 에서 이루어짐으로 Thread 가 종료 후에 삭제를 한다.
	while(1)
	{
		Sleep( 0 );

		if ( m_sData.bDelete )
		{
			EnterCriticalSection( &m_CSLockColor );
			{
				m_listColorData.clear();
			}
			LeaveCriticalSection( &m_CSLockColor );

			break;
		}
	}

	CloseHandle( hThread );
	hThread = NULL;
}

unsigned int WINAPI DxStaticMeshColor_THREAD::LoadThread(
	LPVOID pData )
{
	DATA* _pData = (DATA*)pData;

	while (1)
	{
		Sleep( 1 );

		// OBJOCTree의 주소를 얻은 후 List에 있는 것을 지워준다.
		OBJOCTree* OcTree = NULL;
		EnterCriticalSection(_pData->pCSLockVB);
		{
			LOADINGDATALIST_ITER iter = _pData->plistLoadingData->begin();
			if( iter!=_pData->plistLoadingData->end() )
			{
				OcTree = (OBJOCTree*)( (*iter) );
				_pData->plistLoadingData->erase( iter );
			}
		}
		LeaveCriticalSection(_pData->pCSLockVB);

		// OBJOCTree의 주소를 얻었다면 Color를 변화시킴.
		if( OcTree )	OcTree->FrameMoveCOLOR();

		// Note : 초기화 하라는 명령이 떨어졌다.
		if( _pData->bProgressInit )
		{
			EnterCriticalSection(_pData->pCSLockVB);
			{
				_pData->plistLoadingData->clear();
			}
			LeaveCriticalSection(_pData->pCSLockVB);

			_pData->bProgressInit = FALSE;
		}

		if ( !_pData->bEnable )
		{
			_pData->bDelete = TRUE;

			return 0;
		}
	}

	return 0;
}

void DxStaticMeshColor_THREAD::CheckListEmpty()
{
	EnterCriticalSection( &m_CSLockColor );
	{
		if( m_listColorData.empty() )	m_bGetListData = TRUE;
		else							m_bGetListData = FALSE;
	}
	LeaveCriticalSection( &m_CSLockColor );
}

void DxStaticMeshColor_THREAD::DeleteList()
{
	m_sData.bProgressInit = TRUE;	// 초기화를 시작하라.

	// Note : 다른 Thread에서 List를 다 지운 후 bProgressInit을 FLASE 값으로 바꿀 것이다.
	while( m_sData.bProgressInit )
	{
		Sleep( 0 );
	}
}




