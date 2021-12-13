// DxEffectShadow.cpp: implementation of the DxEffectShadowHW class.
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
#include "./DxFrameMesh.h"
#include "./DxSetLandMan.h"

#include "./NsOptimizeMesh.h"

#include "./DxEffectShadow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									D	x		S	h	a	d	o	w		A	A	B	B
// -----------------------------------------------------------------------------------------------------------------------------------------
DxShadowAABB::DxShadowAABB() : 
	m_pTree(NULL)
{
}

DxShadowAABB::~DxShadowAABB()
{
	CleanUp ();
}

void DxShadowAABB::CleanUp()
{
	SAFE_DELETE ( m_pTree );
}

void DxShadowAABB::Create ( LPDIRECT3DDEVICEQ pd3dDevice, VERTEX* pVert, WORD* pIndex, DWORD dwFace, D3DXMATRIX& matWorld )
{
	CleanUp();

	D3DXVECTOR3* pVertex = new D3DXVECTOR3[dwFace*3];
	for ( DWORD i=0; i<dwFace; ++i )
	{
		D3DXVec3TransformCoord ( &pVertex[i*3+0], &pVert[pIndex[i*3+0]].vPos, &matWorld );
		D3DXVec3TransformCoord ( &pVertex[i*3+1], &pVert[pIndex[i*3+1]].vPos, &matWorld );
		D3DXVec3TransformCoord ( &pVertex[i*3+2], &pVert[pIndex[i*3+2]].vPos, &matWorld );
	}

	// Note : Octree 형식으로 분할
	NSSHADOW::MakeShadow ( m_pTree, pVertex, dwFace );

	// Note : OcMesh 만들기
	NSSHADOW::MakeMesh ( pd3dDevice, m_pTree );

	// Note : OcMesh 최적화
	NSSHADOW::MakeOptimizeMesh ( pd3dDevice, m_pTree );

	// Note : VB와 IB로 변환하기.
	NSSHADOW::MakeVBIB ( pd3dDevice, m_pTree );

	SAFE_DELETE_ARRAY ( pVertex );
}

void DxShadowAABB::CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree )
{
	CleanUp();

	CloneTree ( pd3dDevice, m_pTree, pTree );
}

void DxShadowAABB::CloneTree ( LPDIRECT3DDEVICEQ pd3dDevice, PDXSHADOWTREE& pDest, DxShadowTree* pSrc )
{
	if ( !pSrc )	return;

	pDest = new DxShadowTree;
	pDest->CloneData ( pd3dDevice, pSrc );

	if ( pSrc->m_pLeftChild )
	{
		CloneTree ( pd3dDevice, pDest->m_pLeftChild, pSrc->m_pLeftChild );
	}
	if ( pSrc->m_pRightChild )
	{
		CloneTree ( pd3dDevice, pDest->m_pRightChild, pSrc->m_pRightChild );
	}
}

void DxShadowAABB::MakeStaticShadowUV ( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize )
{
	MakeStaticShadowUV ( m_pTree, vMin, vMax, fSize );
}

void DxShadowAABB::MakeStaticShadowUV ( DxShadowTree* pTree, const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const float fSize )
{
	if ( !pTree )	return;

	if ( pTree->IsStaticVB() )
	{
		pTree->MakeStaticShadowUV ( vMin, vMax, fSize );
		return;
	}

	if ( pTree->m_pLeftChild )	MakeStaticShadowUV ( pTree->m_pLeftChild, vMin, vMax, fSize );
	if ( pTree->m_pRightChild )	MakeStaticShadowUV ( pTree->m_pRightChild, vMin, vMax, fSize );
}

void DxShadowAABB::RenderChar ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LPDIRECT3DTEXTUREQ pTex, const D3DXMATRIX& matDirect, const BOOL bDynamicLoad )
{
	RenderChar ( pd3dDevice, sCV, m_pTree, pTex, matDirect, bDynamicLoad );
}

void DxShadowAABB::RenderStatic ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LPDIRECT3DTEXTUREQ pTex, const BOOL bDynamicLoad )
{
	RenderStatic ( pd3dDevice, sCV, m_pTree, pTex, bDynamicLoad );
}

void DxShadowAABB::RenderChar ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxShadowTree* pTree, LPDIRECT3DTEXTUREQ pTex, const D3DXMATRIX& matDirect, const BOOL bDynamicLoad )
{
	if ( !pTree )	return;

	if ( !COLLISION::IsCollisionVolume ( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	if ( pTree->IsCharVB() )
	{
		if ( bDynamicLoad )
		{
			if ( pTree->IsLoad() )	// 로딩 되어 있을 때만 렌더
			{
				pTree->MakeCharShadowUV ( matDirect );
				pTree->CharShadowDraw ( pd3dDevice, pTex );
			}
		}
		else
		{
			pTree->MakeCharShadowUV ( matDirect );
			pTree->CharShadowDraw ( pd3dDevice, pTex );
		}
	}

	if ( pTree->m_pLeftChild )	RenderChar ( pd3dDevice, sCV, pTree->m_pLeftChild, pTex, matDirect, bDynamicLoad );
	if ( pTree->m_pRightChild )	RenderChar ( pd3dDevice, sCV, pTree->m_pRightChild, pTex, matDirect, bDynamicLoad );
}

void DxShadowAABB::RenderStatic ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxShadowTree* pTree, LPDIRECT3DTEXTUREQ pTex, const BOOL bDynamicLoad )
{
	if ( !pTree )	return;

	if ( !COLLISION::IsCollisionVolume ( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

	if ( pTree->IsStaticVB() )
	{
		if ( bDynamicLoad )
		{
			if ( pTree->IsLoad() )	// 로딩 되어 있을 때만 렌더
			{
				pTree->StaticShadowDraw ( pd3dDevice, pTex );
			}
		}
		else
		{
			pTree->StaticShadowDraw ( pd3dDevice, pTex );
		}
	}

	if ( pTree->m_pLeftChild )	RenderStatic ( pd3dDevice, sCV, pTree->m_pLeftChild, pTex, bDynamicLoad );
	if ( pTree->m_pRightChild )	RenderStatic ( pd3dDevice, sCV, pTree->m_pRightChild, pTex, bDynamicLoad );
}

void DxShadowAABB::Save ( CSerialFile& SFile )
{
	Save ( SFile, m_pTree );
}

void DxShadowAABB::Save ( CSerialFile& SFile, DxShadowTree* pTree )
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

void DxShadowAABB::Load ( CSerialFile& SFile )
{
	Load ( SFile, m_pTree );
}

void DxShadowAABB::Load ( CSerialFile& SFile, PDXSHADOWTREE& pTree )
{
	BOOL bUse = TRUE;
	SFile >> bUse;

	if ( bUse )
	{
		pTree = new DxShadowTree;
		pTree->Load ( SFile );
	}
	else
	{
		return;
	}

	Load ( SFile, pTree->m_pLeftChild );
	Load ( SFile, pTree->m_pRightChild );
}

void DxShadowAABB::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV )
{
	DynamicLoad ( pd3dDevice, SFile, sCV, m_pTree );
}

void DxShadowAABB::DynamicLoad ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const CLIPVOLUME &sCV, DxShadowTree* pTree )
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
namespace NSSHADOW
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

	void MakeOctree ( PDXSHADOWTREE& pTree )
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

			pTree->m_pLeftChild = new DxShadowTree;
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

			pTree->m_pRightChild = new DxShadowTree;
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

	void MakeShadow ( PDXSHADOWTREE& pTree, D3DXVECTOR3* pVertex, DWORD dwFace )
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

		pTree = new DxShadowTree;
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




	void MakeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->m_pPosSRC )
		{
			SAFE_RELEASE ( pTree->m_pOcMesh );
			D3DXCreateMeshFVF ( pTree->m_dwFaceNUM, pTree->m_dwFaceNUM*3, D3DXMESH_MANAGED, VERTEX::FVF, pd3dDevice, &pTree->m_pOcMesh );
			if ( !pTree->m_pOcMesh )	return;

			VERTEX* pVertices;
			pTree->m_pOcMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD i=0; i<pTree->m_dwFaceNUM*3; ++i )
			{
				pVertices[i].vPos = pTree->m_pPosSRC[i];
				pVertices[i].vNor = D3DXVECTOR3 (0.f,1.f,0.f);
				pVertices[i].vTex = D3DXVECTOR2 (0.f,0.f);
			}
			pTree->m_pOcMesh->UnlockVertexBuffer();

			WORD* pIndices;
			pTree->m_pOcMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
			for ( DWORD i=0; i<pTree->m_dwFaceNUM*3; ++i )
			{
				pIndices[i] = (WORD)i;
			}
			pTree->m_pOcMesh->UnlockIndexBuffer();

			SAFE_DELETE_ARRAY ( pTree->m_pPosSRC );

			return;
		}

		if ( pTree->m_pLeftChild )	MakeMesh ( pd3dDevice, pTree->m_pLeftChild );
		if ( pTree->m_pRightChild )	MakeMesh ( pd3dDevice, pTree->m_pRightChild );
	}




	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree )
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


	void MakeVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, DxShadowTree* pTree )
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

//	void SaveOctree ( CSerialFile& SFile, OBJOCTree* pTree );
//	void LoadOctree ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, POBJOCTREE& pTree );
};

