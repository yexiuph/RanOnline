// DxEffectDot3.cpp: implementation of the DxEffectDot3 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"



#include "./DxFrameMesh.h"
#include "./DxLightMan.h"
#include "./DxEffectMan.h"

#include "./DxEffectDot3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const DWORD DxEffectDot3::TYPEID = DEF_EFFECT_DOT3;
const DWORD	DxEffectDot3::VERSION = 0x00000100;
const char DxEffectDot3::NAME[] = "[ 내적블렌딩,디테일 맵 ]";
const DWORD DxEffectDot3::FLAG = NULL;

void DxEffectDot3::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(DOT3_PROPERTY);
	dwVer = VERSION;
}

void DxEffectDot3::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(DOT3_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

LPDIRECT3DSTATEBLOCK9 DxEffectDot3::m_pDetailSB = NULL;
LPDIRECT3DSTATEBLOCK9 DxEffectDot3::m_pDetailSB_SAVE = NULL;
LPDIRECT3DSTATEBLOCK9 DxEffectDot3::m_pEffectSB = NULL;
LPDIRECT3DSTATEBLOCK9 DxEffectDot3::m_pEffectSB_SAVE = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectDot3::DxEffectDot3() :
	m_bDot3(TRUE),
	m_pddsTexDetail(NULL),
	m_pddsTexture(NULL),
	m_fPower(0.9f),
	m_fTexX(1.0f),
	m_fTexY(1.0f)
{
	StringCchCopy( m_szTexDetail, MAX_PATH, "_Eff_detail.bmp" );
	StringCchCopy( m_szTexture, MAX_PATH, "Test_Day.dds" );
}

DxEffectDot3::~DxEffectDot3()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectDot3::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//DWORD dwDecl[] =
	//{
	//	D3DVSD_STREAM(0),
	//	D3DVSD_REG(0, D3DVSDT_FLOAT3 ),		//	D3DVSDE_POSITION,  0  
	//	D3DVSD_REG(3, D3DVSDT_FLOAT3 ),		//	Normal
	//	D3DVSD_REG(7, D3DVSDT_FLOAT2 ),		//	Tex1
	//	D3DVSD_END()
	//};



	//hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwDot3VertexShader, &m_dwEffect, 
	//										DxEffectMan::GetInstance().GetUseSwShader() );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Dot3 FAILED" );
	//	return E_FAIL;
	//}

//	SAFE_DELETE_ARRAY ( dwDot3VertexShader );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING ,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_DESTCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffffffff );//m_dwBlendAlpha );	// 알파값... ㅋㅋㅋ환경맵 알파.

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pDetailSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pDetailSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING ,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_DOTPRODUCT3 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pEffectSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pEffectSB );
	}

	return S_OK;
}

HRESULT DxEffectDot3::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pDetailSB );
	SAFE_RELEASE( m_pDetailSB_SAVE );

	SAFE_RELEASE( m_pEffectSB );
	SAFE_RELEASE( m_pEffectSB_SAVE );

	return S_OK;
}

HRESULT DxEffectDot3::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectDot3::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexDetail, pd3dDevice, m_pddsTexDetail, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0, TRUE );

	return S_OK;
}

HRESULT DxEffectDot3::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szTexDetail, m_pddsTexDetail );
	TextureManager::ReleaseTexture ( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffectDot3::FrameMove ( float fTime, float fElapsedTime )
{

	return S_OK;
}

HRESULT DxEffectDot3::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	if( !m_szTexDetail )	TextureManager::GetTexture( m_szTexDetail, m_pddsTexDetail );
	if( !m_pddsTexture )	TextureManager::GetTexture( m_szTexture, m_pddsTexture );

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

				//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
				//
				D3DXMATRIX	matWorld, matView, matProj, matWVP;

				matView = DxViewPort::GetInstance().GetMatView();
				matProj = DxViewPort::GetInstance().GetMatProj();

				D3DXMatrixTranspose( &matWorld, &(pframeCur->matCombined) );
				pd3dDevice->SetVertexShaderConstantF( VSC_MATWVP_01, (float*)&matWorld,	4 );

				D3DXMatrixMultiply ( &matWVP, &(pframeCur->matCombined), &matView );
				D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
				D3DXMatrixTranspose( &matWVP, &matWVP );
				pd3dDevice->SetVertexShaderConstantF( VSC_MATWVP_02, (float*)&matWVP,	4 );





				pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );


				pd3dDevice->SetVertexShaderConstantF( VSC_MATRIAL, (float*)&D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ), 1 );

				//	Note : 랜더링 상태 조정.
				if( m_bDot3 )
				{
					pd3dDevice->SetTexture ( 0, m_pddsTexture );
					m_pEffectSB_SAVE->Capture();
					m_pEffectSB->Apply();
				}
				else
				{
					pd3dDevice->SetTexture ( 0, m_pddsTexture );
					m_pDetailSB_SAVE->Capture();
					m_pDetailSB->Apply();
				}

				DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
				D3DXVECTOR3 Light11;
				Light11.x = Light.m_Light.Direction.x;
				Light11.y = Light.m_Light.Direction.z;
				Light11.z = m_fPower;


				DWORD dwFactor = VectortoRGBA( &Light11, 0.0f );
				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFactor );

				//	Note : 소프트웨어 버텍스 프로세싱
				//
				pd3dDevice->SetFVF ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{
					pd3dDevice->SetIndices ( pIB );
					pd3dDevice->SetStreamSource ( 0, pVB, 0, dwFVFSize );
					//pd3dDevice->SetFVF ( pmsMeshs->m_pLocalMesh->GetFVF () );
				}
				else
				{
					pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );
					pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, 0, dwFVFSize );
					//pd3dDevice->SetFVF ( pmsMeshs->m_pDxOctreeMesh->m_dwFVF );
				}				


				pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces );

				//	Note : 랜더링 상태 조정.
				if( m_bDot3 )	m_pEffectSB_SAVE->Apply();
				else			m_pDetailSB_SAVE->Apply();

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

	return S_OK;
}