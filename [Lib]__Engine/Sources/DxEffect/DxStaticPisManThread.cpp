#include "pch.h"
#include <process.h>

#include "./GLDefine.h"
#include "./DxStaticPisMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int WINAPI DxPieceMan::LOADING_THREAD::LoadTileThread(
	LPVOID pData )
{
	//while (1)
	//{
	//	DATA* _pData = (DATA*)pData;

	//	NSMATERIALMESH::MAPMATERIALLOAD_ITER iter_DEL;
	//	NSMATERIALMESH::MAPMATERIALLOAD_ITER iter = NSMATERIALMESH::m_mapLOADING.begin();
	//	for ( ; iter!=NSMATERIALMESH::m_mapLOADING.end(); ++iter )
	//	{
	//		if ( !_pData->bEnable )
	//		{
	//			NSMATERIALMESH::m_mapLOADING.clear();
	//			_pData->bDelete = TRUE;
	//			return 0;
	//		}

	//		DxMaterialMAP*	pCur = (*iter).first;
	//		if ( !(*iter).second )
	//		{
	//			pCur->CreateMesh ( _pData->pd3dDevice );
	//			(*iter).second = TRUE; // 완료 확인
	//		}
	//	}

	//	if ( !_pData->bEnable )
	//	{
	//		NSMATERIALMESH::m_mapLOADING.clear();
	//		_pData->bDelete = TRUE;
	//		return 0;
	//	}

	//	Sleep( 33 );
	//}

	return 0;
}

BOOL DxPieceMan::LOADING_THREAD::StartThread(
	LPDIRECT3DDEVICEQ pd3dDevice )
{
	sData.pd3dDevice = pd3dDevice;
	sData.bEnable = TRUE;
	sData.bDelete = FALSE;

	//hThread = CreateThread ( NULL, 0, LoadTileThread, &sData, 0, &dwThreadID );
	hThread = (HANDLE) ::_beginthreadex(
							NULL,
							0,
							LoadTileThread,
							&sData,
							0,
							(unsigned int*) &dwThreadID );
	if ( !hThread )
	{
		return FALSE;
	}

	return TRUE;
}

void	DxPieceMan::LOADING_THREAD::EndThread ()
{
	sData.bEnable = FALSE;
}