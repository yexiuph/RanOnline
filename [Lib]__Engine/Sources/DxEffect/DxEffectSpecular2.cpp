// Specular.cpp: implementation of the DxEffectSpecular2 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"
#include "./Collision.h"

#include "./DxEffectMan.h"
#include "./SerialFile.h"

#include "./DxShadowMap.h"

#include "./DxViewPort.h"

#include "./DxEffectSpecular2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectSpecular2::TYPEID = DEF_EFFECT_SPECULAR2;
const DWORD	DxEffectSpecular2::VERSION = 0x00000100;
const char	DxEffectSpecular2::NAME[] = "[ 스 펙 큘 러 2 ]";
const DWORD DxEffectSpecular2::FLAG = NULL;

LPDIRECT3DSTATEBLOCK9	DxEffectSpecular2::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectSpecular2::m_pDrawStateBlock	= NULL;

void DxEffectSpecular2::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(SPECULAR2_PROPERTY);
	dwVer = VERSION;
}

void DxEffectSpecular2::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(SPECULAR2_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectSpecular2::DxEffectSpecular2 () :
	m_dwColorOP(D3DTOP_MODULATE),
	m_dwMaterials(0),
	m_dwTempMaterials(0),
	m_pTempMaterials(NULL),
	m_pMaterials(NULL)
{
}

DxEffectSpecular2::~DxEffectSpecular2 ()
{
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials );
}

DXMATERIAL_SPEC2::~DXMATERIAL_SPEC2 ()
{
	TextureManager::ReleaseTexture( szSpecTex, pSpecTex );
}

HRESULT DxEffectSpecular2::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			DxAnalysis ( pd3dDevice, pmsMeshs );
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectSpecular2::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectSpecular2::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_SPECULAR );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pDrawStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectSpecular2::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pDrawStateBlock	);

	return S_OK;
}

HRESULT DxEffectSpecular2::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffectSpecular2::DeleteDeviceObjects ()
{
	return S_OK;
}

void DxEffectSpecular2::ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VOID* pTex = NULL;
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		TextureManager::ReleaseTexture( m_pMaterials[i].szSpecTex, m_pMaterials[i].pSpecTex );
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		TextureManager::LoadTexture ( m_pMaterials[i].szSpecTex, pd3dDevice, m_pMaterials[i].pSpecTex, 0, 0 );
	}
}

HRESULT DxEffectSpecular2::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectSpecular2::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectSpecular2::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{

	PROFILE_BEGIN("DxEffectSpecular2");

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
					if ( !m_pMaterials[ pAttribTable[i].AttribId ].bSpecUse )	continue;

					//	Note : 메터리얼, 텍스쳐 지정.
					pmsMeshs->rgMaterials[ pAttribTable[i].AttribId ].Power = 30.f;
					pmsMeshs->rgMaterials[ pAttribTable[i].AttribId ].Specular.r = 1.f;
					pmsMeshs->rgMaterials[ pAttribTable[i].AttribId ].Specular.g = 1.f;
					pmsMeshs->rgMaterials[ pAttribTable[i].AttribId ].Specular.b = 1.f;
					pmsMeshs->rgMaterials[ pAttribTable[i].AttribId ].Specular.a = 1.f;
					pd3dDevice->SetMaterial ( &( pmsMeshs->rgMaterials[ pAttribTable[i].AttribId ] ) );

					pd3dDevice->SetTexture ( 0, m_pMaterials[ pAttribTable[i].AttribId ].pSpecTex );


					m_pSavedStateBlock->Capture();
					m_pDrawStateBlock->Apply();

					DWORD	dwColorOP;
					pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );
					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	m_dwColorOP );


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

					pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

					m_pSavedStateBlock->Apply();
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

	PROFILE_END("DxEffectSpecular2");

	return S_OK;
}

HRESULT DxEffectSpecular2::DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	HRESULT	hr = S_OK;

	m_dwMaterials = pmsMeshs->cMaterials;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_SPEC2[m_dwMaterials];

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pmsMeshs->strTextureFiles[i].GetString () );

		if ( !strlen(m_pMaterials[i].szSpecTex) )
			StringCchCopy( m_pMaterials[i].szSpecTex, MAX_PATH, pmsMeshs->strTextureFiles[i].GetString () );

		TextureManager::LoadTexture ( m_pMaterials[i].szSpecTex, pd3dDevice, m_pMaterials[i].pSpecTex, 0, 0 );

		//hr = TextureManager::LoadTexture ( m_pMaterials[i].szSpecTex, pd3dDevice, m_pMaterials[i].pSpecTex, 0, 0 );
		//if (FAILED(hr))
		//{
		//	hr = S_OK;
		//	m_pMaterials[i].pSpecTex = NULL;
		//}
	}

	if ( pmsMeshs->m_pDxOctreeMesh )
	{
		if ( !m_pTempMaterials )	return S_OK;

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			for ( DWORD j=0; j<m_dwTempMaterials; j++ )
			{
				if ( !strcmp ( m_pMaterials[i].szTexture, m_pTempMaterials[j].szTexture ) )
				{
					m_pMaterials[i].bSpecUse	= m_pTempMaterials[j].bSpecUse;
					StringCchCopy( m_pMaterials[i].szSpecTex, MAX_PATH, m_pTempMaterials[j].szSpecTex );

					TextureManager::ReleaseTexture( m_pMaterials[i].szSpecTex, m_pMaterials[i].pSpecTex );
					TextureManager::LoadTexture( m_pMaterials[i].szSpecTex, pd3dDevice, m_pMaterials[i].pSpecTex, 0, 0 );
				}
			}
		}
	}

	return hr;
}

void DxEffectSpecular2::SaveBuffer ( CSerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(DXMATERIAL_SPEC2)*m_dwTempMaterials
			+sizeof(DWORD)+sizeof(DXMATERIAL_SPEC2)*m_dwMaterials;

	// Material 저장
	SFile << m_dwTempMaterials;
	if ( m_pTempMaterials )
		SFile.WriteBuffer ( m_pTempMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwTempMaterials );

	SFile << m_dwMaterials;
	if ( m_pMaterials )
		SFile.WriteBuffer ( m_pMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwMaterials );
}

void DxEffectSpecular2::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	// Material
	SFile >> m_dwTempMaterials;
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	if ( m_dwTempMaterials )
	{
		m_pTempMaterials = new DXMATERIAL_SPEC2[m_dwTempMaterials];
		SFile.ReadBuffer ( m_pTempMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwTempMaterials );
	}

	// Material
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	if ( m_dwMaterials )
	{
		m_pMaterials = new DXMATERIAL_SPEC2[m_dwMaterials];
		SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwMaterials );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].pSpecTex = NULL;
			TextureManager::LoadTexture( m_pMaterials[i].szSpecTex,	pd3dDevice, m_pMaterials[i].pSpecTex, 0, 0 );
		}
	}
}

void DxEffectSpecular2::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	// Material
	SFile >> m_dwTempMaterials;
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	if ( m_dwTempMaterials )
	{
		m_pTempMaterials = new DXMATERIAL_SPEC2[m_dwTempMaterials];
		SFile.ReadBuffer ( m_pTempMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwTempMaterials );
	}

	// Material
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	if ( m_dwMaterials )
	{
		m_pMaterials = new DXMATERIAL_SPEC2[m_dwMaterials];
		SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwMaterials );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].pSpecTex = NULL;
			TextureManager::LoadTexture( m_pMaterials[i].szSpecTex, pd3dDevice, m_pMaterials[i].pSpecTex, 0, 0 );
		}
	}
}

HRESULT DxEffectSpecular2::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectSpecular2* pSrcEff = (DxEffectSpecular2*) pSrcEffect;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );

	// Material 
	if ( pSrcEff->m_pMaterials )
	{
		//	원본 만들기
		m_dwMaterials = pSrcEff->m_dwMaterials;

		m_pMaterials = new DXMATERIAL_SPEC2[pSrcEff->m_dwMaterials];
		memcpy ( m_pMaterials, pSrcEff->m_pMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwMaterials );

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].pSpecTex = NULL;
			TextureManager::LoadTexture( m_pMaterials[i].szSpecTex, pd3dDevice, m_pMaterials[i].pSpecTex, 0, 0 );
		}

		//	Frame 만들기
		m_dwTempMaterials = pSrcEff->m_dwMaterials;

		m_pTempMaterials = new DXMATERIAL_SPEC2[m_dwTempMaterials];
		memcpy ( m_pTempMaterials, pSrcEff->m_pMaterials, sizeof(DXMATERIAL_SPEC2)*m_dwTempMaterials );

		for ( DWORD i=0; i<m_dwTempMaterials; i++ )
		{
			m_pTempMaterials[i].pSpecTex = NULL;
		}
	}

	return S_OK;
}