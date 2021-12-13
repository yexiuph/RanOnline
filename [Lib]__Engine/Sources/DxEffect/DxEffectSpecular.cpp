// Specular.cpp: implementation of the DxEffectSpecular class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"
#include "./Collision.h"

#include "./DxEffectMan.h"
#include "./DxShadowMap.h"

#include "./DxViewPort.h"

#include "./DxEffectSpecular.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectSpecular::TYPEID = DEF_EFFECT_SPECULAR;
const DWORD	DxEffectSpecular::VERSION = 0x00000100;
const char	DxEffectSpecular::NAME[] = "[ 스 펙 큘 러 (권장) ]";
const DWORD DxEffectSpecular::FLAG = NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectSpecular::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectSpecular::m_pDrawStateBlock		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectSpecular::m_pSavedALLSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectSpecular::m_pDrawALLSB			= NULL;

void DxEffectSpecular::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(SPECULAR_PROPERTY);
	dwVer = VERSION;
}

void DxEffectSpecular::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(SPECULAR_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectSpecular::DxEffectSpecular ()	:
	m_bAll ( FALSE )
{
}

DxEffectSpecular::~DxEffectSpecular ()
{
	CleanUp ();
}

//	Note : DxFrame 에 효과를 붙일 경우에 사용된다.
//
HRESULT DxEffectSpecular::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectSpecular::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,	0x00000000 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_SPECULAR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATEALPHA_ADDCOLOR  );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE  );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedALLSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawALLSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE  );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pDrawStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectSpecular::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pDrawStateBlock );
	SAFE_RELEASE( m_pSavedALLSB );
	SAFE_RELEASE( m_pDrawALLSB );
	
	return S_OK;
}

HRESULT DxEffectSpecular::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectSpecular");

	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

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
				LPDIRECT3DINDEXBUFFERQ	pIB = NULL;
				LPDIRECT3DVERTEXBUFFERQ pVB = NULL;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );
					pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
					
					dwFVFSize = pmsMeshs->m_pLocalMesh->GetFVF ();
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
					dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
				}
				else
				{
					dwFVFSize = pmsMeshs->m_pDxOctreeMesh->m_dwFVF;
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
					dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
				}
				pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );

				DWORD					AttribTableSize = 0;
				LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

				if ( pmsMeshs->m_pLocalMesh )
				{	
					pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &AttribTableSize );
				}
				else
				{
					AttribTableSize = pmsMeshs->m_pDxOctreeMesh->m_dwAttribTableSize;
				}
				
				LPD3DXATTRIBUTERANGE pAttribTable;
				pAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];

				if ( pmsMeshs->m_pLocalMesh )
				{
					pmsMeshs->m_pLocalMesh->GetAttributeTable( pAttribTable, &AttribTableSize );
				}
				else
				{
					memcpy ( pAttribTable, pmsMeshs->m_pDxOctreeMesh->m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * AttribTableSize );
				}

				//	Note : 불투명 메쉬 드로잉.
				//		pmsMeshs->cMaterials --> AttribTableSize
				//
				for ( DWORD i=0; i<AttribTableSize; i++ )
				{
					//	Note : 메터리얼, 텍스쳐 지정.
					pmsMeshs->rgMaterials[i].Power = 30.f;
					pmsMeshs->rgMaterials[i].Specular.r = 1.f;
					pmsMeshs->rgMaterials[i].Specular.g = 1.f;
					pmsMeshs->rgMaterials[i].Specular.b = 1.f;
					pmsMeshs->rgMaterials[i].Specular.a = 1.f;
					pd3dDevice->SetMaterial ( &( pmsMeshs->rgMaterials[i] ) );
					pd3dDevice->SetTexture ( 0, pmsMeshs->pTextures[pAttribTable[i].AttribId] );
				

					if ( m_bAll )
					{
						m_pSavedALLSB->Capture();
						m_pDrawALLSB->Apply();
					}
					else
					{
						m_pSavedStateBlock->Capture();
						m_pDrawStateBlock->Apply();
					}


					//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
					//
					if ( pmsMeshs->m_pLocalMesh )
					{
						pd3dDevice->SetIndices ( pIB );
						pd3dDevice->SetStreamSource ( 0, pVB, 0, dwFVFSize );
						pd3dDevice->SetFVF ( pmsMeshs->m_pLocalMesh->GetFVF () );
					}
					else
					{
						pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );
						pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, 0, dwFVFSize );
						pd3dDevice->SetFVF ( pmsMeshs->m_pDxOctreeMesh->m_dwFVF );
					}		

					pd3dDevice->DrawIndexedPrimitive
					(
						D3DPT_TRIANGLELIST,
						0,
						pAttribTable[i].VertexStart,
						pAttribTable[i].VertexCount,
						pAttribTable[i].FaceStart * 3,
						pAttribTable[i].FaceCount
					);

					if ( m_bAll )	m_pSavedALLSB->Apply();
					else			m_pSavedStateBlock->Apply();
				}

				SAFE_DELETE(pAttribTable);

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
					if(pVB) pVB->Release ();
				}
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : 자식 프레임 그리기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = Render ( pframeChild, pd3dDevice, pFrameMesh, pLandMan );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	PROFILE_END("DxEffectSpecular");

	return S_OK;
}
