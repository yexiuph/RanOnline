// Terrain0.cpp: implementation of the DxEffectRiver class.
//
//	UPDATE [03.02.20] : 새롭게 물 효과를 바꿈
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxLoadShader.h"
#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"

#include "./SerialFile.h"
#include "./Collision.h"

#include "./DxEffectMan.h"
#include "./DxShadowMap.h"
#include "./DxEnvironment.h"
#include "./DxCubeMap.h"
#include "./DxSkyMan.h"

#include "./DxViewPort.h"

#include "./NsOptimizeMesh.h"

#include "./DxEffectRiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									D	x		W	a	t	e	r		T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxWaterTree::CleanUp()
{
	SAFE_RELEASE ( m_pOcMesh );

	SAFE_DELETE_ARRAY ( m_pPosSRC );
	SAFE_DELETE_ARRAY ( m_pTexUV );
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );
}

void DxWaterTree::CreateSampleMesh ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pPosSRC )	return;

	SAFE_RELEASE ( m_pOcMesh );
	D3DXCreateMeshFVF ( m_dwFaceNUM, m_dwFaceNUM*3, D3DXMESH_MANAGED, D3DFVF_XYZ, pd3dDevice, &m_pOcMesh );
	if ( !m_pOcMesh )	return;

	D3DXVECTOR3* pVertices;
	m_pOcMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
	for ( DWORD i=0; i<m_dwFaceNUM*3; ++i )
	{
		pVertices[i] = m_pPosSRC[i];
	}
	m_pOcMesh->UnlockVertexBuffer();

	WORD* pIndices;
	m_pOcMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
	for ( DWORD i=0; i<m_dwFaceNUM*3; ++i )
	{
		pIndices[i] = (WORD)i;
	}
	m_pOcMesh->UnlockIndexBuffer();

	SAFE_DELETE_ARRAY ( m_pPosSRC );
}

void DxWaterTree::CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_dwVertNUM = m_pOcMesh->GetNumVertices();
	m_dwFaceNUM = m_pOcMesh->GetNumFaces();

	//	Pos SRC
	SAFE_DELETE_ARRAY ( m_pPosSRC );
	m_pPosSRC = new D3DXVECTOR3[m_dwVertNUM];
	D3DXVECTOR3*	pVertSRC;
	m_pOcMesh->LockVertexBuffer ( 0L, (VOID**)&pVertSRC );
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		m_pPosSRC[i] = pVertSRC[i];
	}
	m_pOcMesh->UnlockVertexBuffer ();

	// VB와 UV 생성
	CreateVB_TexUV ( pd3dDevice, m_pPosSRC );

	//	Index Buffer
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( sizeof(WORD)*m_dwFaceNUM*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	//	Index Buffer
	WORD*	pIndexSRC;
	WORD*	pIndexDEST;
	m_pIB->Lock ( 0, 0, (VOID**)&pIndexDEST, 0L );
	m_pOcMesh->LockIndexBuffer ( 0L, (VOID**)&pIndexSRC );
	for ( DWORD i=0; i<m_dwFaceNUM*3; ++i )
	{
		pIndexDEST[i] = pIndexSRC[i];
	}
	m_pOcMesh->UnlockIndexBuffer ();
	m_pIB->Unlock ();

	SAFE_RELEASE ( m_pOcMesh );
}

void DxWaterTree::CreateVB_TexUV ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* pPos )
{
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( sizeof(D3DWATERVERTEX)*m_dwVertNUM, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, D3DWATERVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
	D3DWATERVERTEX* pVert;
	m_pVB->Lock( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		pVert[i].vPos = pPos[i];
		pVert[i].vColor = 0xffffffff;
		pVert[i].vTex1 = D3DXVECTOR2( 0.f, 0.f );
		pVert[i].vTex2 = D3DXVECTOR2( 0.f, 0.f );
	}
	m_pVB->Unlock ();

	// Tex UV 좌표 생성
	SAFE_DELETE_ARRAY ( m_pTexUV );
	m_pTexUV = new D3DXVECTOR2[m_dwVertNUM];
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		m_pTexUV[i].x = pPos[i].x - m_vMin.x;
		m_pTexUV[i].y = pPos[i].z - m_vMin.z;
	}
}

void DxWaterTree::CloneData ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pSrc )
{
	CleanUp();

	m_vMin = pSrc->m_vMin;
	m_vMax = pSrc->m_vMax;

	m_fDisX = pSrc->m_fDisX;
	m_fDisY = pSrc->m_fDisY;
	m_fDisZ = pSrc->m_fDisZ;

	m_dwVertNUM = pSrc->m_dwVertNUM;
	m_dwFaceNUM = pSrc->m_dwFaceNUM;

	if ( m_dwVertNUM && m_dwFaceNUM )
	{
		// 위치 정보
		SAFE_DELETE_ARRAY ( m_pPosSRC );
		m_pPosSRC = new D3DXVECTOR3[m_dwVertNUM];
		memcpy ( m_pPosSRC, pSrc->m_pPosSRC, sizeof(D3DXVECTOR3)*m_dwVertNUM );

		// VB 생성
		CreateVB_TexUV ( pd3dDevice, m_pPosSRC );

		//	Index Buffer
		pd3dDevice->CreateIndexBuffer ( sizeof(WORD)*m_dwFaceNUM*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

		//	Index Buffer
		WORD*	pIndexSRC;
		WORD*	pIndexDEST;
		m_pIB->Lock( 0, 0, (VOID**)&pIndexDEST, 0L );
		pSrc->m_pIB->Lock ( 0, 0, (VOID**)&pIndexSRC, 0L );
		for ( DWORD i=0; i<m_dwFaceNUM*3; ++i )
		{
			pIndexDEST[i] = pIndexSRC[i];
		}
		pSrc->m_pIB->Unlock ();
		m_pIB->Unlock ();
	}
}

void DxWaterTree::FrameMove ( const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1 )
{
	if ( !m_pVB )	return;

	D3DWATERVERTEX* pVert;
	m_pVB->Lock( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		pVert[i].vPos	= m_pPosSRC[i];
		pVert[i].vColor = dwColor;
		pVert[i].vTex1.x = (m_pTexUV[i].x*fScale) + vAddTex1.x;
		pVert[i].vTex1.y = (m_pTexUV[i].y*fScale) + vAddTex1.y;
		pVert[i].vTex2	= D3DXVECTOR2( 0.f, 0.f );
	}
	m_pVB->Unlock();
}

void DxWaterTree::FrameMove ( const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1, const D3DXVECTOR2& vAddTex2 )
{
	if ( !m_pVB )	return;

	D3DWATERVERTEX* pVert;
	m_pVB->Lock( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		pVert[i].vPos	= m_pPosSRC[i];
		pVert[i].vColor = dwColor;
		pVert[i].vTex1.x = (m_pTexUV[i].x*fScale) + vAddTex1.x;
		pVert[i].vTex1.y = (m_pTexUV[i].y*fScale) + vAddTex1.y;
		pVert[i].vTex2.x = ((1.f-m_pTexUV[i].x)*fScale) + vAddTex2.x;	//
		pVert[i].vTex2.y = ((1.f-m_pTexUV[i].y)*fScale) + vAddTex2.y;	// 
	}
	m_pVB->Unlock();
}

void DxWaterTree::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pVB || !m_pIB )	return;

	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(D3DWATERVERTEX) );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertNUM, 0, m_dwFaceNUM );
}

void DxWaterTree::Save ( CSerialFile& SFile )
{
	SFile << m_vMin;
	SFile << m_vMax;

	SFile << m_fDisX;
	SFile << m_fDisY;
	SFile << m_fDisZ;

	SFile << m_dwVertNUM;
	SFile << m_dwFaceNUM;

	DWORD dwBufferSize = sizeof(D3DXVECTOR3)*m_dwVertNUM + sizeof(WORD)*m_dwFaceNUM*3;

	SFile << dwBufferSize;

	if ( m_dwVertNUM && m_dwFaceNUM )
	{
		SFile.WriteBuffer ( m_pPosSRC, sizeof(D3DXVECTOR3)*m_dwVertNUM );

		WORD* pIndex;
		m_pIB->Lock ( 0, 0, (VOID**)&pIndex, 0L );
		SFile.WriteBuffer ( pIndex, sizeof(WORD)*m_dwFaceNUM*3 );
		m_pIB->Unlock ();
	}
}

void DxWaterTree::Load ( CSerialFile& SFile )
{
	SFile >> m_vMin;
	SFile >> m_vMax;

	SFile >> m_fDisX;
	SFile >> m_fDisY;
	SFile >> m_fDisZ;

	SFile >> m_dwVertNUM;
	SFile >> m_dwFaceNUM;

	DWORD dwBufferSize;
	SFile >> dwBufferSize;

	m_dwFileCur = SFile.GetfTell();
	SFile.SetOffSet ( m_dwFileCur+dwBufferSize );
}

void DxWaterTree::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	if ( IsLoad() )		return;
	if ( !m_dwFileCur )	return;

	SFile.SetOffSet ( m_dwFileCur );

	if ( m_dwVertNUM && m_dwFaceNUM )
	{
		SAFE_DELETE_ARRAY ( m_pPosSRC );
		m_pPosSRC = new D3DXVECTOR3[m_dwVertNUM];
		SFile.ReadBuffer ( m_pPosSRC, sizeof(D3DXVECTOR3)*m_dwVertNUM );

		SAFE_RELEASE ( m_pIB );
		pd3dDevice->CreateIndexBuffer ( sizeof(WORD)*m_dwFaceNUM*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
		WORD* pIndex;
		m_pIB->Lock ( 0, 0, (VOID**)&pIndex, 0L );
		SFile.ReadBuffer ( pIndex, sizeof(WORD)*m_dwFaceNUM*3 );
		m_pIB->Unlock ();

		// VB 생성
		CreateVB_TexUV ( pd3dDevice, m_pPosSRC );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									D	x		W	a	t	e	r		A	A	B	B
// -----------------------------------------------------------------------------------------------------------------------------------------
DxWaterAABB::DxWaterAABB() : 
	m_pTree(NULL)
{
}

DxWaterAABB::~DxWaterAABB()
{
	CleanUp ();
}

void DxWaterAABB::CleanUp()
{
	SAFE_DELETE ( m_pTree );
}

void DxWaterAABB::Create ( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVert, WORD* pIndex, DWORD dwFace, D3DXMATRIX& matWorld, const DWORD dwFVF )
{
	CleanUp();

	UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

	D3DXVECTOR3* pVector;
	D3DXVECTOR3* pVertex = new D3DXVECTOR3[dwFace*3];
	for ( DWORD i=0; i<dwFace; ++i )
	{
		pVector = (D3DXVECTOR3*) ( pVert + pIndex[i*3+0]*nSIZE );
		D3DXVec3TransformCoord ( &pVertex[i*3+0], pVector, &matWorld );
		pVector = (D3DXVECTOR3*) ( pVert + pIndex[i*3+1]*nSIZE );
		D3DXVec3TransformCoord ( &pVertex[i*3+1], pVector, &matWorld );
		pVector = (D3DXVECTOR3*) ( pVert + pIndex[i*3+2]*nSIZE );
		D3DXVec3TransformCoord ( &pVertex[i*3+2], pVector, &matWorld );
	}

	// Note : Octree 형식으로 분할
	NSWATER::DivideMesh ( m_pTree, pVertex, dwFace );

	// Note : OcMesh 만들기
	NSWATER::MakeMesh ( pd3dDevice, m_pTree );

	// Note : OcMesh 최적화
	NSWATER::MakeOptimizeMesh ( pd3dDevice, m_pTree );

	// Note : VB와 IB로 변환하기.
	NSWATER::MakeVBIB ( pd3dDevice, m_pTree );

	SAFE_DELETE_ARRAY ( pVertex );
}

void DxWaterAABB::CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree )
{
	CleanUp();

	CloneTree ( pd3dDevice, m_pTree, pTree );
}

void DxWaterAABB::CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, PDXWATERTREE& pSrc, DxWaterTree* pDest )
{
	if ( !pDest )	return;

	pSrc = new DxWaterTree;
	pSrc->CloneData ( pd3dDevice, pDest );

	if ( pDest->m_pLeftChild )
	{
		CloneTree ( pd3dDevice, pSrc->m_pLeftChild, pDest->m_pLeftChild );
	}
	if ( pDest->m_pRightChild )
	{
		CloneTree ( pd3dDevice, pSrc->m_pRightChild, pDest->m_pRightChild );
	}
}

void DxWaterAABB::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const BOOL bDynamicLoad,
						  const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1, const D3DXVECTOR2& vAddTex2 )
{
	Render ( pd3dDevice, sCV, m_pTree, bDynamicLoad, dwColor, fScale, vAddTex1, vAddTex2 );
}

void DxWaterAABB::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxWaterTree* pTree, const BOOL bDynamicLoad,
						  const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1, const D3DXVECTOR2& vAddTex2 )
{
	if ( !pTree )	return;

	if ( !COLLISION::IsCollisionVolume ( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	if ( pTree->IsVB() )
	{
		if ( bDynamicLoad )
		{
			if ( pTree->IsLoad() )	// 로딩 되어 있을 때만 렌더
			{
				pTree->FrameMove ( dwColor, fScale, vAddTex1, vAddTex2 );
				pTree->Render ( pd3dDevice );
			}
		}
		else
		{
			pTree->FrameMove ( dwColor, fScale, vAddTex1, vAddTex2 );
			pTree->Render ( pd3dDevice );
		}
	}

	if ( pTree->m_pLeftChild )	Render ( pd3dDevice, sCV, pTree->m_pLeftChild, bDynamicLoad, dwColor, fScale, vAddTex1, vAddTex2 );
	if ( pTree->m_pRightChild )	Render ( pd3dDevice, sCV, pTree->m_pRightChild, bDynamicLoad, dwColor, fScale, vAddTex1, vAddTex2 );
}

void DxWaterAABB::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const BOOL bDynamicLoad,
						const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1 )
{
	Render ( pd3dDevice, sCV, m_pTree, bDynamicLoad, dwColor, fScale, vAddTex1 );
}

void DxWaterAABB::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxWaterTree* pTree, const BOOL bDynamicLoad,
							const DWORD& dwColor, const float& fScale, const D3DXVECTOR2& vAddTex1 )
{
	if ( !pTree )	return;

	if ( !COLLISION::IsCollisionVolume ( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	if ( pTree->IsVB() )
	{
		if ( bDynamicLoad )
		{
			if ( pTree->IsLoad() )	// 로딩 되어 있을 때만 렌더
			{
				pTree->FrameMove ( dwColor, fScale, vAddTex1 );
				pTree->Render ( pd3dDevice );
			}
		}
		else
		{
			pTree->FrameMove ( dwColor, fScale, vAddTex1 );
			pTree->Render ( pd3dDevice );
		}
	}

	if ( pTree->m_pLeftChild )	Render ( pd3dDevice, sCV, pTree->m_pLeftChild, bDynamicLoad, dwColor, fScale, vAddTex1 );
	if ( pTree->m_pRightChild )	Render ( pd3dDevice, sCV, pTree->m_pRightChild, bDynamicLoad, dwColor, fScale, vAddTex1);
}

void DxWaterAABB::Save ( CSerialFile& SFile )
{
	Save ( SFile, m_pTree );
}

void DxWaterAABB::Save ( CSerialFile& SFile, DxWaterTree* pTree )
{
	if ( pTree )
	{
		SFile << (BOOL)TRUE;
		pTree->Save ( SFile );
	}
	else
	{
		SFile << (BOOL)FALSE;
		return;
	}

	Save ( SFile, pTree->m_pLeftChild );
	Save ( SFile, pTree->m_pRightChild );
}

void DxWaterAABB::Load ( CSerialFile& SFile )
{
	Load ( SFile, m_pTree );
}

void DxWaterAABB::Load ( CSerialFile& SFile, PDXWATERTREE& pTree )
{
	BOOL bUse = TRUE;
	SFile >> bUse;

	if ( bUse )
	{
		pTree = new DxWaterTree;
		pTree->Load ( SFile );
	}
	else
	{
		return;
	}

	Load ( SFile, pTree->m_pLeftChild );
	Load ( SFile, pTree->m_pRightChild );
}

void DxWaterAABB::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV )
{
	DynamicLoad ( pd3dDevice, SFile, sCV, m_pTree );
}

void DxWaterAABB::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV, DxWaterTree* pTree )
{
	if ( !pTree )	return;

	if ( !COLLISION::IsCollisionVolume ( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	pTree->DynamicLoad ( pd3dDevice, SFile );

	if ( pTree->m_pLeftChild )	DynamicLoad ( pd3dDevice, SFile, sCV, pTree->m_pLeftChild );
	if ( pTree->m_pRightChild )	DynamicLoad ( pd3dDevice, SFile, sCV, pTree->m_pRightChild );
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									N	S		S	H	A	D	O	W
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSWATER
{
	BOOL IsFaceFront ( D3DXVECTOR3* pSrc, const D3DXPLANE& sPlane )
	{
		float fResult;
		for ( DWORD i=0; i<3; ++i )
		{
			fResult = D3DXPlaneDotCoord ( &sPlane, &pSrc[i] );
			if ( fResult >= 0.f )	return TRUE;
		}
		return FALSE;
	}

	void DivideFace ( D3DXVECTOR3* pSrc, D3DXVECTOR3* pLeft, D3DXVECTOR3* pRight, DWORD dwFace, DWORD& dwLeft, DWORD& dwRight, const D3DXPLANE& sPlane )
	{
		for ( DWORD i=0; i<dwFace; ++i )
		{
			// Note : 면이 앞에 있다.		 면 단위 계산
			if ( IsFaceFront ( &pSrc[i*3], sPlane ) )
			{
				pLeft[dwLeft*3+0] = pSrc[i*3+0];
				pLeft[dwLeft*3+1] = pSrc[i*3+1];
				pLeft[dwLeft*3+2] = pSrc[i*3+2];
				++dwLeft;
			}
			else
			{
				pRight[dwRight*3+0] = pSrc[i*3+0];
				pRight[dwRight*3+1] = pSrc[i*3+1];
				pRight[dwRight*3+2] = pSrc[i*3+2];
				++dwRight;
			}
		}
	}

	float DISMAX = 1500.f;
	DWORD FACEMAX = 2000;

	void MakeOctree ( PDXWATERTREE& pTree )
	{
		// Note : 이건 완료된 데이터
		if ( (pTree->m_fDisX<DISMAX) && (pTree->m_fDisY<DISMAX) && (pTree->m_fDisZ<DISMAX) && (pTree->m_dwFaceNUM<FACEMAX) )
		{
			return;
		}

		// Note : 분할을 하자.
		DWORD	dwLeftFace = 0;
		DWORD	dwRightFace = 0;
		D3DXVECTOR3* pLeftVertex = new D3DXVECTOR3[pTree->m_dwFaceNUM*3];
		D3DXVECTOR3* pRightVertex = new D3DXVECTOR3[pTree->m_dwFaceNUM*3];

		D3DXPLANE sPlane;
		D3DXVECTOR3 vCenter = pTree->m_vMin;
		vCenter.x += (pTree->m_fDisX*0.5f);
		vCenter.y += (pTree->m_fDisY*0.5f);
		vCenter.z += (pTree->m_fDisZ*0.5f);
		if ( (pTree->m_fDisX>pTree->m_fDisY) && (pTree->m_fDisX>pTree->m_fDisZ) )	// X 길다.
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.z += 1.f;
			v2.y += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
		}
		else if (pTree->m_fDisZ>pTree->m_fDisY)									// Z 길다.
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.x += 1.f;
			v2.y += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
		}								
		else 
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.x += 1.f;
			v2.z += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );	
		}

		DivideFace ( pTree->m_pPosSRC, pLeftVertex, pRightVertex, pTree->m_dwFaceNUM, dwLeftFace, dwRightFace, sPlane );

		// Note : 필요 없어진것 삭제
		SAFE_DELETE_ARRAY ( pTree->m_pPosSRC );
		pTree->m_dwFaceNUM = 0;

		// Note : Left 생성
		if ( dwLeftFace )
		{
			D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
			for ( DWORD i=0; i<dwLeftFace*3; ++i )
			{
				if ( vMax.x < pLeftVertex[i].x )	vMax.x = pLeftVertex[i].x;
				if ( vMax.y < pLeftVertex[i].y )	vMax.y = pLeftVertex[i].y;
				if ( vMax.z < pLeftVertex[i].z )	vMax.z = pLeftVertex[i].z;

				if ( vMin.x > pLeftVertex[i].x )	vMin.x = pLeftVertex[i].x;
				if ( vMin.y > pLeftVertex[i].y )	vMin.y = pLeftVertex[i].y;
				if ( vMin.z > pLeftVertex[i].z )	vMin.z = pLeftVertex[i].z;
			}

			pTree->m_pLeftChild = new DxWaterTree;
			pTree->m_pLeftChild->m_vMax = vMax;
			pTree->m_pLeftChild->m_vMin = vMin;
			pTree->m_pLeftChild->m_fDisX = vMax.x - vMin.x;
			pTree->m_pLeftChild->m_fDisY = vMax.y - vMin.y;
			pTree->m_pLeftChild->m_fDisZ = vMax.z - vMin.z;
			pTree->m_pLeftChild->m_pPosSRC = pLeftVertex;
			pTree->m_pLeftChild->m_dwFaceNUM = dwLeftFace;

			MakeOctree ( pTree->m_pLeftChild );
		}
		else
		{
			SAFE_DELETE_ARRAY ( pLeftVertex );
		}

		// Note : Right 생성
		if ( dwRightFace )
		{
			D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
			for ( DWORD i=0; i<dwRightFace*3; ++i )
			{
				if ( vMax.x < pRightVertex[i].x )	vMax.x = pRightVertex[i].x;
				if ( vMax.y < pRightVertex[i].y )	vMax.y = pRightVertex[i].y;
				if ( vMax.z < pRightVertex[i].z )	vMax.z = pRightVertex[i].z;

				if ( vMin.x > pRightVertex[i].x )	vMin.x = pRightVertex[i].x;
				if ( vMin.y > pRightVertex[i].y )	vMin.y = pRightVertex[i].y;
				if ( vMin.z > pRightVertex[i].z )	vMin.z = pRightVertex[i].z;
			}

			pTree->m_pRightChild = new DxWaterTree;
			pTree->m_pRightChild->m_vMax = vMax;
			pTree->m_pRightChild->m_vMin = vMin;
			pTree->m_pRightChild->m_fDisX = vMax.x - vMin.x;
			pTree->m_pRightChild->m_fDisY = vMax.y - vMin.y;
			pTree->m_pRightChild->m_fDisZ = vMax.z - vMin.z;
			pTree->m_pRightChild->m_pPosSRC = pRightVertex;
			pTree->m_pRightChild->m_dwFaceNUM = dwRightFace;

			MakeOctree ( pTree->m_pRightChild );
		}
		else
		{
			SAFE_DELETE_ARRAY ( pRightVertex );
		}
	}

	void DivideMesh ( PDXWATERTREE& pTree, D3DXVECTOR3* pVertex, DWORD dwFace )
	{
		D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( DWORD i=0; i<dwFace*3; ++i )
		{
			if ( vMax.x < pVertex[i].x )	vMax.x = pVertex[i].x;
			if ( vMax.y < pVertex[i].y )	vMax.y = pVertex[i].y;
			if ( vMax.z < pVertex[i].z )	vMax.z = pVertex[i].z;

			if ( vMin.x > pVertex[i].x )	vMin.x = pVertex[i].x;
			if ( vMin.y > pVertex[i].y )	vMin.y = pVertex[i].y;
			if ( vMin.z > pVertex[i].z )	vMin.z = pVertex[i].z;
		}

		pTree = new DxWaterTree;
		pTree->m_vMax = vMax;
		pTree->m_vMin = vMin;
		pTree->m_fDisX = vMax.x - vMin.x;
		pTree->m_fDisY = vMax.y - vMin.y;
		pTree->m_fDisZ = vMax.z - vMin.z;
		pTree->m_pPosSRC = new D3DXVECTOR3[dwFace*3];
		memcpy ( pTree->m_pPosSRC, pVertex, sizeof(D3DXVECTOR3)*dwFace*3 );
		pTree->m_dwFaceNUM = dwFace;

		MakeOctree ( pTree );
	}




	void MakeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree )
	{
		if ( !pTree )	return;

		pTree->CreateSampleMesh ( pd3dDevice );

		if ( pTree->m_pLeftChild )	MakeMesh ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeMesh ( pd3dDevice, pTree->m_pRightChild );
	}




	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->m_pOcMesh )
		{
			NSOPTIMIZEMESH::MakeOptimizeMesh ( pd3dDevice, pTree->m_pOcMesh );
			return;
		}

		if ( pTree->m_pLeftChild )	MakeOptimizeMesh ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeOptimizeMesh ( pd3dDevice, pTree->m_pRightChild );
	}


	void MakeVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, DxWaterTree* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->m_pOcMesh )
		{
			pTree->CreateVBIB ( pd3dDevice );
			return;
		}

		if ( pTree->m_pLeftChild )	MakeVBIB ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeVBIB ( pd3dDevice, pTree->m_pRightChild );
	}
};