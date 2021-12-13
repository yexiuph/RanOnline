// DxEffectToon.cpp: implementation of the DxEffectToon class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"

#include "./DxEffectToon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectToon::TYPEID = DEF_EFFECT_CARTOON;
const DWORD	DxEffectToon::VERSION = 0x00000100;
const char DxEffectToon::NAME[] = "[ 카 툰 렌 더 링 ]";
const DWORD DxEffectToon::FLAG = NULL;

LPDIRECT3DSTATEBLOCK9	DxEffectToon::m_pSavedStateBlock = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectToon::m_pEffectStateBlock = NULL;

void DxEffectToon::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(CARTOON_PROPERTY);
	dwVer = VERSION;
}

void DxEffectToon::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(CARTOON_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////6.f/32.f , 0.35f, 31.f/32.f, 0.4f

DxEffectToon::DxEffectToon() :
	m_fNum1(0.f),	// 어두워지는 범위
	m_fNum2(0.3f),	// 전체적인 밝기
	m_fNum3(0.9f),	// 밝아 지는 범위
	m_fNum4(0.8f),	// 제일 밝은 곳의 밝기
	m_pddsTexture(NULL)
{
	StringCchCopy( m_szTexture, MAX_PATH, "Test_Day.dds" );
}

DxEffectToon::~DxEffectToon()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectToon::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//DWORD dwDecl[] =
	//{
	//	D3DVSD_STREAM(0),
	//	D3DVSD_REG(0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
	//	D3DVSD_REG(1, D3DVSDT_FLOAT3 ),		
	//	D3DVSD_REG(2, D3DVSDT_FLOAT2 ),		
	//	D3DVSD_END()
	//};

	//hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwToonVertexShader, &m_dwEffect, 
	//										DxEffectMan::GetInstance().GetUseSwShader() );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Toon FAILED" );
	//	return E_FAIL;
	//}



	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );


		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectToon::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( m_dwEffect )	pd3dDevice->DeleteVertexShader ( m_dwEffect );

	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffectToon::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectToon::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0 );

	return S_OK;
}

HRESULT DxEffectToon::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture ( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffectToon::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT DxEffectToon::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
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
				LPDIRECT3DINDEXBUFFERQ pIB	= NULL;
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
				pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_01, (float*)&matWorld,	4 );

				D3DXMatrixMultiply ( &matWVP, &(pframeCur->matCombined), &matView );
				D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
				D3DXMatrixTranspose( &matWVP, &matWVP );
				pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_02, (float*)&matWVP,	4 );


				//	Note : SetTextureStageState() 선언
				//
				pd3dDevice->SetTexture ( 0, m_pddsTexture );


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
				//
				//	Note : 불투명 메쉬 드로잉.
				//		pmsMeshs->cMaterials --> AttribTableSize
				//
				for ( DWORD i=0; i<AttribTableSize; i++ )
				{
					//	Note : 메터리얼, 텍스쳐 지정.
					pd3dDevice->SetTexture ( 1, pmsMeshs->pTextures[pAttribTable[i].AttribId] );
					pd3dDevice->SetVertexShaderConstantF ( VSC_MATRIAL, (float*)&( pmsMeshs->rgMaterials[0].Diffuse ), 1 );			


					//	Note : 이전 상태 백업.
					m_pSavedStateBlock->Capture();

					//	Note : 랜더링 상태 조정.
					m_pEffectStateBlock->Apply();



					//	Note : 소프트웨어 버텍스 프로세싱
					if ( DxEffectMan::GetInstance().GetUseSwShader() )
						pd3dDevice->SetSoftwareVertexProcessing( TRUE );


					//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
					//
					if ( pmsMeshs->m_pLocalMesh )
					{
						pd3dDevice->SetIndices ( pIB );
						pd3dDevice->SetStreamSource ( 0, pVB, 0, dwFVFSize );
					}
					else
					{
						pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );
						pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, 0, dwFVFSize );
					}
					pd3dDevice->SetFVF ( m_dwEffect );


					pd3dDevice->DrawIndexedPrimitive
					(
						D3DPT_TRIANGLELIST,
						0,
						pAttribTable[i].VertexStart,
						pAttribTable[i].VertexCount,
						pAttribTable[i].FaceStart * 3,
						pAttribTable[i].FaceCount
					);


					//	Note : 소프트웨어 버텍스 프로세싱
					if ( DxEffectMan::GetInstance().GetUseSwShader() )
						pd3dDevice->SetSoftwareVertexProcessing( FALSE );

					//	Note : 이전상태로 복원.
					m_pSavedStateBlock->Apply();

					
					pd3dDevice->SetTexture ( 1, NULL );
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

	return S_OK;
}
