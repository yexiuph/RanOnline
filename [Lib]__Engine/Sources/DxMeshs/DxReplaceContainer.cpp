#include "pch.h"
#include "./DxReplaceContainer.h"

#include "./DxEnvironment.h"
#include "./DxViewPort.h"
#include "./EditMeshs.h"
#include "./DxLandMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note : --------------------------------------Static Variable And Static function---------------------
//
DWORD DXPIECEOBJ::m_dwVERSION = 0x00000100;

DxReplaceContainer& DxReplaceContainer::GetInstance()
{
	static DxReplaceContainer Instance;
	return Instance;
}

void DxReplaceContainer::MakeAABBTree ( LPDXPIECEOBJ *ppArray, DWORD dwArraySize, LPDXPIECEOBJNODE &pTreeNode,
				   const D3DXVECTOR3 vMax, const D3DXVECTOR3 vMin )
{
	if ( dwArraySize == 1 )
	{
		const D3DXMATRIX &matComb = ppArray[0]->matComb;
		
		D3DXVECTOR3 vMaxComb = ppArray[0]->pReplacePiece->vMax;
		D3DXVECTOR3 vMinComb = ppArray[0]->pReplacePiece->vMin;
		COLLISION::TransformAABB ( vMaxComb, vMinComb, matComb );

		pTreeNode->vMax = vMaxComb;
		pTreeNode->vMin = vMinComb;

		pTreeNode->pPieceObj = ppArray[0];
		return;
	}

	float SizeX = vMax.x - vMin.x;
	float SizeY = vMax.y - vMin.y;
	float SizeZ = vMax.z - vMin.z;

	D3DXVECTOR3 vDivMax = vMax;
	D3DXVECTOR3 vDivMin = vMin;

	if ( SizeX > SizeY && SizeX > SizeZ )	vDivMin.x = vMin.x + SizeX/2;
	else if ( SizeY > SizeZ )				vDivMin.y = vMin.y + SizeY/2;
	else									vDivMin.z = vMin.z + SizeZ/2;

	DWORD dwLeftCount = 0, dwRightCount = 0;
	LPDXPIECEOBJ* pLeftArray = new LPDXPIECEOBJ[dwArraySize];
	LPDXPIECEOBJ* pRightArray = new LPDXPIECEOBJ[dwArraySize];

	DWORD i;
	for ( i=0; i<dwArraySize; i++ )
	{
		const D3DXMATRIX &matComb = ppArray[i]->matComb;
		
		D3DXVECTOR3 vCenter = (ppArray[i]->pReplacePiece->vMax+ppArray[i]->pReplacePiece->vMin)*0.5f;
		D3DXVec3TransformCoord ( &vCenter, &vCenter, &matComb );

		if ( COLLISION::IsWithInPoint(vDivMax,vDivMin,vCenter) )
			pLeftArray[dwLeftCount++] = ppArray[i];
		else
			pRightArray[dwRightCount++] = ppArray[i];
	}

	if ( dwLeftCount == 0 || dwRightCount == 0 )
	{
		DWORD dwDivCount = dwArraySize / 2;
	
		dwLeftCount = 0;
		dwRightCount = 0;

		for ( i=0; i<dwDivCount; i++ )
			pLeftArray[dwLeftCount++] = ppArray[i];

		for ( i=dwDivCount; i<dwArraySize; i++ )
			pRightArray[dwRightCount++] = ppArray[i];
	}

	//	Note : Create Left Node
	//
	D3DXVECTOR3 vCMax, vCMin;
	vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	for ( i=0; i<dwLeftCount; i++ )
	{
		const D3DXMATRIX &matComb = pLeftArray[i]->matComb;

		D3DXVECTOR3 vNodeMax = pLeftArray[i]->pReplacePiece->vMax;
		D3DXVECTOR3 vNodeMin = pLeftArray[i]->pReplacePiece->vMin;
		COLLISION::TransformAABB ( vNodeMax, vNodeMin, matComb );

		if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
		if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
		if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

		if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
		if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
		if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;
	}

	pTreeNode->pLeftChild = new DXPIECEOBJNODE;
	pTreeNode->pLeftChild->vMax = vCMax;
	pTreeNode->pLeftChild->vMin = vCMin;
	MakeAABBTree ( pLeftArray, dwLeftCount, pTreeNode->pLeftChild, vCMax, vCMin );

	//	Note : Create Right Node
	//
	vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	for ( i=0; i<dwRightCount; i++ )
	{
		const D3DXMATRIX &matComb = pRightArray[i]->matComb;

		D3DXVECTOR3 vNodeMax = pRightArray[i]->pReplacePiece->vMax;
		D3DXVECTOR3 vNodeMin = pRightArray[i]->pReplacePiece->vMin;
		COLLISION::TransformAABB ( vNodeMax, vNodeMin, matComb );

		if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
		if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
		if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

		if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
		if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
		if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;
	}

	pTreeNode->pRightChild = new DXPIECEOBJNODE;
	pTreeNode->pRightChild->vMax = vCMax;
	pTreeNode->pRightChild->vMin = vCMin;
	MakeAABBTree ( pRightArray, dwRightCount, pTreeNode->pRightChild, vCMax, vCMin );

	SAFE_DELETE_ARRAY(pLeftArray);
	SAFE_DELETE_ARRAY(pRightArray);
}

void DxReplaceContainer::MakeAABBTree ( LPDXPIECEOBJNODE &pTreeHead, LPDXPIECEOBJ pListHead )
{
	SAFE_DELETE(pTreeHead);

	DWORD dwNodeCount = 0;
	LPDXPIECEOBJ pNodeCur;

	//	Note : Check List Number
	pNodeCur = pListHead;
	while ( pNodeCur )
	{
		dwNodeCount++;
		pNodeCur = pNodeCur->pNext;
	}

	if ( dwNodeCount == 0 )	return;

	//	Note : Point Array Save. And Sorting
	LPDXPIECEOBJ *ppArray = new LPDXPIECEOBJ[dwNodeCount];

	dwNodeCount = 0;
	pNodeCur = pListHead;
	while ( pNodeCur )
	{
		ppArray[dwNodeCount++] = pNodeCur;
		pNodeCur = pNodeCur->pNext;
	}

	D3DXVECTOR3 vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	D3DXVECTOR3 vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	for ( DWORD i=0; i<dwNodeCount; i++ )
	{
		const D3DXMATRIX &matComb = ppArray[i]->matComb;
		
		D3DXVECTOR3 vNodeMax = ppArray[i]->pReplacePiece->vMax;
		D3DXVECTOR3 vNodeMin = ppArray[i]->pReplacePiece->vMin;
		COLLISION::TransformAABB ( vNodeMax, vNodeMin, matComb );

		if ( vMax.x < vNodeMax.x )	vMax.x = vNodeMax.x;
		if ( vMax.y < vNodeMax.y )	vMax.y = vNodeMax.y;
		if ( vMax.z < vNodeMax.z )	vMax.z = vNodeMax.z;

		if ( vMin.x > vNodeMin.x )	vMin.x = vNodeMin.x;
		if ( vMin.y > vNodeMin.y )	vMin.y = vNodeMin.y;
		if ( vMin.z > vNodeMin.z )	vMin.z = vNodeMin.z;

		// Note : Get    Center And Length*Length
		ppArray[i]->m_vCenter = (vNodeMax+vNodeMin) * 0.5f;
		vNodeMax = vNodeMax - ppArray[i]->m_vCenter;
		ppArray[i]->m_fLengthPOW2 = (vNodeMax.x*vNodeMax.x) + (vNodeMax.y*vNodeMax.y) + (vNodeMax.z*vNodeMax.z);

		// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
		ppArray[i]->CheckPointLight();
	}

	pTreeHead = new DXPIECEOBJNODE;
	pTreeHead->vMax = vMax;
	pTreeHead->vMin = vMin;
	MakeAABBTree ( ppArray, dwNodeCount, pTreeHead, vMax, vMin );

	SAFE_DELETE_ARRAY(ppArray);
}

//	Note : -------------------------------------- struct DXREPLACEPIECE ---------------------
DXREPLACEPIECE::~DXREPLACEPIECE ()
{
	CleanUp ();
}

HRESULT DXREPLACEPIECE::Load ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr=S_OK;

	CleanUp ();

	if( !szFile )	return E_FAIL;

	int nStrLen = strlen(szFile)+1;
	DXREPLACEPIECE::szFileName = new char[nStrLen];
	StringCchCopy( DXREPLACEPIECE::szFileName, nStrLen, szFile );

	char szFullName[MAX_PATH] = "";
	StringCchCopy( szFullName, MAX_PATH, DxReplaceContainer::GetInstance().GetPath() );
	StringCchCat( szFullName, MAX_PATH, szFile );

	CSerialFile SFile;
	if ( SFile.OpenFile ( FOT_READ, szFullName ) )
	{
		BOOL bExist = FALSE;

		SFile >> DXREPLACEPIECE::vMax;
		SFile >> DXREPLACEPIECE::vMin;
		
		SFile >> bExist;
		if ( bExist )
		{
			DXREPLACEPIECE::pFrameRoot = new DxFrame;
			DXREPLACEPIECE::pFrameRoot->LoadFile ( SFile, pd3dDevice, DXREPLACEPIECE::pEffectHead, DXREPLACEPIECE::pAphaMapFrameHead );
		}

		SFile >> bExist;
		if ( bExist )
		{
			DXREPLACEPIECE::pAniManHead = new DxAnimationMan;
			DXREPLACEPIECE::pAniManHead->LoadFile ( SFile, pd3dDevice );
		}

		SFile >> DXREPLACEPIECE::bRendAni;

		DWORD dwMaxCount = 0;
		SFile >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = DxLandMan::EffectLoadToList ( pd3dDevice, SFile, DXREPLACEPIECE::pGlobalEffectHead );
			if ( FAILED(hr) )
				MessageBox ( NULL, "Effect Load .... ing.. Error", "ERROR", MB_OK );
		}

		SFile >> bExist;
		if ( bExist )
		{
			while ( bExist )
			{
				PLANDEFF pLandEff = new DXLANDEFF;
				pLandEff->Load ( SFile, pd3dDevice );
				DXREPLACEPIECE::AddLandEff ( pLandEff );
				
				bExist = FALSE;
				SFile >> bExist;
			}
		}

		SFile >> bExist;
		if ( bExist )
		{
			DXREPLACEPIECE::m_pStaticMesh = new DxStaticMesh;
			DXREPLACEPIECE::m_pStaticMesh->Load( pd3dDevice, SFile, FALSE, TRUE );

			D3DXVECTOR3* pPos;
			pPos = &DXREPLACEPIECE::m_pStaticMesh->GetAABBMax();
			if( DXREPLACEPIECE::vMax.x < pPos->x )	DXREPLACEPIECE::vMax.x = pPos->x;
			if( DXREPLACEPIECE::vMax.y < pPos->y )	DXREPLACEPIECE::vMax.y = pPos->y;
			if( DXREPLACEPIECE::vMax.z < pPos->z )	DXREPLACEPIECE::vMax.z = pPos->z;

			pPos = &DXREPLACEPIECE::m_pStaticMesh->GetAABBMin();
			if( DXREPLACEPIECE::vMin.x > pPos->x )	DXREPLACEPIECE::vMin.x = pPos->x;
			if( DXREPLACEPIECE::vMin.y > pPos->y )	DXREPLACEPIECE::vMin.y = pPos->y;
			if( DXREPLACEPIECE::vMin.y > pPos->z )	DXREPLACEPIECE::vMin.z = pPos->z;
		}
	}
	else
	{
		CDebugSet::ToLogFile( "Don`t Exist : %s", szFile );
	}

	return S_OK;
}

HRESULT DXREPLACEPIECE::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->InitDeviceObjects ( pd3dDevice );

		pCur = pCur->m_pNext;
	}

	return S_OK;
}

HRESULT DXREPLACEPIECE::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectBase *pEffectCur;

	//	Note : Eff to Frame
	pEffectCur = pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT DXREPLACEPIECE::InvalidateDeviceObjects ()
{
	DxEffectBase *pEffectCur;

	//	Note :  Eff to Frame
	pEffectCur = pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	pEffectCur = pGlobalEffectHead;
	while ( pEffectCur )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects ();

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT DXREPLACEPIECE::DeleteDeviceObjects ()
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->DeleteDeviceObjects ();
	
		pCur = pCur->m_pNext;
	}

	SAFE_DELETE_ARRAY(szFileName);

	SAFE_DELETE(pFrameRoot);
	SAFE_DELETE(pAniManHead);

	SAFE_DELETE(m_pStaticMesh);

	SAFE_DELETE(m_pLandEffList);
	SAFE_DELETE(pEffectHead);
	SAFE_DELETE(pGlobalEffectHead);

	SAFE_DELETE(pNext);

	return S_OK;
}

HRESULT DXREPLACEPIECE::SetCurTime ( float fCurTime )
{
	return S_OK;
}

HRESULT DXREPLACEPIECE::FrameMove ( float fTime, float fETime )
{
	DxAnimationMan*	pAniManCur = pAniManHead;
	while ( pAniManCur )
	{
		pAniManCur->SetCurTime ( fTime );

		pAniManCur = pAniManCur->m_pNext;
	}

	DXLANDEFF::FrameMove ( fTime, fETime );

	return S_OK;
}

void DXREPLACEPIECE::DrawFrame( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	//	Note : Base Eff
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectPrev;
	while ( pEffectCur )
	{
		pEffectCur->Render ( pframeCur, pd3dDevice );
		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	if ( pframeCur->pEffect )
	{
		// Replace Eff
		pEffectCur = pframeCur->pEffect;
		while ( pEffectCur )
		{
			pEffectCur->Render ( pframeCur, pd3dDevice );
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}
	else
	{
		pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );

		if ( pframeCur->pmsMeshs != NULL )
		{
			DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					pmsMeshs->RenderOctree( pd3dDevice, fAlpha );
				}
				pmsMeshs = pmsMeshs->pMeshNext;
			}
		}
	}

	//	Note : Render After Eff
	pEffectCur = pframeCur->pEffectNext;
	while ( pEffectCur )
	{
		pEffectCur->Render ( pframeCur, pd3dDevice );
		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	if ( pframeCur->pframeSibling )			DrawFrame ( pframeCur->pframeSibling, pd3dDevice, fAlpha );
	if ( pframeCur->pframeFirstChild )		DrawFrame ( pframeCur->pframeFirstChild, pd3dDevice, fAlpha );
}

void DXREPLACEPIECE::DrawAlpha ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	if ( !pframeCur->pEffect )
	{
		pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );

		if ( pframeCur->pmsMeshs != NULL )
		{
			DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					pmsMeshs->RenderOctreeAlphaMap( pd3dDevice, FALSE, fAlpha );
				}
				pmsMeshs = pmsMeshs->pMeshNext;
			}
		}
	}

	if ( pframeCur->pframeSibling )			DrawAlpha ( pframeCur->pframeSibling, pd3dDevice, fAlpha );
	if ( pframeCur->pframeFirstChild )		DrawAlpha ( pframeCur->pframeFirstChild, pd3dDevice, fAlpha );
}

HRESULT DXREPLACEPIECE::UpdateFrames ( DxFrame *pframeCur, D3DXMATRIX &matCur )
{
    HRESULT hr = S_OK;
    
	//	Note : pframeCur Matrix Caculation..      matCombined = matRot * matParent
	D3DXMatrixMultiply ( &pframeCur->matCombined, &pframeCur->matRot, &matCur );

	if ( pframeCur->pframeSibling )			UpdateFrames ( pframeCur->pframeSibling, matCur );
	if ( pframeCur->pframeFirstChild )		UpdateFrames ( pframeCur->pframeFirstChild, pframeCur->matCombined );

    return S_OK;
}

HRESULT DXREPLACEPIECE::RenderAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow, float fAlpha )
{
	DrawAlpha ( pFrameRoot, pd3dDevice, fAlpha );

	return S_OK;
}

HRESULT DXREPLACEPIECE::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pMatWorld, CLIPVOLUME* pCV, float fAlpha )
{
	GASSERT(pd3dDevice);
	GASSERT(pMatWorld);

	if ( pFrameRoot )
	{
		UpdateFrames ( pFrameRoot, *pMatWorld );
		DrawFrame( pFrameRoot, pd3dDevice, fAlpha );
	}
	else if ( m_pStaticMesh )
	{
		if( pCV )
		{
			pd3dDevice->SetTransform ( D3DTS_WORLD, pMatWorld );

			CLIPVOLUME sCV;
			D3DXClipVolumeTransform ( sCV, (*pCV), (*pMatWorld) );

			m_pStaticMesh->Render_DYNAMIC_( pd3dDevice, sCV, NULL );
			m_pStaticMesh->Render_DYNAMIC_Alpha( pd3dDevice, sCV, NULL );
		}
		else
		{
			pd3dDevice->SetTransform ( D3DTS_WORLD, pMatWorld );
			m_pStaticMesh->Render_PIECE_( pd3dDevice, NULL );
			m_pStaticMesh->Render_PIECE_Alpha( pd3dDevice, NULL );
		}
	}

	DxAnimationMan*	pAniManCur = pAniManHead;
	while ( pAniManCur )
	{
		pAniManCur->SetWorldMatrix ( *pMatWorld );
		pAniManCur->Render( pd3dDevice, fAlpha );

		pAniManCur = pAniManCur->m_pNext;
	}

	return S_OK;
}

void DXREPLACEPIECE::Render_ALPHA ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pMatWorld, CLIPVOLUME* pCV, float fAlpha )
{
//	GASSERT(pd3dDevice);
//	GASSERT(pMatWorld);

	if ( pFrameRoot )
	{
		UpdateFrames ( pFrameRoot, *pMatWorld );
		RenderAlpha ( pd3dDevice, FALSE, fAlpha );
	}
	else if ( m_pStaticMesh )
	{
		if( pCV )
		{
			pd3dDevice->SetTransform ( D3DTS_WORLD, pMatWorld );

			CLIPVOLUME sCV;
			D3DXClipVolumeTransform ( sCV, (*pCV), (*pMatWorld) );

			m_pStaticMesh->Render_DYNAMIC_SoftAlpha( pd3dDevice, sCV, NULL );
		}
		else
		{
			pd3dDevice->SetTransform ( D3DTS_WORLD, pMatWorld );
			m_pStaticMesh->Render_PIECE_SoftAlpha( pd3dDevice, NULL );
			
		}
	}

	PDXEFFECTBASE pEffCur = pGlobalEffectHead;
	while ( pEffCur )
	{
		*pEffCur->m_pmatLocal = *pMatWorld;
		pEffCur->Render( NULL, pd3dDevice );
		pEffCur = pEffCur->pEffectNext;
	}

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		D3DXMATRIX backupMat = pCur->m_matWorld;
		D3DXMatrixMultiply( &pCur->m_matWorld, &backupMat, pMatWorld );

		pCur->Render ( pd3dDevice, pCV );

		pCur->m_matWorld = backupMat;

		pCur = pCur->m_pNext;
	}
}

void DXREPLACEPIECE::AddLandEff ( PLANDEFF pLandEff )
{
	m_dwNumLandEff++;
	pLandEff->m_pNext = m_pLandEffList;
	m_pLandEffList = pLandEff;
}

void DXPIECEOBJ::CheckPickAlpha( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, float fLength, float fElapsedTime )
{
	if( !pReplacePiece )	return;

	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();

	float fTempLength(0.f);
	D3DXVECTOR3 vDir(0.f,0.f,0.f);

	vDir = m_vCenter - vFromPt;
	fTempLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;

	if( fLength > (fTempLength - m_fLengthPOW2) )
	{
		// Note : Pick Check
		if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, vMax, vMin ) )
		{
			if( m_fAlpha > 0.4f )	m_fAlpha -= fElapsedTime*0.6f;
			else					m_fAlpha = 0.4f;

			m_bAplhaBlend = TRUE;
		}
		else if( m_bAplhaBlend )
		{
			if( m_fAlpha < 1.f ) 
			{
				m_fAlpha += fElapsedTime*0.6f;
			}
			if( m_fAlpha > 1.f )
			{
				m_bAplhaBlend = FALSE;
				m_fAlpha = 1.f;
			}
		}
	}
	else if( m_bAplhaBlend )
	{
		if( m_fAlpha < 1.f ) 
		{
			m_fAlpha += fElapsedTime*0.6f;
		}
		if( m_fAlpha > 1.f )
		{
			m_bAplhaBlend = FALSE;
			m_fAlpha = 1.f;
		}
	}
}

void DXPIECEOBJ::CheckPointLight()
{
	DxLightMan::GetInstance()->SetPiecePointIndex( m_vCenter, m_fLengthPOW2, m_vecPointIndex );
}

void DXPIECEOBJ::RenderPointLight( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Light가 꺼져 있을 경우는 체크를 하지 않는다.

	// Note : 2개만 적용시키자.
	for( DWORD i=0; i<2; ++i )
	{
		if( m_vecPointIndex.size() > i )
		{
			D3DLIGHT9* pLight = DxLightMan::GetInstance()->GetPointLight( m_vecPointIndex[i] );
			if( pLight )
			{
				pd3dDevice->SetLight( i+1, pLight );
				pd3dDevice->LightEnable( i+1, TRUE );

				continue;
			}
		}

		// Note : 안 걸리면 무조건 조명은 끈다.
		pd3dDevice->LightEnable( i+1, FALSE );
	}
}

//----------------------------------------------------------------------------------------------------------------------
//								D	x		P	i	e	c	e		Q	u	i	c	k		S	o	r	t
//----------------------------------------------------------------------------------------------------------------------
DxPieceQuickSort2::DxPieceQuickSort2() :
	m_pArrayPiece(NULL),
	m_dwArraySize(0),
	m_dwPieceCount(0)
{
}

DxPieceQuickSort2::~DxPieceQuickSort2()
{
	m_dwPieceCount = 0;
	m_dwArraySize = 0;
	SAFE_DELETE_ARRAY( m_pArrayPiece );
}

void DxPieceQuickSort2::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : RenderState
	DWORD dwAlphaBlendEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// Note : Change for Alpha 0.5f
	D3DMATERIALQ sMaterial;
	sMaterial.Diffuse.r = sMaterial.Diffuse.g = sMaterial.Diffuse.b = 1.f;
	sMaterial.Ambient.r = sMaterial.Ambient.g = sMaterial.Ambient.b = 1.f;
	sMaterial.Specular.a = sMaterial.Specular.r = sMaterial.Specular.g = sMaterial.Specular.b = 0.f;
	sMaterial.Emissive.a = sMaterial.Emissive.r = sMaterial.Emissive.g = sMaterial.Emissive.b = 0.f;

	DXPIECEOBJ* pPiece(NULL);
	for( int i=(int)(m_dwPieceCount-1); i>=0; --i )
	{
		pPiece = m_pArrayPiece[i].m_pPiece;

		// Note : Point Light를 적용시킨다.
		pPiece->RenderPointLight( pd3dDevice );

		sMaterial.Diffuse.a = sMaterial.Ambient.a = pPiece->m_fAlpha;
		pd3dDevice->SetMaterial( &sMaterial );

		pPiece->pReplacePiece->Render( pd3dDevice, &pPiece->matComb, NULL, pPiece->m_fAlpha );
		pPiece->pReplacePiece->Render_ALPHA( pd3dDevice, &pPiece->matComb, NULL, pPiece->m_fAlpha );
	}

	// Note : ReSet Material
	sMaterial.Diffuse.a = sMaterial.Ambient.a = 1.f;
	pd3dDevice->SetMaterial( &sMaterial );

	// Note : RenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
}

void DxPieceQuickSort2::SetArrayMaxSize( DWORD dwCount )
{
	if( dwCount > m_dwArraySize )
	{
		SAFE_DELETE_ARRAY( m_pArrayPiece );

		m_dwArraySize = dwCount;
		m_pArrayPiece = new PIECEDISTANCE[m_dwArraySize];
	}
}

void DxPieceQuickSort2::Reset()
{
	m_dwPieceCount = 0;
}

void DxPieceQuickSort2::InsertData( DXPIECEOBJ* pPiece, const D3DXVECTOR3& vFromPt )
{
	float fTempLength(0.f);
	D3DXVECTOR3 vDir(0.f,0.f,0.f);

	DXPIECEOBJ* pCur = pPiece;
	while( pCur )
	{
		vDir = pCur->m_vCenter - vFromPt;
		fTempLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		m_pArrayPiece[m_dwPieceCount].m_fDistance = fTempLength;
		m_pArrayPiece[m_dwPieceCount].m_pPiece = pCur;

		++m_dwPieceCount;
		pCur = pCur->pPickAlphaNext;
	}
}

void DxPieceQuickSort2::QuickSortProcess()
{
	DxQuickSort( m_pArrayPiece, 0, m_dwPieceCount-1 );
}

void DxPieceQuickSort2::DxQuickSort( PIECEDISTANCE pArray[], int ilo, int ihi ) 
{
    float pivot;	// pivot value for partitioning array
    int ulo, uhi;	// indices at ends of unpartitioned region
    int ieq;		// least index of array entry with value equal to pivot
 //   int tempEntry;	// temporary entry used for swapping

    if (ilo >= ihi) return;

    // Select a pivot value.
	pivot = pArray[(ilo + ihi)/2].m_fDistance;
    // Initialize ends of unpartitioned region and least index of entry
    // with value equal to pivot.
    ieq = ulo = ilo;
    uhi = ihi;
    // While the unpartitioned region is not empty, try to reduce its size.
    while( ulo <= uhi ) 
	{
		if( pArray[uhi].m_fDistance > pivot ) 
		{
			// Here, we can reduce the size of the unpartitioned region and
			// try again.
			uhi--;
		} 
		else 
		{
			// Here, A[uhi] <= pivot, so swap entries at indices ulo and
			// uhi.
			//tempEntry = pArray[ulo];
			//pArray[ulo] = pArray[uhi];
			//pArray[uhi] = tempEntry;

			Swap( pArray[uhi], pArray[ulo] );

			// After the swap, A[ulo] <= pivot.
			if (pArray[ulo].m_fDistance < pivot) 
			{
				// Swap entries at indices ieq and ulo.
				//tempEntry = pArray[ieq];
				//pArray[ieq] = pArray[ulo];
				//pArray[ulo] = tempEntry;
				Swap( pArray[ieq], pArray[ulo] );

				// After the swap, A[ieq] < pivot, so we need to change
				// ieq.
				ieq++;
				// We also need to change ulo, but we also need to do
				// that when A[ulo] = pivot, so we do it after this if
				// statement.
			}
			// Once again, we can reduce the size of the unpartitioned
			// region and try again.
			ulo++;
		}
    }

    // Now, all entries from index ilo to ieq - 1 are less than the pivot
    // and all entries from index uhi to ihi + 1 are greater than the
    // pivot.  So we have two regions of the array that can be sorted
    // recursively to put all of the entries in order.
    DxQuickSort( pArray, ilo, ieq - 1 );
    DxQuickSort( pArray, uhi + 1, ihi );
}


void DxPieceQuickSort2::Swap( PIECEDISTANCE& pData1, PIECEDISTANCE& pData2 )
{
	PIECEDISTANCE pTemp;
	pTemp.m_fDistance = pData1.m_fDistance;
	pTemp.m_pPiece = pData1.m_pPiece;

	pData1.m_fDistance = pData2.m_fDistance;
	pData1.m_pPiece = pData2.m_pPiece;

	pData2.m_fDistance = pTemp.m_fDistance;
	pData2.m_pPiece = pTemp.m_pPiece;
}

//	Note : -------------------------------------- class DxReplaceContainer ---------------------

DXPIECEOBJ* DxReplaceContainer::m_pPickAlpha = NULL;
DxPieceQuickSort2 DxReplaceContainer::m_sQuickSort;

DxReplaceContainer::DxReplaceContainer(void) :
	m_pd3dDevice(NULL)
{
}

DxReplaceContainer::~DxReplaceContainer(void)
{
}

DXREPLACEPIECE* DxReplaceContainer::FindPice ( const char *szFile )
{
	REPLACEMAP_ITER iter = m_mapReplace.find ( string(szFile) );
	if ( iter!=m_mapReplace.end() )	return iter->second;

	return NULL;
}

DXREPLACEPIECE* DxReplaceContainer::LoadPiece ( const char *szFile )
{
	HRESULT hr=S_OK;
	GASSERT(m_pd3dDevice);

	DXREPLACEPIECE* pPiece;
	if ( pPiece = FindPice ( szFile ) )
	{
		pPiece->nRefCount++;
		return pPiece;
	}

	DXREPLACEPIECE* pNewPiece = new DXREPLACEPIECE;
	hr = pNewPiece->Load ( szFile, m_pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pNewPiece);
		return NULL;
	}

	pNewPiece->nRefCount++;

	//	Note : Replace Map .. Insert
	m_mapReplace.insert ( std::make_pair(std::string(pNewPiece->szFileName),pNewPiece) );

	return pNewPiece;
}

void DxReplaceContainer::ReleasePiece ( char *szFile )
{
	REPLACEMAP_ITER iter = m_mapReplace.find ( string(szFile) );
	if ( iter==m_mapReplace.end() )	return;

	iter->second->nRefCount--;
	if ( iter->second->nRefCount <= 0 )
	{
		delete iter->second;
		m_mapReplace.erase ( iter );
	}

	return;
}

HRESULT DxReplaceContainer::OneTimeSceneInit ( char* szPath )
{
	StringCchCopy( szPiecePath, MAX_PATH, szPath );
	
	return S_OK;
}

HRESULT DxReplaceContainer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;

	REPLACEMAP_ITER iter = m_mapReplace.begin ();
	REPLACEMAP_ITER iter_end = m_mapReplace.end ();

	for ( iter; iter!=iter_end; iter++ )
	{
		iter->second->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxReplaceContainer::RestoreDeviceObjects ()
{
	HRESULT hr = S_OK;
	//GASSERT(m_pd3dDevice);

	REPLACEMAP_ITER iter = m_mapReplace.begin ();
	REPLACEMAP_ITER iter_end = m_mapReplace.end ();

	for ( iter; iter!=iter_end; iter++ )
	{
		iter->second->RestoreDeviceObjects ( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxReplaceContainer::InvalidateDeviceObjects ()
{
	REPLACEMAP_ITER iter = m_mapReplace.begin ();
	REPLACEMAP_ITER iter_end = m_mapReplace.end ();

	for ( iter; iter!=iter_end; iter++ )
	{
		iter->second->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxReplaceContainer::DeleteDeviceObjects ()
{
	REPLACEMAP_ITER iter = m_mapReplace.begin ();
	REPLACEMAP_ITER iter_end = m_mapReplace.end ();

	for ( iter; iter!=iter_end; iter++ )
	{
		iter->second->DeleteDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxReplaceContainer::FinalCleanup ()
{
	std::for_each ( m_mapReplace.begin(), m_mapReplace.end(), std_afunc::DeleteMapObject() );
	m_mapReplace.clear ();

	return S_OK;
}

HRESULT DxReplaceContainer::CleanUp ()
{
	InvalidateDeviceObjects ();
	DeleteDeviceObjects ();
	FinalCleanup ();

	return S_OK;
}

HRESULT DxReplaceContainer::FrameMove ( float fTime, float fETime )
{
	HRESULT hr = S_OK;

	REPLACEMAP_ITER iter = m_mapReplace.begin ();
	REPLACEMAP_ITER iter_end = m_mapReplace.end ();

	for ( iter; iter!=iter_end; iter++ )
	{
		hr = iter->second->UpdateEffect ( fTime, fETime );
		if ( FAILED(hr) )	return hr;
	}

	// Note : 
	m_sQuickSort.SetArrayMaxSize( m_mapReplace.size() );

	// Note : Init PickAlpha Point.
	if( m_pPickAlpha )	m_pPickAlpha->pPickAlphaNext = NULL;
	m_pPickAlpha = NULL;

	return S_OK;
}

float DxReplaceContainer::m_fTime = 0.0f;
float DxReplaceContainer::m_fElapsedTime = 0.0f;
BOOL DxReplaceContainer::m_bRendAABBB = FALSE;
DWORD DxReplaceContainer::m_dwRendPiece = 0;

HRESULT DxReplaceContainer::SetTime ( float fTime, float fElapsedTime )
{
	m_fTime = fTime;
	m_fElapsedTime = fElapsedTime;

	return S_OK;
}

HRESULT DxReplaceContainer::DrawRePlacePieceNode( LPDIRECT3DDEVICEQ pd3dDevice, DXPIECEOBJNODE *pNode, 
												CLIPVOLUME *pCV, EMREFLECTOPTION emReflect, float fDisPOW2 )
{
	if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )	return S_OK;

	if ( m_bRendAABBB )		EDITMESHS::RENDERAABB ( pd3dDevice, pNode->vMax, pNode->vMin );	// Test AABB Render

	if ( pNode->pPieceObj )
	{
		PROFILE_BEGIN("DrawRePlacePieceNode");

		DXPIECEOBJ* pPieceObjCur = pNode->pPieceObj;
		switch( emReflect )
		{
		case EM_NULL:
			pPieceObjCur->CheckPickAlpha( pNode->vMax, pNode->vMin, fDisPOW2, m_fElapsedTime );				// Pick Alpha Check
			pPieceObjCur->pReplacePiece->FrameMove ( m_fTime, m_fElapsedTime );

			// Note : AlphaBlending ,..do..  Char After Render 
			if( pNode->pPieceObj->m_bAplhaBlend )
			{
				pNode->pPieceObj->pPickAlphaNext = m_pPickAlpha;
				m_pPickAlpha = pNode->pPieceObj;

				m_dwRendPiece++;

				PROFILE_END("DrawRePlacePieceNode");
				return S_OK;
			}
			break;
		};

		DWORD	dwCullMode;
		D3DXMATRIX matWorld, matOldWorld;
		matWorld = pPieceObjCur->matComb;
		switch( emReflect )
		{
		case EM_REFLECT:
			D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

			matWorld = pPieceObjCur->matComb;
			matOldWorld = pPieceObjCur->matComb;

			matWorld._42 = matWorld._42 - (vCenter.y*2.f);

			D3DXMatrixMultiply_MIRROR( matWorld );
			
			pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
			break;
		};

		// Note : Point Light를 적용시킨다.
		pPieceObjCur->RenderPointLight( pd3dDevice );

		pPieceObjCur->pReplacePiece->Render ( pd3dDevice, &matWorld, pCV, pPieceObjCur->m_fAlpha );

		switch( emReflect )
		{
		case EM_REFLECT:
			pPieceObjCur->matComb = matOldWorld;
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
			break;
		};

		m_dwRendPiece++;

		PROFILE_END("DrawRePlacePieceNode");

		return S_OK;
	}

	DrawRePlacePieceNode ( pd3dDevice, pNode->pLeftChild, pCV, emReflect, fDisPOW2 );
	DrawRePlacePieceNode ( pd3dDevice, pNode->pRightChild, pCV, emReflect, fDisPOW2 );

	return S_OK;
}

void DxReplaceContainer::DrawRePlacePieceNode_ALPHA( LPDIRECT3DDEVICEQ pd3dDevice, DXPIECEOBJNODE *pNode, CLIPVOLUME *pCV, EMREFLECTOPTION emReflect )
{
	if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )	return;

	if ( pNode->pPieceObj )
	{
		PROFILE_BEGIN("DrawRePlacePieceNode_ALPHA");

		DXPIECEOBJ* pPieceObjCur = pNode->pPieceObj;
		switch( emReflect )
		{
		case EM_NULL:
			if( pPieceObjCur->m_bAplhaBlend )
			{
				PROFILE_END("DrawRePlacePieceNode_ALPHA");
				return;
			}
			break;
		};

		DWORD	dwCullMode;
		D3DXMATRIX matWorld, matOldWorld;
		matWorld = pPieceObjCur->matComb;
		switch( emReflect )
		{
		case EM_REFLECT:
			D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

			matWorld = pPieceObjCur->matComb;
			matOldWorld = pPieceObjCur->matComb;

			matWorld._42 = matWorld._42 - (vCenter.y*2.f);

			D3DXMatrixMultiply_MIRROR( matWorld );
			
			pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
			break;
		};

		// Note : Point Light를 적용시킨다.
		pPieceObjCur->RenderPointLight( pd3dDevice );

		//	Note : 
		pPieceObjCur->pReplacePiece->Render_ALPHA( pd3dDevice, &matWorld, pCV, 1.f );

		switch( emReflect )
		{
		case EM_REFLECT:
			pPieceObjCur->matComb = matOldWorld;
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
			break;
		};

		PROFILE_END("DrawRePlacePieceNode_ALPHA");

		return;
	}

	DrawRePlacePieceNode_ALPHA( pd3dDevice, pNode->pLeftChild, pCV, emReflect );
	DrawRePlacePieceNode_ALPHA( pd3dDevice, pNode->pRightChild, pCV, emReflect );

	return;
}

// Note : Render of ( Octree Mode And Frame Mesh )
HRESULT DxReplaceContainer::DrawRePlacePiece ( LPDIRECT3DDEVICEQ pd3dDevice, DXPIECEOBJ *pObjList, DXPIECEOBJNODE *pObjTree, 
												CLIPVOLUME *pCV, EMREFLECTOPTION emReflect, float fDisPOW2 )
{
	m_dwRendPiece = 0;

	// Note : pObjTree==TRUE 일 경우 Octree Mode
	//			FALSE 일 경우 Frame Mesh
	if ( pObjTree )
	{
		DrawRePlacePieceNode( pd3dDevice, pObjTree, pCV, emReflect, fDisPOW2 );
		DrawRePlacePieceNode_ALPHA( pd3dDevice, pObjTree, pCV, emReflect );
	}
	else
	{
		DXPIECEOBJ* pPieceObjCur = pObjList;
		while ( pPieceObjCur )
		{
			pPieceObjCur->pReplacePiece->FrameMove ( m_fTime, m_fElapsedTime );
			
			DWORD	dwCullMode;
			D3DXMATRIX matWorld, matOldWorld;
			matWorld = pPieceObjCur->matComb;

			switch( emReflect )
			{
			case EM_REFLECT_OLD:
				D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

				matWorld = pPieceObjCur->matComb;
				matOldWorld = pPieceObjCur->matComb;

				matWorld._42 = matWorld._42 - (vCenter.y*2.f);

				D3DXMatrixMultiply_MIRROR( matWorld );
				
				pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
				pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
				break;
			};

			// Note : Point Light를 적용시킨다.
			pPieceObjCur->RenderPointLight( pd3dDevice );

			//	Note : Render
			pPieceObjCur->pReplacePiece->Render( pd3dDevice, &matWorld, pCV, 1.f );
			pPieceObjCur->pReplacePiece->Render_ALPHA( pd3dDevice, &matWorld, pCV, 1.f );

			switch( emReflect )
			{
			case EM_REFLECT_OLD:
				pPieceObjCur->matComb = matOldWorld;
				pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
				break;
			};

			pPieceObjCur = pPieceObjCur->pNext;
		}
	}

	//CDebugSet::ToView ( 2, "Piece Render Numder: %d", m_dwRendPiece );
	return S_OK;
}

// Note : Octree Mode ... No Alpha Render
void DxReplaceContainer::DrawRePlacePiece( LPDIRECT3DDEVICEQ pd3dDevice, DXPIECEOBJNODE *pObjTree, CLIPVOLUME *pCV, EMREFLECTOPTION emReflect, float fDisPOW2 )
{
	m_pPickAlpha = NULL;
	m_dwRendPiece = 0;
	if( pObjTree )	DrawRePlacePieceNode( pd3dDevice, pObjTree, pCV, emReflect, fDisPOW2 );
}

// Note : Octree Mode ... Alpha Render
void DxReplaceContainer::DrawRePlacePiece_ALPHA( LPDIRECT3DDEVICEQ pd3dDevice, DXPIECEOBJNODE *pObjTree, CLIPVOLUME *pCV, EMREFLECTOPTION emReflect )
{
	if( pObjTree )	DrawRePlacePieceNode_ALPHA( pd3dDevice, pObjTree, pCV, emReflect );
}

void DxReplaceContainer::DrawRePlacePiecePickAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();

	m_sQuickSort.Reset();
	m_sQuickSort.InsertData( m_pPickAlpha, vFromPt );
	m_sQuickSort.QuickSortProcess();

	if( !m_pPickAlpha )	return;

	m_sQuickSort.RenderPickAlpha( pd3dDevice );

	m_pPickAlpha->pPickAlphaNext = NULL;
	m_pPickAlpha = NULL;
}



