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
	// �����尡 ���� �ִٸ� �� �������� ����
	if ( m_sData.bEnable )	return TRUE;

	// Note : ������ ������ �ִ��� üũ
	if ( !pSFile->IsOpen() )	return FALSE;

	// �ʱ�ȭ
	m_sData.pd3dDevice = pd3dDevice;
	m_sData.bEnable = TRUE;
	m_sData.bDelete = FALSE;
	m_sData.pSFile = pSFile;
	m_sData.plistLoadingData = plistLoadingData;
	m_sData.pCSLockLoading = pCSLockLoading;

	// ����
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
	// �����尡 ���� �ִٸ� �� ������ ����.
	if( !m_sData.bEnable )	return;

	// ������ ���°��� ����
	m_sData.bEnable = FALSE;

	// Note : Data �ε��� Thread ���� �̷�������� Thread �� ���� �Ŀ� ������ �Ѵ�.
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

		// OBJOCTree�� �ּҸ� ��´�.
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

		// SFile�� �𸣰ų� ������ ���� ���� ���� ��� �׳� �Ѿ��.
		if( !_pData->pSFile || !_pData->pSFile->IsOpen() )
		{
			goto _RETURN;
		}

		// OBJOCTree�� �ּҸ� ����ٸ� ���� �ε��� �Ѵ�.
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
	// �����尡 ���� �ִٸ� �� �������� ����
	if( m_sData.bEnable )	return TRUE;

	// �ʱ�ȭ
	m_sData.pd3dDevice = pd3dDevice;
	m_sData.bEnable = TRUE;
	m_sData.bDelete = FALSE;
	m_sData.plistLoadingData = &m_listColorData;
	m_sData.pCSLockVB = &m_CSLockColor;

	// ����
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
	// �����尡 ���� �ִٸ� �� ������ ����.
	if( !m_sData.bEnable )	return;

	// ������ ���°��� ����
	m_sData.bEnable = FALSE;

	// Note : Data �ε��� Thread ���� �̷�������� Thread �� ���� �Ŀ� ������ �Ѵ�.
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

		// OBJOCTree�� �ּҸ� ���� �� List�� �ִ� ���� �����ش�.
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

		// OBJOCTree�� �ּҸ� ����ٸ� Color�� ��ȭ��Ŵ.
		if( OcTree )	OcTree->FrameMoveCOLOR();

		// Note : �ʱ�ȭ �϶�� ����� ��������.
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
	m_sData.bProgressInit = TRUE;	// �ʱ�ȭ�� �����϶�.

	// Note : �ٸ� Thread���� List�� �� ���� �� bProgressInit�� FLASE ������ �ٲ� ���̴�.
	while( m_sData.bProgressInit )
	{
		Sleep( 0 );
	}
}




