// DxEffectLightMap.cpp: implementation of the DxEffectLightMap class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"



#include "./DxFrameMesh.h"
#include "./DxLightMan.h"
#include "./DxEffectMan.h"

#include "./DxEffectLightMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectLightMap::TYPEID = DEF_EFFECT_LIGHTMAP;
const DWORD	DxEffectLightMap::VERSION = 0x00000100;
const char DxEffectLightMap::NAME[] = "[ 라이트 맵 ]";
const DWORD DxEffectLightMap::FLAG = NULL;

LPDIRECT3DSTATEBLOCK9	DxEffectLightMap::m_pSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectLightMap::m_pSB_SAVE = NULL;

void DxEffectLightMap::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(LIGHTMAP_PROPERTY);
	dwVer = VERSION;
}

void DxEffectLightMap::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(LIGHTMAP_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectLightMap::DxEffectLightMap() :
	m_bRender(TRUE),
	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),
	m_bUse0(FALSE),
	m_bUse1(FALSE),
	m_bUse2(FALSE),
	m_bUse3(FALSE),
	m_bUse4(FALSE),
	m_bUse5(FALSE),
	m_bUse6(FALSE),
	m_bUse7(FALSE)
{
	D3DUtil_InitLight ( m_sLight0, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	D3DUtil_InitLight ( m_sLight1, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	D3DUtil_InitLight ( m_sLight2, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	D3DUtil_InitLight ( m_sLight3, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	D3DUtil_InitLight ( m_sLight4, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	D3DUtil_InitLight ( m_sLight5, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	D3DUtil_InitLight ( m_sLight6, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	D3DUtil_InitLight ( m_sLight7, D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
}

DxEffectLightMap::~DxEffectLightMap()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectLightMap::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	float fBias = -0.0001f;
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB );
	}

	return S_OK;
}

HRESULT DxEffectLightMap::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB );
	SAFE_RELEASE( m_pSB_SAVE );

	return S_OK;
}

//	Note : DxFrame 에 효과를 붙일 경우에 사용된다.
//
HRESULT DxEffectLightMap::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	m_pAdaptFrame = pFrame;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectLightMap::FrameMove ( float fTime, float fElapsedTime )
{

	return S_OK;
}

HRESULT DxEffectLightMap::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	if( !DxLightMan::GetInstance()->ISDynamicLight() )	return S_OK;

	pd3dDevice->GetLight ( 0, &m_sLight0 );
	pd3dDevice->GetLight ( 1, &m_sLight1 );
	pd3dDevice->GetLight ( 2, &m_sLight2 );
	pd3dDevice->GetLight ( 3, &m_sLight3 );
	pd3dDevice->GetLight ( 4, &m_sLight4 );
	pd3dDevice->GetLight ( 5, &m_sLight5 );
	pd3dDevice->GetLight ( 6, &m_sLight6 );
	pd3dDevice->GetLight ( 7, &m_sLight7 );

	pd3dDevice->GetLightEnable ( 0, &m_bUse0 );
	pd3dDevice->GetLightEnable ( 1, &m_bUse1 );
	pd3dDevice->GetLightEnable ( 2, &m_bUse2 );
	pd3dDevice->GetLightEnable ( 3, &m_bUse3 );
	pd3dDevice->GetLightEnable ( 4, &m_bUse4 );
	pd3dDevice->GetLightEnable ( 5, &m_bUse5 );
	pd3dDevice->GetLightEnable ( 6, &m_bUse6 );
	pd3dDevice->GetLightEnable ( 7, &m_bUse7 );

	DxLightMan::GetInstance()->SetDPLight( pd3dDevice );

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
				LPDIRECT3DVERTEXBUFFERQ	pVB = NULL;
				LPDIRECT3DINDEXBUFFERQ	pIB = NULL;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{	
					pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );
					
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

				//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
				//
				D3DXMATRIX	matWorld;

				pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );

				// Note : StateBlock Capture
				m_pSB_SAVE->Capture();
				m_pSB->Apply();

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

				pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces );

				// Note : StateBlock Apply
				m_pSB_SAVE->Apply();

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pVB) pVB->Release ();
					if(pIB) pIB->Release ();
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

	pd3dDevice->SetLight ( 0, &m_sLight0 );
	pd3dDevice->SetLight ( 1, &m_sLight1 );
	pd3dDevice->SetLight ( 2, &m_sLight2 );
	pd3dDevice->SetLight ( 3, &m_sLight3 );
	pd3dDevice->SetLight ( 4, &m_sLight4 );
	pd3dDevice->SetLight ( 5, &m_sLight5 );
	pd3dDevice->SetLight ( 6, &m_sLight6 );
	pd3dDevice->SetLight ( 7, &m_sLight7 );

	pd3dDevice->LightEnable ( 0, m_bUse0 );
	pd3dDevice->LightEnable ( 1, m_bUse1 );
	pd3dDevice->LightEnable ( 2, m_bUse2 );
	pd3dDevice->LightEnable ( 3, m_bUse3 );
	pd3dDevice->LightEnable ( 4, m_bUse4 );
	pd3dDevice->LightEnable ( 5, m_bUse5 );
	pd3dDevice->LightEnable ( 6, m_bUse6 );
	pd3dDevice->LightEnable ( 7, m_bUse7 );

	return S_OK;
}