#include "pch.h"

#include "./DxRenderStates.h"
#include "./DxCubeMap.h"
#include "./DxFrameMesh.h"

#include "./DxTexEffDiffuse.h"
#include "./DxTexEffFlowUV.h"
#include "./DxTexEffRotate.h"
#include "./DxTexEffSpecular.h"

#include "./DxTextureEffMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DxTextureEffMan::FrameMove( float fElapsedTime )
{
	MAPTEXEFF_ITER iter = m_mapTexEff.begin();
	for( ; iter!=m_mapTexEff.end(); ++iter )
	{
		DxTexEffBase* pEff = (*iter).second;
		while( pEff )
		{
			switch( pEff->GetTypeID() )
			{
			case DEF_TEXEFF_DIFFUSE:
				((DxTexEffDiffuse*)pEff)->FrameMove( fElapsedTime );
				break;
			case DEF_TEXEFF_FLOWUV:
				((DxTexEffFlowUV*)pEff)->FrameMove( fElapsedTime );
				break;
			case DEF_TEXEFF_ROTATE:
				((DxTexEffRotate*)pEff)->FrameMove( fElapsedTime );
				break;
			//case DEF_TEXEFF_SPECULAR:	// 할 필요가 없다. 여기서 하는 것이 없기 때문
			//	((DxTexEffSpecular*)pEff)->FrameMove( fElapsedTime );
			//	break;
			};
				
			pEff = pEff->m_pNext;
		}
	}
}

void DxTextureEffMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs )
{
	LPD3DXMESH	pLocalMesh = pMeshs->m_pLocalMesh;
	if( !pLocalMesh )	return;

	if( pMeshs->m_pAttrib )
	{
		DWORD	dwAttribID = 0;
		for ( DWORD i=0; i<pMeshs->m_dwAttrib; ++i )
		{
			dwAttribID = pMeshs->m_pAttrib[i].AttribId;

			if ( pMeshs->exMaterials[dwAttribID].MtrlType != MATERIAL_NORMAL )		continue;

			//	Note : 메터리얼, 텍스쳐 지정.
			pd3dDevice->SetMaterial( &pMeshs->rgMaterials[dwAttribID] );
			pd3dDevice->SetTexture( 0, pMeshs->pTextures[dwAttribID] );

			// Note : 기본
			MAPTEXEFF_ITER iter = m_mapTexEff.find( pMeshs->strTextureFiles[dwAttribID].GetString() );
			if( iter==m_mapTexEff.end() )
			{
				pLocalMesh->DrawSubset( dwAttribID );
				continue;
			}

			// Note : Base Render
			pLocalMesh->DrawSubset( dwAttribID );

			// Note : 효과 렌더
			DxTexEffBase* pEff = (*iter).second;
			while( pEff )
			{
				switch( pEff->GetTypeID() )
				{
				case DEF_TEXEFF_DIFFUSE:
					((DxTexEffDiffuse*)pEff)->Render( pd3dDevice, pLocalMesh, dwAttribID );
					break;
				case DEF_TEXEFF_FLOWUV:
					((DxTexEffFlowUV*)pEff)->Render( pd3dDevice, pMeshs->m_pEffVB, pMeshs->m_pEffIB, pMeshs->m_pAttrib, 
													i, pMeshs->m_pVertSrc, pMeshs->m_dwVert );
					break;
				case DEF_TEXEFF_ROTATE:
					((DxTexEffRotate*)pEff)->Render( pd3dDevice, pMeshs->m_pEffVB, pMeshs->m_pEffIB, pMeshs->m_pAttrib, 
													i, pMeshs->m_pVertSrc, pMeshs->m_dwVert );
					break;
				case DEF_TEXEFF_SPECULAR:
					((DxTexEffSpecular*)pEff)->Render( pd3dDevice, pLocalMesh, dwAttribID );
					break;
				};
					
				pEff = pEff->m_pNext;
			}
		}

	}
	else
	{
		for ( DWORD i=0; i<pMeshs->cMaterials; ++i )
		{
			if ( pMeshs->exMaterials[i].MtrlType != MATERIAL_NORMAL )		continue;

			//	Note : 메터리얼, 텍스쳐 지정.
			pd3dDevice->SetMaterial( &pMeshs->rgMaterials[i] );
			pd3dDevice->SetTexture( 0, pMeshs->pTextures[i] );

			// Note : 기본
			MAPTEXEFF_ITER iter = m_mapTexEff.find( pMeshs->strTextureFiles[i].GetString() );
			if( iter==m_mapTexEff.end() )
			{
				pLocalMesh->DrawSubset( i );
				continue;
			}

			// Note : Base Render
			pLocalMesh->DrawSubset( i );

			// Note : 효과 렌더
			DxTexEffBase* pEff = (*iter).second;
			while( pEff )
			{
				switch( pEff->GetTypeID() )
				{
				case DEF_TEXEFF_DIFFUSE:
					((DxTexEffDiffuse*)pEff)->Render( pd3dDevice, pLocalMesh, i );
					break;
				case DEF_TEXEFF_FLOWUV:
					if( !pMeshs->m_pEffVB )	pMeshs->MakeEffectMeshs( pd3dDevice, VERTEX::FVF );
					break;
				case DEF_TEXEFF_ROTATE:
					if( !pMeshs->m_pEffVB )	pMeshs->MakeEffectMeshs( pd3dDevice, VERTEX::FVF );
					break;
				case DEF_TEXEFF_SPECULAR:
					((DxTexEffSpecular*)pEff)->Render( pd3dDevice, pLocalMesh, i );
					break;
				};
					
				pEff = pEff->m_pNext;
			}
		}
	}

	//DWORD	dwAttribID = 0;
	//for ( DWORD i=0; i<pMeshs->m_dwAttrib; ++i )
	//{
	//	dwAttribID = pMeshs->m_pAttrib[i].AttribId;

	//	if ( pMeshs->exMaterials[dwAttribID].MtrlType != MATERIAL_NORMAL )		continue;

	//	//	Note : 메터리얼, 텍스쳐 지정.
	//	pd3dDevice->SetMaterial( &pMeshs->rgMaterials[dwAttribID] );
	//	pd3dDevice->SetTexture( 0, pMeshs->pTextures[dwAttribID] );

	//	// Note : 기본
	//	MAPTEXEFF_ITER iter = m_mapTexEff.find( pMeshs->strTextureFiles[dwAttribID].GetString(MAX_PATH) );
	//	if( iter==m_mapTexEff.end() )
	//	{
	//		pLocalMesh->DrawSubset( dwAttribID );
	//		continue;
	//	}

	//	// Note : Base Render
	//	pLocalMesh->DrawSubset( dwAttribID );

	//	// Note : 효과 렌더
	//	DxTexEffBase* pEff = (*iter).second;
	//	while( pEff )
	//	{
	//		switch( pEff->GetTypeID() )
	//		{
	//		case DEF_TEXEFF_DIFFUSE:
	//			((DxTexEffDiffuse*)pEff)->Render( pd3dDevice, pLocalMesh, dwAttribID );
	//			break;
	//		case DEF_TEXEFF_FLOWUV:
	//			// Note : EFFECT MESH가 없다면 생성한다.
	//			if( !pMeshs->m_pEffVB )	pMeshs->MakeEffectMeshs( pd3dDevice, VERTEX::FVF );

	//			// Note : Eff Render
	//			((DxTexEffFlowUV*)pEff)->Render( pd3dDevice, pMeshs->m_pEffVB, pMeshs->m_pEffIB, pMeshs->m_pAttrib, 
	//											i, pMeshs->m_pVertSrc, pMeshs->m_dwVert );
	//			break;
	//		};
	//			
	//		pEff = pEff->m_pNext;
	//	}

	//	//// Note : Texture Eff
	//	//DWORD dwFlag = (*iter).second.dwFlag;
	//	//if( dwFlag&TEXEFF_EFF )
	//	//{
	//	//	if( dwFlag&TEXEFF_SPECULAR )
	//	//	{
	//	//		RenderSpecular( pd3dDevice, pLocalMesh, i, &((*iter).second) );
	//	//	}
	//	//	if( dwFlag&TEXEFF_REFLECT )
	//	//	{
	//	//		RenderReflect( pd3dDevice, pLocalMesh, i, &((*iter).second) );
	//	//	}
	//	//	if( dwFlag&TEXEFF_BUMP )
	//	//	{
	//	//		RenderBump( pd3dDevice, pLocalMesh, i, &((*iter).second) );
	//	//	}
	//	//	if( dwFlag&TEXEFF_GLOW )
	//	//	{
	//	//		RenderGlow( pd3dDevice, pLocalMesh, i, &((*iter).second) );
	//	//	}
	//	//	if( dwFlag&TEXEFF_DIFFUSE )
	//	//	{
	//	//		m_sEffDiffuse.Render( pd3dDevice, pLocalMesh, i, &((*iter).second.sDiffuse), fElapsedTime );
	//	//	}
	//	//	if( dwFlag&TEXEFF_MULTITEX )
	//	//	{
	//	//		// Note : EFFECT MESH가 없다면 생성한다.
	//	//		if( !pMeshs->m_pEffectMesh )	MakeEffectMesh( pd3dDevice, pMeshs );

	//	//		// Note : Base Render
	//	//		pLocalMesh->DrawSubset( i );

	//	//		// Note : Eff Render
	//	//		RenderMultiTex( pd3dDevice, pMeshs->m_pEffectMesh, i, &((*iter).second) );
	//	//	}
	//	//}
	//	//else
	//	//{
	//	//	pLocalMesh->DrawSubset( i );
	//	//}
	//}
}

void DxTextureEffMan::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const BOOL bShadow )
{
	LPD3DXMESH	pLocalMesh = pMeshs->m_pLocalMesh;
	if( !pLocalMesh )	return;

	for ( DWORD i=0; i<pMeshs->cMaterials; i++ )
	{
		DXMTRLTYPE emMETR = pMeshs->exMaterials[i].MtrlType;
		if ( pMeshs->exMaterials[i].MtrlType==MATERIAL_NORMAL )			continue;

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetMaterial( &pMeshs->rgMaterials[i] );
		pd3dDevice->SetTexture( 0, pMeshs->pTextures[i] );

		// Note : 기본
		MAPTEXEFF_ITER iter = m_mapTexEff.find( pMeshs->strTextureFiles[i].GetString() );
		if( iter==m_mapTexEff.end() )
		{
			RenderAlpha( pd3dDevice, pLocalMesh, i, emMETR, bShadow );
			return;
		}

		RenderAlpha( pd3dDevice, pLocalMesh, i, emMETR, bShadow );

		// Note : 효과 렌더
		DxTexEffBase* pEff = (*iter).second;
		while( pEff )
		{
			switch( pEff->GetTypeID() )
			{
			case DEF_TEXEFF_DIFFUSE:
				((DxTexEffDiffuse*)pEff)->Render( pd3dDevice, pLocalMesh, i );
				break;
			case DEF_TEXEFF_SPECULAR:
				((DxTexEffSpecular*)pEff)->Render( pd3dDevice, pLocalMesh, i );
				break;
			};
				
			pEff = pEff->m_pNext;
		}

		//// Note : Texture Eff
		//DWORD dwFlag = (*iter).second.dwFlag;
		//if( dwFlag&TEXEFF_EFF )
		//{
		//	if( dwFlag&TEXEFF_SPECULAR )
		//	{
		//		RenderSpecular( pd3dDevice, pLocalMesh, i, &((*iter).second) );
		//	}
		//	if( dwFlag&TEXEFF_REFLECT )
		//	{
		//		RenderReflect( pd3dDevice, pLocalMesh, i, &((*iter).second) );
		//	}
		//	if( dwFlag&TEXEFF_BUMP )
		//	{
		//		RenderBump( pd3dDevice, pLocalMesh, i, &((*iter).second) );
		//	}
		//	if( dwFlag&TEXEFF_GLOW )
		//	{
		//		RenderGlow( pd3dDevice, pLocalMesh, i, &((*iter).second) );
		//	}
		//	if( dwFlag&TEXEFF_DIFFUSE )
		//	{
		//		m_sEffDiffuse.Render( pd3dDevice, pLocalMesh, i, &((*iter).second.sDiffuse), fElapsedTime );
		//	}
		//	if( dwFlag&TEXEFF_MULTITEX )
		//	{
		//		// Note : EFFECT MESH가 없다면 생성한다.
		//		if( !pMeshs->m_pEffectMesh )	MakeEffectMesh( pd3dDevice, pMeshs );

		//		// Note : Base Render
		//		pLocalMesh->DrawSubset( i );

		//		// Note : Eff Render
		//		RenderMultiTex( pd3dDevice, pMeshs->m_pEffectMesh, i, &((*iter).second) );
		//	}
		//}
		//else
		//{
		//	RenderAlpha( pd3dDevice, pLocalMesh, i, emMETR, bShadow );
		//}
	}
}

void DxTextureEffMan::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh, const DWORD dwAttrib, const int nMETR, const BOOL bShadow )
{
	if ( !bShadow )
	{
		switch ( nMETR )
		{
		case MATERIAL_ALPHA:
			DxRenderStates::GetInstance().SetAlphaLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOZWRI:
			DxRenderStates::GetInstance().SetAlphaNoZWriLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOTEST:
			DxRenderStates::GetInstance().SetAlphaNoTestLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_HARD:
			DxRenderStates::GetInstance().SetOnAlphaMap( pd3dDevice );
			break;
		case MATERIAL_ALPHA_SOFT:
			DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
			break;
		};
	}

	pLocalMesh->DrawSubset( dwAttrib );

	if ( !bShadow )
	{
		switch ( nMETR )
		{
		case MATERIAL_ALPHA:
			DxRenderStates::GetInstance().ReSetAlphaLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOZWRI:
			DxRenderStates::GetInstance().ReSetAlphaNoZWriLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_NOTEST:
			DxRenderStates::GetInstance().ReSetAlphaNoTestLand( pd3dDevice );
			break;
		case MATERIAL_ALPHA_HARD:
			DxRenderStates::GetInstance().ReSetOnAlphaMap( pd3dDevice );
			break;
		case MATERIAL_ALPHA_SOFT:
			DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
			break;
		};
	}
}

//void DxTextureEffMan::RenderSpecular( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//
//	// Note : GetTexture
//	if( !pProp->pSpecularTex )
//	{
//		TextureManager::GetTexture( pProp->strSpecularTex.c_str(), pProp->pSpecularTex );
//		if( !pProp->pSpecularTex )	return;
//	}
//
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pSpecularTex );
//	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );
//
//	
//	m_pSpecularSavedSB->Capture();
//	m_pSpecularDrawSB->Apply();
//	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffffffff );
//
//	// Note : Specular Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_pSpecularSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//
//	// Note : GetTexture
//	if( !pProp->pTex )
//	{
//		TextureManager::GetTexture( pProp->szTex, pProp->pTex );
//		if( !pProp->pTex )	return;
//	}
//	
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pTex );
//	pd3dDevice->SetTexture( 1, pProp->pCubeTex );
//
//	m_pReflectSavedSB->Capture();
//	m_pReflectDrawSB->Apply();
//	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, pProp->dwColor );
//
//	// Note : Reflect Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_pReflectSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::RenderBump( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//}
//
//void DxTextureEffMan::RenderGlow( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : Base Render
//	pMesh->DrawSubset( dwAttrib );
//}
//
//void DxTextureEffMan::RenderMultiTex( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : GetTexture
//	if( !pProp->pMultiBaseTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiBaseTex.c_str(), pProp->pMultiBaseTex );
//		if( !pProp->pMultiBaseTex )	return;
//	}
//	if( !pProp->pMultiFlowTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiFlowTex.c_str(), pProp->pMultiFlowTex );
//		if( !pProp->pMultiFlowTex )	return;
//	}
//
//	// Note : 텍스쳐를 흘린다.
//	if( pProp->bfMultiTexNEWUV )
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pProp->fMultiTexUVScale, pProp->fMultiTexAddUV, pMesh );
//	}
//	else
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pMesh );
//	}
//
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pMultiBaseTex );
//	pd3dDevice->SetTexture( 1, pProp->pMultiFlowTex  );
//
//	
//	m_pMultiTexSavedSB->Capture();
//	m_pMultiTexDrawSB->Apply();
//
//	// Note : Specular Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_pMultiTexSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::FrameMoveMultiTex( float fBaseSpeed, LPD3DXMESH pMesh )
//{
//	float fSpeed = fBaseSpeed * 0.01f;
//
//	struct VERTEXTEX2 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1, vTex2; };
//
//	VERTEXTEX2* pVert;
//	pMesh->LockVertexBuffer( 0L, (VOID**)&pVert );
//	for( DWORD i=0; i<pMesh->GetNumVertices(); ++i )
//	{
//		pVert[i].vTex2 += D3DXVECTOR2( fSpeed, fSpeed );
//	}
//	pMesh->UnlockVertexBuffer();
//}
//
//void DxTextureEffMan::FrameMoveMultiTex( float fBaseSpeed, float UVScale, float& UVAdd, LPD3DXMESH pMesh )
//{
//	UVAdd += fBaseSpeed * 0.01f;
//	float fScale = UVScale * 0.01f;
//
//	struct VERTEXTEX2 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1, vTex2; };
//
//	VERTEXTEX2* pVert;
//	pMesh->LockVertexBuffer( 0L, (VOID**)&pVert );
//	for( DWORD i=0; i<pMesh->GetNumVertices(); ++i )
//	{
//		pVert[i].vTex2.x = pVert[i].vPos.x*fScale + UVAdd;
//		pVert[i].vTex2.y = pVert[i].vPos.z*fScale + UVAdd;
//	}
//	pMesh->UnlockVertexBuffer();
//}

void DxTextureEffMan::MakeEffectMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs )
{
	//if( pMeshs->m_pEffVB )	return;

	//pMeshs->MakeEffectMeshs( pd3dDevice, D3DFVF_XYZ|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE1(2) );

	//struct VERTEXTEX1 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1; };
	//struct VERTEXTEX2 { D3DXVECTOR3 vPos; D3DXVECTOR2 vTex1, vTex2; float fTex3; };

	//VERTEXTEX1* pSrc;
	//VERTEXTEX2* pDest;
	//pMeshs->m_pLocalMesh->LockVertexBuffer( 0L, (VOID**)&pSrc );
	//pMeshs->m_pEffectMesh->LockVertexBuffer( 0L, (VOID**)&pDest );
	//for( DWORD i=0; i<pMeshs->m_pLocalMesh->GetNumVertices(); ++i )
	//{
	//	pDest[i].vPos =  pSrc[i].vPos;
	//	pDest[i].vTex1 =  pSrc[i].vTex1;
	//	pDest[i].vTex2 =  pSrc[i].vTex1;
	//	pDest[i].fTex3 = 0.f;
	//}
	//pMeshs->m_pEffectMesh->UnlockVertexBuffer();
	//pMeshs->m_pLocalMesh->UnlockVertexBuffer();


	//if( !pMeshs->m_pLocalMesh )		return;

	//{
	//	m_dwVert = m_pLocalMesh->GetNumVertices();

	//	SAFE_DELETE_ARRAY( m_pVertSrc );
	//	m_pVertSrc = new VERTEX[ m_dwVert ];

	//	SAFE_RELEASE( m_pEffVB );
	//	pd3dDevice->CreateVertexBuffer( m_dwVert*sizeof(VERTEX), D3DUSAGE_WRITEONLY,
	//									VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pEffVB );

	//	VERTEX *pSrc, *Dest;
	//	m_pLocalMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
	//	m_pEffVB->Lock( 0, 0, (VOID**)&Dest, 0L );
	//	memcpy( m_pVertSrc, pSrc, m_dwVert*sizeof(VERTEX) );
	//	memcpy( Dest, pSrc, m_dwVert*sizeof(VERTEX) );
	//	m_pEffVB->Unlock();
	//	m_pLocalMesh->UnlockVertexBuffer();
	//}

	//{
	//	m_dwFace = m_pLocalMesh->GetNumFaces();

	//	SAFE_RELEASE( m_pEffIB );
	//	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L,
	//									D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pEffIB );

	//	WORD *pSrc, *Dest;
	//	m_pLocalMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
	//	m_pEffIB->Lock( 0, 0, (VOID**)&Dest, 0L );
	//	memcpy( Dest, pSrc, m_dwFace*3*sizeof(WORD) );
	//	m_pEffIB->Unlock();
	//	m_pLocalMesh->UnlockIndexBuffer();
	//}

	//{
	//	m_pLocalMesh->GetAttributeTable( NULL, &m_dwAttrib );

	//	SAFE_DELETE_ARRAY( m_pAttrib );
	//	m_pAttrib = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
	//	m_pLocalMesh->GetAttributeTable( m_pAttrib, &m_dwAttrib );
	//}
}

