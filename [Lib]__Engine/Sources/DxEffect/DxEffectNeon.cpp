// .cpp: implementation of the DxEffectNeon class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"
#include "./Collision.h"
#include "./SerialFile.h"

#include "./DxEffectMan.h"
#include "./DxSurfaceTex.h"
#include "./DxShadowMap.h"

#include "./DxViewPort.h"
#include "./DxRenderStates.h"

#include "./DxEffectNeon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectNeon::OBJECT::FVF = D3DFVF_XYZ|D3DFVF_TEX1;
const DWORD DxEffectNeon::OBJECTNORMAL::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
const DWORD DxEffectNeon::TYPEID = DEF_EFFECT_NEON;
const DWORD	DxEffectNeon::VERSION = 0x00000100;
const char	DxEffectNeon::NAME[] = "[ 방향표시, 텍스쳐의 가는 흐름 ]";
const DWORD DxEffectNeon::FLAG = _EFF_REPLACE;

LPDIRECT3DSTATEBLOCK9	DxEffectNeon::m_pSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectNeon::m_pSB_SAVE = NULL;

void DxEffectNeon::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(NEON_PROPERTY);
	dwVer = VERSION;
}

void DxEffectNeon::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(NEON_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectNeon::DxEffectNeon () :
	m_pVB(NULL),
	m_pSrcVB(NULL),
	m_pIB(NULL),
	m_vAddTex(0,0),
	m_dwVertices(0),
	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),
	m_vCenter(0.f,0.f,0.f),

	m_fTime(0.f),

	m_dwMaterials(0),
	m_pMaterials(NULL),
	m_dwAttribTable(0),
	m_pAttribTable(NULL)
{
}

DxEffectNeon::~DxEffectNeon ()
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pSrcVB );
	SAFE_RELEASE ( m_pIB );

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pAttribTable );
}

DXMATERIAL_NEON::~DXMATERIAL_NEON()
{
	TextureManager::ReleaseTexture( szTexture, pSrcTex );
	TextureManager::ReleaseTexture( szNeonTex, pNeonTex );
}

HRESULT DxEffectNeon::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				DxAnalysis ( pd3dDevice, pmsMeshs );
				CreateObject( pd3dDevice, pmsMeshs );
				SetBoundBox ();
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

//	Note : DxFrame 에 효과를 붙일 경우에 사용된다.
//
HRESULT DxEffectNeon::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
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

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectNeon::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectNeon::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );	// 디퓨즈

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB );
	}

	return S_OK;
}

HRESULT DxEffectNeon::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB );
	SAFE_RELEASE( m_pSB_SAVE );
	
	return S_OK;
}

HRESULT DxEffectNeon::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectNeon::DeleteDeviceObjects ()
{
	return S_OK;
}

void DxEffectNeon::ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VOID* pTex = NULL;
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		TextureManager::ReleaseTexture ( m_pMaterials[i].szTexture, m_pMaterials[i].pSrcTex );
		TextureManager::ReleaseTexture ( m_pMaterials[i].szNeonTex, m_pMaterials[i].pNeonTex );
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		TextureManager::LoadTexture( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pSrcTex, 0, 0, TRUE );
		TextureManager::LoadTexture( m_pMaterials[i].szNeonTex, pd3dDevice, m_pMaterials[i].pNeonTex, 0, 0, TRUE );
	}
}

HRESULT DxEffectNeon::FrameMove ( float fTime, float fElapsedTime )
{
//	m_vAddTex += m_vMoveTex*fElapsedTime;

	m_fTime += fElapsedTime;

	return S_OK;
}

HRESULT DxEffectNeon::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectNeon");

	HRESULT hr = S_OK;

	//	Note : 트렌스폼 설정.
	//
	DWORD			dwFVFSize;
	D3DXMATRIX		matWorld, matTemp;
	D3DXMATRIX		matIdentity;
	if ( pframeCur )	m_matFrameComb = pframeCur->matCombined;
	D3DXMatrixIdentity ( &matIdentity );

	hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &m_matFrameComb );

	dwFVFSize = D3DXGetFVFVertexSize ( OBJECT::FVF );

	DWORD		dwAttribld;
	D3DXVECTOR2 vMove = D3DXVECTOR2 ( 0.f, 0.f );

	for ( DWORD i=0; i<m_dwAttribTable; i++ )
	{
		// Get Texture 
		if( !m_pMaterials[i].pSrcTex )	TextureManager::GetTexture( m_pMaterials[i].szTexture, m_pMaterials[i].pSrcTex );
		if( !m_pMaterials[i].pNeonTex )	TextureManager::GetTexture( m_pMaterials[i].szNeonTex, m_pMaterials[i].pNeonTex );

		dwAttribld = m_pAttribTable[i].AttribId;

		if ( pframeCur )
			pd3dDevice->SetMaterial ( &pframeCur->pmsMeshs->rgMaterials[ dwAttribld ] );

		if ( m_pMaterials[ dwAttribld ].bUse )
		{
			//	기본
			vMove.x = m_pMaterials[ dwAttribld ].vMoveTex.x * m_fTime * m_pMaterials[ dwAttribld ].vScaleUV.x;
			vMove.y = m_pMaterials[ dwAttribld ].vMoveTex.y * m_fTime * m_pMaterials[ dwAttribld ].vScaleUV.y;
			DxImageMove ( pd3dDevice, m_pMaterials[ dwAttribld ].pNeonTex, 
						DxSurfaceTex::GetInstance().m_pTempSuf128, vMove, TRUE, D3DFVF_DIFFUSE, 
						m_pMaterials[ dwAttribld ].vColor, 
						m_pMaterials[ dwAttribld ].vScaleUV, 128 );

			pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pTempTex128 );

			// Note : StateBlock Capture
			m_pSB_SAVE->Capture();
			m_pSB->Apply();

			pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(OBJECT) );
			pd3dDevice->SetIndices ( m_pIB );
			pd3dDevice->SetFVF ( OBJECT::FVF );

			pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST,
												0,
												m_pAttribTable[i].VertexStart,
												m_pAttribTable[i].VertexCount,
												m_pAttribTable[i].FaceStart*3,
												m_pAttribTable[i].FaceCount
												);

			// Note : StateBlock Apply
			m_pSB_SAVE->Apply();
		}

		pd3dDevice->SetTexture ( 0, m_pMaterials[ dwAttribld ].pSrcTex );

		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

		pd3dDevice->SetStreamSource ( 0, m_pSrcVB, 0, sizeof(OBJECTNORMAL) );
		pd3dDevice->SetIndices ( m_pIB );
		pd3dDevice->SetFVF ( OBJECTNORMAL::FVF );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST,
											0,
											m_pAttribTable[i].VertexStart,
											m_pAttribTable[i].VertexCount,
											m_pAttribTable[i].FaceStart*3,
											m_pAttribTable[i].FaceCount
											);

		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	}

	PROFILE_END("DxEffectNeon");

	return S_OK;
}

HRESULT DxEffectNeon::DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	HRESULT	hr = S_OK;

	m_dwMaterials = pmsMeshs->cMaterials;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_NEON[m_dwMaterials];

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pmsMeshs->strTextureFiles[i].GetString () );

		if ( !strlen(m_pMaterials[i].szNeonTex) )
			StringCchCopy( m_pMaterials[i].szNeonTex, MAX_PATH, pmsMeshs->strTextureFiles[i].GetString () );

		TextureManager::LoadTexture ( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pSrcTex, 0, 0, TRUE );
		TextureManager::LoadTexture ( m_pMaterials[i].szNeonTex, pd3dDevice, m_pMaterials[i].pNeonTex, 0, 0, TRUE );
	}

	pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &m_dwAttribTable );
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE [ m_dwAttribTable ];

	pmsMeshs->m_pLocalMesh->GetAttributeTable( m_pAttribTable, &m_dwAttribTable );

	return hr;
}

HRESULT DxEffectNeon::CreateObject( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	struct VERTEX { D3DXVECTOR3 p; D3DXVECTOR3 n; D3DXVECTOR2 t; };
	VERTEX*	pVertices;
	WORD*	pIndices;

	OBJECT*	pDestVert;
	WORD*	pDestIndices;

	OBJECTNORMAL*	pNormalVert;

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( pmsMeshs->m_pLocalMesh )
	{	
		m_dwVertices	= pmsMeshs->m_pLocalMesh->GetNumVertices ( );
		m_dwFaces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );

		pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
		pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
	}
	else
	{
		m_dwVertices	= pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
		m_dwFaces		= pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;

		pmsMeshs->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pVertices );
		pmsMeshs->m_pDxOctreeMesh->LockIndexBuffer( (VOID**)&pIndices );
	}



	//	원본
	//
	SAFE_RELEASE ( m_pSrcVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECTNORMAL), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECTNORMAL::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );

	m_pSrcVB->Lock ( 0, 0, (VOID**)&pNormalVert, 0 );

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pNormalVert[i].vPos		= pVertices[i].p;
		pNormalVert[i].vNor		= pVertices[i].n;
		pNormalVert[i].vTex		= pVertices[i].t;
	}
	m_pSrcVB->Unlock ();



	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECT), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECT::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );

	m_pVB->Lock ( 0, 0, (VOID**)&pDestVert, 0 );

	m_vMax = m_vMin = pVertices[0].p;	// 초기값 셋팅

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pDestVert[i].vPos		= pVertices[i].p;
		pDestVert[i].vTex		= pVertices[i].t;

		m_vMax.x = (pVertices[i].p.x > m_vMax.x) ? pVertices[i].p.x : m_vMax.x ;
		m_vMax.y = (pVertices[i].p.y > m_vMax.y) ? pVertices[i].p.y : m_vMax.y ;
		m_vMax.z = (pVertices[i].p.z > m_vMax.z) ? pVertices[i].p.z : m_vMax.z ;

		m_vMin.x = (pVertices[i].p.x < m_vMin.x) ? pVertices[i].p.x : m_vMin.x ;
		m_vMin.y = (pVertices[i].p.y < m_vMin.y) ? pVertices[i].p.y : m_vMin.y ;
		m_vMin.z = (pVertices[i].p.z < m_vMin.z) ? pVertices[i].p.z : m_vMin.z ;
	}
	m_pVB->Unlock ();

	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( 3*m_dwFaces*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	m_pIB->Lock ( 0, 0, (VOID**)&pDestIndices, 0 );
	for ( DWORD i=0; i<m_dwFaces*3; i++ )
	{
		pDestIndices[i] = pIndices[i];
	}
	m_pIB->Unlock ();


	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( pmsMeshs->m_pLocalMesh )
	{	
		pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
		pmsMeshs->m_pLocalMesh->UnlockIndexBuffer();
	}
	else
	{
		pmsMeshs->m_pDxOctreeMesh->UnlockVertexBuffer ( );
		pmsMeshs->m_pDxOctreeMesh->UnlockIndexBuffer ( );
	}

	ChangeObject ( pd3dDevice, -0.02f );						// 스케일을 작게 만든다.

	return S_OK;
}

HRESULT	DxEffectNeon::ChangeObject ( LPDIRECT3DDEVICEQ pd3dDevice, float fScale )
{
	if ( !m_pSrcVB || !m_pIB || !m_pVB )	return S_OK;

	WORD wVertex;

	OBJECT*			pDestVertices;
	OBJECTNORMAL*	pVertices;
	WORD*			pIndices;
	D3DXVECTOR3*	pTempNormal;
	D3DXVECTOR3		vTempNormal;

	//	Note : 임시 저장소 생성 및 초기화
	//
	pTempNormal = new D3DXVECTOR3[m_dwVertices];
	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pTempNormal[i] = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
	}

	//	임시 저장소에 값 넣기
	//
	m_pSrcVB->Lock ( 0, 0, (VOID**)&pVertices, NULL );
	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, NULL );

	for ( DWORD i=0; i<m_dwFaces; i++ )
	{
		wVertex = pIndices[i*3+0];
		pTempNormal[wVertex] += pVertices[wVertex].vNor;

		wVertex = pIndices[i*3+1];
		pTempNormal[wVertex] += pVertices[wVertex].vNor;

		wVertex = pIndices[i*3+2];
		pTempNormal[wVertex] += pVertices[wVertex].vNor;
	}

	m_pIB->Unlock ();

	//	임시 저장소에 있는 값 정리하기
	//
	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		vTempNormal = pTempNormal[i];
		D3DXVec3Normalize ( &vTempNormal, &vTempNormal );
		pTempNormal[i] = D3DXVECTOR3 ( vTempNormal.x*fScale, vTempNormal.y*fScale, vTempNormal.z*fScale );
	}

	////	Note : 변환값 넣기
	////	
	m_pVB->Lock ( 0, 0, (VOID**)&pDestVertices, NULL );
	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pDestVertices[i].vPos = pVertices[i].vPos + pTempNormal[i];
	}
	m_pVB->Unlock ();
	m_pSrcVB->Unlock ();

	//	Note : 정리
	//
	SAFE_DELETE_ARRAY ( pTempNormal );

	return S_OK;
}

void DxEffectNeon::SaveBuffer ( CSerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(OBJECT)*m_dwVertices
			+sizeof(OBJECTNORMAL)*m_dwVertices
			+sizeof(DWORD)+sizeof(WORD)*m_dwFaces*3
			+sizeof(D3DXVECTOR2)*m_dwVertices
			+sizeof(DWORD)+sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable
			+sizeof(DWORD)+sizeof(DXMATERIAL_NEON)*m_dwMaterials;

	//	버텍스 버퍼
	SFile << m_dwVertices;
	PBYTE pbPoints;
	m_pVB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	SFile.WriteBuffer ( pbPoints, sizeof(OBJECT)*m_dwVertices );
	m_pVB->Unlock ();

	//	버텍스 버퍼 노멀
	m_pSrcVB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	SFile.WriteBuffer ( pbPoints, sizeof(OBJECTNORMAL)*m_dwVertices );
	m_pSrcVB->Unlock ();

	//	인덱스 버퍼
	SFile << m_dwFaces;
	m_pIB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	SFile.WriteBuffer ( pbPoints, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock ();

	//
	SFile << m_dwAttribTable;
	SFile.WriteBuffer ( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable );

	//
	SFile << m_dwMaterials;
	SFile.WriteBuffer ( m_pMaterials, sizeof(DXMATERIAL_NEON)*m_dwMaterials );
}

void DxEffectNeon::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	OBJECT*		pTemp;
	WORD*		 pIndices;
	OBJECT*		pDestTemp;
	WORD*		 pDestIndices;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.


	// 버텍스 버퍼
	SFile >> dwBuffSize;
	if ( dwBuffSize >= 1 )
	{
		pTemp = new OBJECT[dwBuffSize];
		SFile.ReadBuffer ( pTemp, sizeof(OBJECT)*dwBuffSize );
	}

	//	Note : VB Size
	//
	m_dwVertices = dwBuffSize;

	//	Note : Base Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECT), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECT::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );

	m_pVB->Lock( 0, 0, (VOID**)&pDestTemp, NULL );
	memcpy ( pDestTemp, pTemp, sizeof(OBJECT)*dwBuffSize );
	m_pVB->Unlock ();
	
	SAFE_DELETE_ARRAY(pTemp);

	{
		OBJECTNORMAL*		pTemp;
		OBJECTNORMAL*		pDestTemp;

		//	버텍스 버퍼 노멀
		if ( dwBuffSize >= 1 )
		{
			pTemp = new OBJECTNORMAL[dwBuffSize];
			SFile.ReadBuffer ( pTemp, sizeof(OBJECTNORMAL)*dwBuffSize );
		}

		//	Note : VB Size
		//
		m_dwVertices = dwBuffSize;

		//	Note : Base Vertex Buffer 복제.
		//
		SAFE_RELEASE ( m_pSrcVB );
		pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECTNORMAL), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECTNORMAL::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );

		m_pSrcVB->Lock( 0, 0, (VOID**)&pDestTemp, NULL );
		memcpy ( pDestTemp, pTemp, sizeof(OBJECTNORMAL)*dwBuffSize );
		m_pSrcVB->Unlock ();
		
		SAFE_DELETE_ARRAY(pTemp);
	}



	// 인덱스 버퍼
	SFile >> dwBuffSize;
	if ( dwBuffSize >= 1 )
	{
		pIndices = new WORD[dwBuffSize*3];
		SFile.ReadBuffer ( pIndices, sizeof(WORD)*dwBuffSize*3 );
	}

	//	Note : IB Size
	//
	m_dwFaces = dwBuffSize;

	//	Note : Base Index Buffer 복제.
	//
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	m_pIB->Lock( 0, 0, (VOID**)&pDestIndices, NULL );
	memcpy ( pDestIndices, pIndices, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock ();
	
	SAFE_DELETE_ARRAY(pIndices);


	//
	SFile >> m_dwAttribTable;
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTable];
	SFile.ReadBuffer ( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable );

	//
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_NEON[m_dwMaterials];
	SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_NEON)*m_dwMaterials );

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].pSrcTex = NULL;
		m_pMaterials[i].pNeonTex = NULL;
		TextureManager::LoadTexture ( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pSrcTex, 0, 0, TRUE );
		TextureManager::LoadTexture ( m_pMaterials[i].szNeonTex, pd3dDevice, m_pMaterials[i].pNeonTex, 0, 0, TRUE );
	}
}

void DxEffectNeon::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	PBYTE pbBuff;
	PBYTE pbPoints;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.


	// 버텍스 버퍼
	SFile >> dwBuffSize;
	pbBuff = new BYTE[sizeof(OBJECT)*dwBuffSize];
	SFile.ReadBuffer ( pbBuff, sizeof(OBJECT)*dwBuffSize );

	//	Note : VB Size
	//
	m_dwVertices = dwBuffSize;

	//	Note : Base Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECT), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECT::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );

	m_pVB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	memcpy ( pbPoints, pbBuff, sizeof(OBJECT)*dwBuffSize );
	m_pVB->Unlock ();
	
	SAFE_DELETE(pbBuff);

	// 버텍스 버퍼 노멀
	pbBuff = new BYTE[sizeof(OBJECTNORMAL)*dwBuffSize];
	SFile.ReadBuffer ( pbBuff, sizeof(OBJECTNORMAL)*dwBuffSize );

	//	Note : VB Size
	//
	m_dwVertices = dwBuffSize;

	//	Note : Base Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pSrcVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECTNORMAL), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECTNORMAL::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );

	m_pSrcVB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	memcpy ( pbPoints, pbBuff, sizeof(OBJECTNORMAL)*dwBuffSize );
	m_pSrcVB->Unlock ();
	
	SAFE_DELETE(pbBuff);



	// 인덱스 버퍼
	SFile >> dwBuffSize;
	pbBuff = new BYTE[sizeof(WORD)*dwBuffSize*3];
	SFile.ReadBuffer ( pbBuff, sizeof(WORD)*dwBuffSize*3 );

	//	Note : IB Size
	//
	m_dwFaces = dwBuffSize;

	//	Note : Base Index Buffer 복제.
	//
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	m_pIB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	memcpy ( pbPoints, pbBuff, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock ();
	
	SAFE_DELETE(pbBuff);


	//
	SFile >> m_dwAttribTable;
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	//pbBuff = new BYTE[sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable];
	//SFile.ReadBuffer ( pbBuff, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable );
	//m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTable];
	//memcpy ( m_pAttribTable, pbBuff, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTable];
	SFile.ReadBuffer ( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable );

	//
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_NEON[m_dwMaterials];
	SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_NEON)*m_dwMaterials );

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].pSrcTex = NULL;
		m_pMaterials[i].pNeonTex = NULL;
		TextureManager::LoadTexture ( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pSrcTex, 0, 0, TRUE );
		TextureManager::LoadTexture ( m_pMaterials[i].szNeonTex, pd3dDevice, m_pMaterials[i].pNeonTex, 0, 0, TRUE );
	}
}

HRESULT DxEffectNeon::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	DxEffectNeon* pSrcEff = (DxEffectNeon*) pSrcEffect;
	GASSERT(pSrcEff->m_pVB);
	GASSERT(pSrcEff->m_pIB);
	GASSERT(pd3dDevice);

	m_dwVertices = pSrcEff->m_dwVertices;
	m_dwFaces	= pSrcEff->m_dwFaces;

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);



	//	Note : Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECT), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECT::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );

	PBYTE pbSrcPoints, pbDesPoints;
	hr = pSrcEff->m_pVB->Lock ( 0, 0, (VOID**)&pbSrcPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pVB->Lock ( 0, 0, (VOID**)&pbDesPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(OBJECT)*m_dwVertices );

	pSrcEff->m_pVB->Unlock ();

	m_pVB->Unlock ();


	//	Note : Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pSrcVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(OBJECTNORMAL), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, OBJECTNORMAL::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );

	hr = pSrcEff->m_pSrcVB->Lock ( 0, 0, (VOID**)&pbSrcPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pSrcVB->Lock ( 0, 0, (VOID**)&pbDesPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(OBJECTNORMAL)*m_dwVertices );

	pSrcEff->m_pSrcVB->Unlock ();

	m_pSrcVB->Unlock ();


	//	Note : Index Buffer 복제.
	//
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	hr = pSrcEff->m_pIB->Lock ( 0, 0, (VOID**)&pbSrcPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pIB->Lock ( 0, 0, (VOID**)&pbDesPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(WORD)*m_dwFaces*3 );

	pSrcEff->m_pIB->Unlock ();
	m_pIB->Unlock ();


	//	Note : AttribTable 복제.
	//
	m_dwAttribTable = pSrcEff->m_dwAttribTable;
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE[ m_dwAttribTable ];

	memcpy ( m_pAttribTable, pSrcEff->m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTable );


	//	Note : DXMATERIAL 복제.
	//
	m_dwMaterials = pSrcEff->m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_NEON[ m_dwMaterials ];

	memcpy ( m_pMaterials, pSrcEff->m_pMaterials, sizeof(DXMATERIAL_NEON)*m_dwMaterials );

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].pSrcTex = NULL;
		m_pMaterials[i].pNeonTex = NULL;
		TextureManager::LoadTexture ( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pSrcTex, 0, 0, TRUE );
		TextureManager::LoadTexture ( m_pMaterials[i].szNeonTex, pd3dDevice, m_pMaterials[i].pNeonTex, 0, 0, TRUE );
	}

	return S_OK;


E_ERROR:
	pSrcEff->m_pVB->Unlock ();
	pSrcEff->m_pSrcVB->Unlock ();
	pSrcEff->m_pIB->Unlock ();

	m_pVB->Unlock ();
	m_pSrcVB->Unlock ();
	m_pIB->Unlock ();

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pSrcVB);
	SAFE_RELEASE(m_pIB);

	SAFE_DELETE_ARRAY(m_pAttribTable);
	SAFE_DELETE_ARRAY(m_pMaterials);


	return E_FAIL;
}

HRESULT DxEffectNeon::SetBoundBox ()
{
	OBJECT*	pVertices;

	m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	m_vMax = m_vMin = pVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		m_vMax.x = (pVertices[i].vPos.x > m_vMax.x) ? pVertices[i].vPos.x : m_vMax.x ;
		m_vMax.y = (pVertices[i].vPos.y > m_vMax.y) ? pVertices[i].vPos.y : m_vMax.y ;
		m_vMax.z = (pVertices[i].vPos.z > m_vMax.z) ? pVertices[i].vPos.z : m_vMax.z ;

		m_vMin.x = (pVertices[i].vPos.x < m_vMin.x) ? pVertices[i].vPos.x : m_vMin.x ;
		m_vMin.y = (pVertices[i].vPos.y < m_vMin.y) ? pVertices[i].vPos.y : m_vMin.y ;
		m_vMin.z = (pVertices[i].vPos.z < m_vMin.z) ? pVertices[i].vPos.z : m_vMin.z ;
	}
	m_pVB->Unlock ();

	return S_OK;
}

void DxEffectNeon::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax;
	vMin = m_vMin;

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	COLLISION::TransformAABB( vMax, vMin, m_matFrameComb );
}


BOOL DxEffectNeon::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3  vCenter;
	vCenter.x = 0.f;
	vCenter.y = 0.f;
	vCenter.z = 0.f;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}

