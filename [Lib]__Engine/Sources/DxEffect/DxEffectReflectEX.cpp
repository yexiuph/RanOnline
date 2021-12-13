// DxEffectReflectEX.cpp: 
//
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

#include "./DxEffectReflect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									D	x		W	a	t	e	r		T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxReflectTree::CleanUp()
{
	SAFE_RELEASE ( m_pOcMesh );

	SAFE_DELETE_ARRAY ( m_pPosSRC );
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );
}

void DxReflectTree::CreateSampleMesh ( LPDIRECT3DDEVICEQ pd3dDevice )
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

void DxReflectTree::CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice )
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
	m_pOcMesh->UnlockIndexBuffer();
	m_pIB->Unlock ();

	SAFE_RELEASE ( m_pOcMesh );
}

void DxReflectTree::CreateVB_TexUV ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* pPos )
{
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( sizeof(D3DREFLECTVERTEX)*m_dwVertNUM, 0L, D3DREFLECTVERTEX::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );
	D3DREFLECTVERTEX* pVert;
	m_pVB->Lock( 0, 0, (VOID**)&pVert, 0L );
	for ( DWORD i=0; i<m_dwVertNUM; ++i )
	{
		pVert[i].vPos = pPos[i];
		pVert[i].vTex1 = D3DXVECTOR2( 0.f, 0.f );
	}
	m_pVB->Unlock ();
}

void DxReflectTree::CloneData ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pSrc )
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

void DxReflectTree::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pVB || !m_pIB )	return;

	pd3dDevice->SetIndices( m_pIB );
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DREFLECTVERTEX) );

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwVertNUM, 0, m_dwFaceNUM );
}

void DxReflectTree::Save ( CSerialFile& SFile )
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

void DxReflectTree::Load ( CSerialFile& SFile )
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

void DxReflectTree::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
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
DxReflectAABB::DxReflectAABB() : 
	m_pTree(NULL)
{
}

DxReflectAABB::~DxReflectAABB()
{
	CleanUp ();
}

void DxReflectAABB::CleanUp()
{
	SAFE_DELETE ( m_pTree );
}

void DxReflectAABB::Create ( LPDIRECT3DDEVICEQ pd3dDevice, BYTE* pVert, WORD* pIndex, DWORD dwFace, D3DXMATRIX& matWorld, const DWORD dwFVF )
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
	NSREFLECT::DivideMesh ( m_pTree, pVertex, dwFace );

	// Note : OcMesh 만들기
	NSREFLECT::MakeMesh ( pd3dDevice, m_pTree );

	// Note : OcMesh 최적화
	NSREFLECT::MakeOptimizeMesh ( pd3dDevice, m_pTree );

	// Note : VB와 IB로 변환하기.
	NSREFLECT::MakeVBIB ( pd3dDevice, m_pTree );

	SAFE_DELETE_ARRAY ( pVertex );
}

void DxReflectAABB::CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree )
{
	CleanUp();

	CloneTree ( pd3dDevice, m_pTree, pTree );
}

void DxReflectAABB::CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, PDXREFLECTTREE& pSrc, DxReflectTree* pDest )
{
	if ( !pDest )	return;

	pSrc = new DxReflectTree;
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

void DxReflectAABB::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const BOOL bDynamicLoad )
{
	Render ( pd3dDevice, sCV, m_pTree, bDynamicLoad );
}

void DxReflectAABB::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxReflectTree* pTree, const BOOL bDynamicLoad )
{
	if ( !pTree )	return;

	if ( !COLLISION::IsCollisionVolume ( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	if ( pTree->IsVB() )
	{
		if ( bDynamicLoad )
		{
			// 로딩 되어 있을 때만 렌더
			if ( pTree->IsLoad() )	pTree->Render ( pd3dDevice );
		}
		else	pTree->Render ( pd3dDevice );
	}

	if ( pTree->m_pLeftChild )	Render ( pd3dDevice, sCV, pTree->m_pLeftChild, bDynamicLoad );
	if ( pTree->m_pRightChild )	Render ( pd3dDevice, sCV, pTree->m_pRightChild, bDynamicLoad );
}

void DxReflectAABB::Save ( CSerialFile& SFile )
{
	Save ( SFile, m_pTree );
}

void DxReflectAABB::Save ( CSerialFile& SFile, DxReflectTree* pTree )
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

void DxReflectAABB::Load ( CSerialFile& SFile )
{
	Load ( SFile, m_pTree );
}

void DxReflectAABB::Load ( CSerialFile& SFile, PDXREFLECTTREE& pTree )
{
	BOOL bUse = TRUE;
	SFile >> bUse;

	if ( bUse )
	{
		pTree = new DxReflectTree;
		pTree->Load ( SFile );
	}
	else
	{
		return;
	}

	Load ( SFile, pTree->m_pLeftChild );
	Load ( SFile, pTree->m_pRightChild );
}

void DxReflectAABB::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV )
{
	DynamicLoad ( pd3dDevice, SFile, sCV, m_pTree );
}

void DxReflectAABB::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV, DxReflectTree* pTree )
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
namespace NSREFLECT
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

	void MakeOctree ( PDXREFLECTTREE& pTree )
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

			pTree->m_pLeftChild = new DxReflectTree;
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

			pTree->m_pRightChild = new DxReflectTree;
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

	void DivideMesh ( PDXREFLECTTREE& pTree, D3DXVECTOR3* pVertex, DWORD dwFace )
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

		pTree = new DxReflectTree;
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




	void MakeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree )
	{
		if ( !pTree )	return;

		pTree->CreateSampleMesh ( pd3dDevice );

		if ( pTree->m_pLeftChild )	MakeMesh ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeMesh ( pd3dDevice, pTree->m_pRightChild );
	}




	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree )
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


	void MakeVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, DxReflectTree* pTree )
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