#include "pch.h"

#include "SerialFile.h"
#include "DxMethods.h"
#include "DxSkinMesh9_CPU.h"
#include "DxSkinMesh9_HLSL.h"
#include "DxSkinMesh9_NORMAL.h"

#include "DxSkinMesh9.h"
#include "DxSkinMeshContainer9.h"
#include "DxBoneCollector.h"
#include "DxSkinMeshMan.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

//	D3DNONINDEXED	D3DINDEXEDHLSLVS
METHOD	DxSkinMesh9::m_SkinningMethod =	D3DINDEXEDHLSLVS;	//D3DNONINDEXED;	//D3DINDEXEDHLSLVS;
DWORD	DxSkinMesh9::m_dwBehaviorFlags = NULL;

DxSkinMesh9::DxSkinMesh9 () :
	m_pFrameRoot(NULL),
	m_pSkeleton(0),
	m_vObjectCenter(0,0,0),
	m_fObjectRadius(0),
	m_pAnimController(NULL),

	m_bUseSoftwareVP(false),

	m_pMeshContainerHead(NULL),

	m_bOriginDraw(TRUE),
	m_bAlpha(FALSE),
	m_bShadow(FALSE),
	m_bAlphaTex(FALSE),
	m_bLevel(FALSE)
{
	memset( m_szName, 0, sizeof(char)*MAX_PATH );
	memset( m_szSkeleton, 0, sizeof(char)*MAX_PATH );

	D3DXMatrixIdentity ( &m_matWorld );
	Init_Detail_Texture ();
}

//--------------------------------------------------------------------------------------
// Called to setup the pointers for a given bone to its transformation matrix
//--------------------------------------------------------------------------------------
HRESULT DxSkinMesh9::SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	if ( !m_pSkeleton )		return E_FAIL;

    UINT iBone, cBones;
	DxBoneTrans* pBoneTrans;

    SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

    // if there is a skinmesh, then setup the bone matrices
    if (pMeshContainer->pSkinInfo != NULL)
    {
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

        pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
        if (pMeshContainer->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        for (iBone = 0; iBone < cBones; iBone++)
        {
			pBoneTrans = m_pSkeleton->FindBone ( pMeshContainer->pSkinInfo->GetBoneName(iBone) );
            if ( pBoneTrans == NULL )		return E_FAIL;

			pMeshContainer->ppBoneMatrixPtrs[iBone] = &pBoneTrans->matCombined;
        }
    }
	else
	{
		SFrame* pFrame = (SFrame*)pFrameBase;
		SFrame* pframeParent = FindParentFrame ( pFrame );

		if ( pframeParent )
		{
			SFrame* pBoneFrame = FindParentFrame ( pframeParent );
			if( pBoneFrame && pBoneFrame->Name )
			{
				DxBoneTrans* pBoneTrans = m_pSkeleton->FindBone ( pBoneFrame->Name );
				if ( pBoneTrans )
				{
					SIZE_T tLen = strlen(pBoneFrame->Name)+1;
					pMeshContainer->szBoneName = new char[tLen];
					StringCchCopy ( pMeshContainer->szBoneName, tLen, pBoneFrame->Name );

					pMeshContainer->dwNumBoneMatrix = cBones = 3;
					pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];

					pMeshContainer->ppBoneMatrixPtrs[0] = &pFrame->TransformationMatrix;		//[LOCAL]
					pMeshContainer->ppBoneMatrixPtrs[1] = &pframeParent->TransformationMatrix;	//[LOCAL] parent
					pMeshContainer->ppBoneMatrixPtrs[2] = &pBoneTrans->matCombined;				//[BONE]
				}
			}
		}
	}

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Called to setup the pointers for a given bone to its transformation matrix
//--------------------------------------------------------------------------------------
HRESULT DxSkinMesh9::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
{
    HRESULT hr;

    if (pFrame->pMeshContainer != NULL)
    {
        hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer,pFrame);
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// update the frame matrices
//--------------------------------------------------------------------------------------
void DxSkinMesh9::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
{
    SFrame *pFrame = (SFrame*)pFrameBase;

    if (pParentMatrix != NULL)
        D3DXMatrixMultiply ( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

    if (pFrame->pFrameSibling != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}

//--------------------------------------------------------------------------------------
// update the skinning method
//--------------------------------------------------------------------------------------
void DxSkinMesh9::UpdateSkinningMethod( LPD3DXFRAME pFrameBase )
{
    SFrame *pFrame = (SFrame*)pFrameBase;
    SMeshContainer *pMeshContainer;

    pMeshContainer = (SMeshContainer *)pFrame->pMeshContainer;

    while( pMeshContainer != NULL )
    {
        pMeshContainer->GenerateSkinnedMesh ( DXUTGetD3DDevice(), m_bUseSoftwareVP );

        pMeshContainer = (SMeshContainer *)pMeshContainer->pNextMeshContainer;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        UpdateSkinningMethod(pFrame->pFrameSibling);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        UpdateSkinningMethod(pFrame->pFrameFirstChild);
    }
}

void DxSkinMesh9::ReleaseAttributeTable( LPD3DXFRAME pFrameBase )
{
	if ( !pFrameBase )		return;

    SFrame *pFrame = (SFrame*)pFrameBase;
    SMeshContainer *pMeshContainer;

    pMeshContainer = (SMeshContainer *)pFrame->pMeshContainer;

    while( pMeshContainer != NULL )
    {
        SAFE_DELETE_ARRAY(pMeshContainer->pAttributeTable);

        pMeshContainer = (SMeshContainer *)pMeshContainer->pNextMeshContainer;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        ReleaseAttributeTable(pFrame->pFrameSibling);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        ReleaseAttributeTable(pFrame->pFrameFirstChild);
    }
}

HRESULT DxSkinMesh9::StaticCreate (IDirect3DDevice9* pd3dDevice )
{
	return S_OK;
}

HRESULT DxSkinMesh9::StaticResetDevice ( IDirect3DDevice9* pd3dDevice )
{
	return S_OK;
}

void DxSkinMesh9::StaticLost ()
{
   
}

void DxSkinMesh9::StaticDestroy ()
{
 
}

HRESULT DxSkinMesh9::OnCreateSkin ( IDirect3DDevice9* pd3dDevice, const char* szSkinFile, const char *szSkeletonFile, BOOL bThread )
{
	HRESULT hr;
	StringCchCopy ( m_szName, MAX_PATH, szSkinFile );
	StringCchCopy ( m_szSkeleton, MAX_PATH, szSkeletonFile );

	m_pSkeleton = DxBoneCollector::GetInstance().Load ( m_szSkeleton, pd3dDevice );
	if ( !m_pSkeleton )	return E_FAIL;

	CAllocateHierarchy Alloc;
	Alloc.m_pSkinMesh = this;
	Alloc.m_bThread = bThread;

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxSkinMeshMan::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szSkinFile );

	SAFE_RELEASE( m_pAnimController );
	V_RETURN( D3DXLoadMeshHierarchyFromXA ( szPathName, D3DXMESH_MANAGED, pd3dDevice,
											&Alloc, NULL, &m_pFrameRoot, &m_pAnimController ) );
	V_RETURN( SetupBoneMatrixPointers( m_pFrameRoot ) );
	V_RETURN( SetupNameOnMeshContainer( m_pFrameRoot ) );
	V_RETURN( D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius ) );

    // Obtain the behavior flags
    D3DDEVICE_CREATION_PARAMETERS cp;
    pd3dDevice->GetCreationParameters( &cp );
	m_dwBehaviorFlags = cp.BehaviorFlags;

	return S_OK;
}

HRESULT DxSkinMesh9::OnSkinFrameMove ( double fTime, float fElapsedTime, D3DXMATRIXA16 &matWorld )
{
    if ( m_pAnimController != NULL )
        m_pAnimController->AdvanceTime ( fElapsedTime, NULL );

    UpdateFrameMatrices ( m_pFrameRoot, &matWorld );

	return S_OK;
}

void DxSkinMesh9::OnSkinFrameRender ( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
	DrawFrame( pd3dDevice, m_pFrameRoot );
}

HRESULT DxSkinMesh9::OnNewSkinnedMethod ( METHOD NewSkinningMethod )
{
    // If the selected skinning method is different than the current one
    if( m_SkinningMethod != NewSkinningMethod )
    {
        m_SkinningMethod = NewSkinningMethod;

        // update the meshes to the new skinning method
        UpdateSkinningMethod( m_pFrameRoot );
    }

	return S_OK;
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void DxSkinMesh9::OnSkinDestroyDevice ()
{
	CAllocateHierarchy Alloc;
    D3DXFrameDestroy( m_pFrameRoot, &Alloc );
	m_pFrameRoot = NULL;

    SAFE_RELEASE( m_pAnimController );
}

void DxSkinMesh9::OnSkinCleanUp ()
{
    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.
    ReleaseAttributeTable( m_pFrameRoot );
}

SMeshContainer* DxSkinMesh9::FindMeshContainer ( LPD3DXFRAME pFrame, char *szName )
{
	if ( !pFrame )	return NULL;

    if ( pFrame->pMeshContainer!=NULL )
    {
		if ( !strcmp(pFrame->pMeshContainer->Name,szName) )
			return (SMeshContainer*)pFrame->pMeshContainer;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        SMeshContainer* pMeshContainer = FindMeshContainer(pFrame->pFrameSibling,szName);
        if ( pMeshContainer )	return (SMeshContainer*)pMeshContainer;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        SMeshContainer* pMeshContainer = FindMeshContainer(pFrame->pFrameFirstChild,szName);
		if ( pMeshContainer )	return (SMeshContainer*)pMeshContainer;
    }

	return NULL;
}

SFrame* DxSkinMesh9::FindFrame ( char *szName )
{
	if ( m_pFrameRoot == NULL )	return NULL;
	
	return ((SFrame*)m_pFrameRoot)->FindFrame ( szName );
}

SFrame* DxSkinMesh9::FindParentFrame ( SFrame* pFrame )
{
	if ( m_pFrameRoot == NULL || pFrame == NULL )	return NULL;
	
	return ((SFrame*)m_pFrameRoot)->FindParentFrame ( pFrame );
}

HRESULT DxSkinMesh9::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer* pmcMesh, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr(S_OK);
	if ( !pmcMesh->pSkinInfo )	return E_FAIL;

	DWORD cBones  = pmcMesh->pSkinInfo->GetNumBones();

	// set up bone transforms
	for ( DWORD iBone = 0; iBone < cBones; ++iBone )
	{
		D3DXMatrixMultiply
		(
			&DxSkinMesh9_HLSL::g_pBoneMatrices[iBone],	// output
			&pmcMesh->pBoneOffsetMatrices[iBone], 
			pmcMesh->ppBoneMatrixPtrs[iBone]
		);
	}

	hr = pmcMesh->CalculateBoundingBox ( pd3dDevice, DxSkinMesh9_HLSL::g_pBoneMatrices, vMax, vMin );

	return hr;
}

void DxSkinMesh9::CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness )
{
	//SMeshContainer* pCur = m_pMeshContainerHead;
	//while( pCur )
	//{
	//	pCur->IsCartoon_CreateMesh( pd3dDevice, fThickness );
	//	pCur = pCur->pGlobalNext;
	//}
}

void DxSkinMesh9::SaveNORMAL( TCHAR* pName )
{
	TSTRING strFileName = DxSkinMeshMan::GetInstance().GetPath();
	strFileName += m_szName;
	if( strFileName.empty() )	return;

	strFileName = ChangeExtName( strFileName.c_str(), _T("enm") );

	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
		return;
	}

	SFile << DxSkinMesh9_NORMAL::VERSION;

	SaveNORMAL( SFile );

	SFile.CloseFile();
}

void DxSkinMesh9::SaveHLSL( TCHAR* pName )
{
	TSTRING strFileName = DxSkinMeshMan::GetInstance().GetPath();
	strFileName += m_szName;
	if( strFileName.empty() )	return;

	strFileName = ChangeExtName( strFileName.c_str(), _T("ehs") );

	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
		return;
	}

	SFile << DxSkinMesh9_HLSL::VERSION;

	SaveHLSL( SFile );

	SFile.CloseFile();
}

void DxSkinMesh9::SaveNormalMesh( TCHAR* pName )
{
	TSTRING strFileName = DxSkinMeshMan::GetInstance().GetPath();
	strFileName += m_szName;
	if( strFileName.empty() )	return;

	strFileName = ChangeExtName( strFileName.c_str(), _T("nor") );

	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
		return;
	}

	SFile << DxSkinMesh9_HLSL::VERSION;

	SaveNormalMesh( SFile );

	SFile.CloseFile();
}

void DxSkinMesh9::SaveLOD_SW( TCHAR* pName )
{
	TSTRING strFileName = DxSkinMeshMan::GetInstance().GetPath();
	strFileName += m_szName;
	if( strFileName.empty() )	return;

	strFileName = ChangeExtName( strFileName.c_str(), _T("lod_sw") );

	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, strFileName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
		return;
	}

	SFile << DxSkinMesh9_CPU::VERSION;

	SaveLOD_SW( SFile );

	SFile.CloseFile();
}

void DxSkinMesh9::SaveNORMAL( CSerialFile& SFile )
{
	SMeshContainer* pCur = m_pMeshContainerHead;
	DWORD dwCount(0L);
	while( pCur )
	{
		++dwCount;
		pCur = pCur->pGlobalNext;	
	}
	SFile << dwCount;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		pCur = m_pMeshContainerHead;
		while( pCur )
		{
			pCur->SaveNORMAL( SFile );
			pCur = pCur->pGlobalNext;	
		}
	}
	SFile.EndBlock( EMBLOCK_00 );
}

void DxSkinMesh9::SaveHLSL( CSerialFile& SFile )
{
	SMeshContainer* pCur = m_pMeshContainerHead;
	DWORD dwCount(0L);
	while( pCur )
	{
		++dwCount;
		pCur = pCur->pGlobalNext;	
	}
	SFile << dwCount;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		pCur = m_pMeshContainerHead;
		while( pCur )
		{
			pCur->SaveHLSL( SFile );
			pCur = pCur->pGlobalNext;	
		}
	}
	SFile.EndBlock( EMBLOCK_00 );
}

void DxSkinMesh9::SaveNormalMesh( CSerialFile& SFile )
{
	SMeshContainer* pCur = m_pMeshContainerHead;
	DWORD dwCount(0L);
	while( pCur )
	{
		++dwCount;
		pCur = pCur->pGlobalNext;	
	}
	SFile << dwCount;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		pCur = m_pMeshContainerHead;
		while( pCur )
		{
			pCur->SaveNormalMesh( SFile );
			pCur = pCur->pGlobalNext;	
		}
	}
	SFile.EndBlock( EMBLOCK_00 );
}

void DxSkinMesh9::SaveLOD_SW( CSerialFile& SFile )
{
	SMeshContainer* pCur = m_pMeshContainerHead;
	DWORD dwCount(0L);
	while( pCur )
	{
		++dwCount;
		pCur = pCur->pGlobalNext;	
	}
	SFile << dwCount;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		pCur = m_pMeshContainerHead;
		while( pCur )
		{
			pCur->SaveLOD_SW( SFile );
			pCur = pCur->pGlobalNext;	
		}
	}
	SFile.EndBlock( EMBLOCK_00 );
}



