// DxEffectShadowLow.cpp: implementation of the DxEffectShadowLowHW class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./DxSurfaceTex.h"
#include "./DxShadowMap.h"

#include "./EditMeshs.h"
#include "./Collision.h"
#include "./SerialFile.h"

#include "./DxLightMan.h"
#include "./DxSetLandMan.h"

#include "./DxEffectShadowLow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT DxEffectShadowLow::RenderObjTex ( DxFrame* pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ	pSSTexture,
									  DWORD dwFVF, LPDIRECT3DSTATEBLOCK9 pSavedSB, LPDIRECT3DSTATEBLOCK9 pEffectSB )
{
	//DxMeshes*	pmsMeshs;
	//if ( !pframeCur->pmsMeshs )	return S_OK;
	//
	////	Note : 메쉬 그리기.
	////
	//pmsMeshs = pframeCur->pmsMeshs;
	//while ( pmsMeshs != NULL )
	//{
	//	if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
	//	{
	//		DWORD dwVerts, dwFaces;
	//		LPDIRECT3DVERTEXBUFFERQ pVB;
	//		LPDIRECT3DINDEXBUFFERQ	pIB;

	//		//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//		//
	//		if ( pmsMeshs->m_pLocalMesh )
	//		{	
	//			pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
	//			pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );

	//			dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
	//			dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
	//		}
	//		else
	//		{
	//			dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
	//			dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
	//		}

	//		//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
	//		//
	//		D3DXMATRIX matTran, matWorld, matView, matProj, matWVP;

	//		pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
	//		pd3dDevice->SetTransform( D3DTS_WORLD, &pframeCur->matCombined );









	//		DWORD					AttribTableSize = 0;
	//		LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

	//		if ( pmsMeshs->m_pLocalMesh )
	//		{	
	//			pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &AttribTableSize );
	//		}
	//		else
	//		{
	//			AttribTableSize = pmsMeshs->m_pDxOctreeMesh->m_dwAttribTableSize;
	//		}
	//		
	//		LPD3DXATTRIBUTERANGE pAttribTable;
	//		pAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];

	//		if ( pmsMeshs->m_pLocalMesh )
	//		{
	//			pmsMeshs->m_pLocalMesh->GetAttributeTable( pAttribTable, &AttribTableSize );
	//		}
	//		else
	//		{
	//			memcpy ( pAttribTable, pmsMeshs->m_pDxOctreeMesh->m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * AttribTableSize );
	//		}

	//		//	Note : 이전 상태 백업.	
	//		//			랜더링 상태 조정.
	//		//
	//		pSavedSB->Capture();
	//		pEffectSB->Apply();

	//		if ( dwFVF & D3DFVF_TEX2 )	pd3dDevice->SetStreamSource ( 0, m_pBaseVB, sizeof(VERTEX_2TEX) );
	//		else
	//		{
	//			if ( pmsMeshs->m_pLocalMesh )	pd3dDevice->SetStreamSource ( 0, pVB, sizeof(VERTEX) );
	//			else							pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, sizeof(VERTEX) );
	//		}

	//		//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//		//
	//		if ( pmsMeshs->m_pLocalMesh )	pd3dDevice->SetIndices ( pIB, 0 );
	//		else							pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB, 0 );


	//		pd3dDevice->SetFVF ( dwFVF );

	//		//	Note : 불투명 메쉬 드로잉.
	//		//		pmsMeshs->cMaterials --> AttribTableSize
	//		//
	//		for ( DWORD i=0; i<AttribTableSize; i++ )
	//		{
	//			pd3dDevice->SetVertexShaderConstant ( VSC_MATRIAL, &( pmsMeshs->rgMaterials[0].Diffuse ), 1 );	

	//			pd3dDevice->SetMaterial ( &pmsMeshs->rgMaterials[0] );


	//			pd3dDevice->SetTexture ( 0, pmsMeshs->pTextures[pAttribTable[i].AttribId] );
	//			if ( dwFVF & D3DFVF_TEX2 )			pd3dDevice->SetTexture ( 1, pSSTexture );






	//			pd3dDevice->DrawIndexedPrimitive
	//			(
	//				D3DPT_TRIANGLELIST,
	//				pAttribTable[i].VertexStart,
	//				pAttribTable[i].VertexCount,
	//				pAttribTable[i].FaceStart * 3,
	//				pAttribTable[i].FaceCount
	//			);
	//		}
	//		SAFE_DELETE(pAttribTable);

	//		pd3dDevice->SetTexture ( 1, NULL );



	//		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );


	//		pSavedSB->Apply();





	//		if ( pmsMeshs->m_pLocalMesh )
	//		{
	//			if(pIB) pIB->Release ();
	//			if(pVB)	pVB->Release ();
	//		}
	//	}
	//	pmsMeshs = pmsMeshs->pMeshNext;
	//}




	if ( !m_pShadowVB )	return S_OK;
		
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	//	Note : Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts, dwFaces;
				LPDIRECT3DINDEXBUFFERQ pIB;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );

					dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
					dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
				}
				else
				{
					dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
					dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
				}
				dwFVFSize = D3DXGetFVFVertexSize ( dwFVF );

				//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
				//
				D3DXMATRIX	matView, matProj;
				pd3dDevice->SetTransform( D3DTS_WORLD, &pframeCur->matCombined );

				pd3dDevice->SetTexture ( 0, pSSTexture );

				//	Note : 이전 상태 백업.	
				//			랜더링 상태 조정.
				pSavedSB->Capture();
				pEffectSB->Apply();


				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
					pd3dDevice->SetIndices ( pIB );
				else
					pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );

				pd3dDevice->SetStreamSource ( 0, m_pShadowVB, 0, dwFVFSize );
				pd3dDevice->SetFVF ( dwFVF );
				pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces);

				//	Note : 이전상태로 복원.
				pSavedSB->Apply();

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
				}
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return S_OK;
}

HRESULT DxEffectShadowLow::RenderTnL ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF, 
									  LPDIRECT3DSTATEBLOCK9 pSavedSB, LPDIRECT3DSTATEBLOCK9 pEffectSB )
{
	if ( !m_pShadowVB )	return S_OK;
		
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	//	Note : Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts, dwFaces;
				LPDIRECT3DINDEXBUFFERQ pIB;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );

					dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
					dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
				}
				else
				{
					dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
					dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
				}
				dwFVFSize = D3DXGetFVFVertexSize ( dwFVF );

				//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
				//
				D3DXMATRIX	matView, matProj;
				pd3dDevice->SetTransform( D3DTS_WORLD, &pframeCur->matCombined );

				if ( DxShadowMap::GetInstance().GetShadowDetail() == SHADOW_NONE )
					pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pShadowTex );
				else
					pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pShadowTex );

				//	Note : 이전 상태 백업.	
				//			랜더링 상태 조정.
				pSavedSB->Capture();
				pEffectSB->Apply();

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
					pd3dDevice->SetIndices ( pIB );
				else
					pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );

				pd3dDevice->SetStreamSource ( 0, m_pShadowVB, 0, dwFVFSize );
				pd3dDevice->SetFVF ( dwFVF );
				pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces);

				//	Note : 이전상태로 복원.
				pSavedSB->Apply();

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
				}
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}
	
	return S_OK;
}

HRESULT DxEffectShadowLow::RenderShadow ( DxFrame* pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ	pSSTexture,
							DWORD dwFVF, LPDIRECT3DSTATEBLOCK9 pSavedSB, LPDIRECT3DSTATEBLOCK9 pEffectSB )
{
	if ( !m_pShadowVB )	return S_OK;
		
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	//	Note : Render
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts, dwFaces;
				LPDIRECT3DINDEXBUFFERQ pIB;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );

					dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
					dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
				}
				else
				{
					dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
					dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
				}
				dwFVFSize = D3DXGetFVFVertexSize ( dwFVF );

				//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
				//
				D3DXMATRIX matView, matProj;
				pd3dDevice->SetTransform( D3DTS_WORLD, &pframeCur->matCombined );

				if ( DxShadowMap::GetInstance().GetShadowDetail() == SHADOW_NONE )
					pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pShadowTex );
				else
					pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pShadowTex );
					
				pd3dDevice->SetTexture ( 1, pSSTexture );

				//	Note : 이전 상태 백업.	
				//			랜더링 상태 조정.
				pSavedSB->Capture();
				pEffectSB->Apply();


				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
					pd3dDevice->SetIndices ( pIB );
				else
					pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );

				pd3dDevice->SetStreamSource ( 0, m_pShadowVB, 0, dwFVFSize );
				pd3dDevice->SetFVF ( dwFVF );
				pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces);

				//	Note : 이전상태로 복원.
				pSavedSB->Apply();

				pd3dDevice->SetTexture ( 1, NULL );

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
				}
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return S_OK;
}
