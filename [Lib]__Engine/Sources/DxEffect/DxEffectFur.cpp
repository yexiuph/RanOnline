// DxEffectFur.cpp: implementation of the DxEffectFur class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"

#include "./DxEffectFur.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const DWORD DxEffectFur::TYPEID = DEF_EFFECT_FUR;
const DWORD	DxEffectFur::VERSION = 0x00000100;
const char DxEffectFur::NAME[] = "[ 잔디,털 효과 ]";
const DWORD DxEffectFur::FLAG = NULL;

LPDIRECT3DSTATEBLOCK9 DxEffectFur::m_pSB = NULL;
LPDIRECT3DSTATEBLOCK9 DxEffectFur::m_pSB_SAVE = NULL;

void DxEffectFur::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(FUR_PROPERTY);
	dwVer = VERSION;
}

void DxEffectFur::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(FUR_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectFur::DxEffectFur() :
	m_pddsTexture(NULL),
	m_fTime(0.0f),
	m_iLayers(4),
	m_fTexX(5.0f),
	m_fTexY(5.0f),
	m_fOffset(0.2f)
{
	StringCchCopy( m_szTexture, MAX_PATH, "Test_Day.dds" );
}

DxEffectFur::~DxEffectFur()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectFur::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//DWORD dwDecl[] =
	//{
	//	D3DVSD_STREAM(0),
	//	D3DVSD_REG(0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
	//	D3DVSD_REG(3, D3DVSDT_FLOAT3 ),		
	//	D3DVSD_REG(7, D3DVSDT_FLOAT2 ),		
	//	D3DVSD_END()
	//};


	//hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwFurVertexShader, &m_dwEffect, 
	//										DxEffectMan::GetInstance().GetUseSwShader() );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Fur FAILED" );
	//	return E_FAIL;
	//}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB );
	}

	return S_OK;
}

HRESULT DxEffectFur::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB );
	SAFE_RELEASE( m_pSB_SAVE );

	return S_OK;
}

HRESULT DxEffectFur::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
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


HRESULT DxEffectFur::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0, TRUE );

	return S_OK;
}

HRESULT DxEffectFur::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffectFur::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime = fTime;
	return S_OK;
}

HRESULT DxEffectFur::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

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

				//	Note : Shader 상수 선언
				//

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


				for (int j=0;j<m_iLayers;j++)
				{

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
						pd3dDevice->SetTexture ( 0, m_pddsTexture );						
						pd3dDevice->SetVertexShaderConstantF( VSC_MATRIAL, (float*)&( pmsMeshs->rgMaterials[0].Diffuse ), 1 );

						// Note : StateBlock Capture
						m_pSB_SAVE->Capture();
						m_pSB->Apply();

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
						//
						if ( DxEffectMan::GetInstance().GetUseSwShader() )
							pd3dDevice->SetSoftwareVertexProcessing( FALSE );

						// Note : StateBlock Apply
						m_pSB_SAVE->Apply();

					}

					SAFE_DELETE(pAttribTable);
				}

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