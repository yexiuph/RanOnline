#include "pch.h"
#include <process.h>

#include "./GLDefine.h"
#include "./DxMapEditMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int WINAPI DxMapEditMan::LOADING_THREAD::LoadTileThread(
	LPVOID pData )
{
	while (1)
	{
		DATA* _pData = (DATA*)pData;

		NSQUADTREE::BASEMAP_ITER iter_DEL;
		NSQUADTREE::BASEMAP_ITER iter = NSQUADTREE::m_mapBaseLOADING.begin();
		for ( ; iter!=NSQUADTREE::m_mapBaseLOADING.end(); )
		{
			if ( !_pData->bEnable )
			{
				NSQUADTREE::m_mapBaseLOADING.clear();
				_pData->bDelete = TRUE;
				return 0;
			}

			QuadBase* pQuadBase = (QuadBase*)((*iter).second.pObject);
			if ( pQuadBase )
			{
				pQuadBase->LoadFile ( _pData->pd3dDevice, TRUE );

				iter_DEL = iter;
				++iter;

				NSQUADTREE::m_mapBaseLOADING.erase ( iter_DEL );

				Sleep( 1 );
			}
		}

		if ( !_pData->bEnable )
		{
			NSQUADTREE::m_mapBaseLOADING.clear();
			_pData->bDelete = TRUE;
			return 0;
		}

		Sleep( 1 );
	}

	return 0;
}

BOOL DxMapEditMan::LOADING_THREAD::StartThread(
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

void	DxMapEditMan::LOADING_THREAD::EndThread ()
{
	sData.bEnable = FALSE;
}