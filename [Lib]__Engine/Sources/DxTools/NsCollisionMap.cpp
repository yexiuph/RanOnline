#include "pch.h"
#include "./SerialFile.h"
#include "NsCollisionMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		F	a	c	e	T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxFaceTree::CleanUp()
{
	dwFace = 0;
	SAFE_DELETE_ARRAY ( pIndex );
	SAFE_DELETE_ARRAY ( pVertex );
}

void DxFaceTree::Save ( CSerialFile& SFile )
{
	SFile << vMax;
	SFile << vMin;

	SFile << dwFace;
	if ( dwFace )
	{
		SFile.WriteBuffer ( pVertex, sizeof(D3DXVECTOR3)*dwFace*3 );
		SFile.WriteBuffer ( pIndex, sizeof(WORD)*dwFace );
	}
}

void DxFaceTree::Load ( CSerialFile& SFile )
{
	CleanUp();

	SFile >> vMax;
	SFile >> vMin;

	SFile >> dwFace;
	if ( dwFace )
	{
		pVertex = new D3DXVECTOR3[dwFace*3];
		SFile.ReadBuffer ( pVertex, sizeof(D3DXVECTOR3)*dwFace*3 );

		pIndex = new WORD[dwFace];
		SFile.ReadBuffer ( pIndex, sizeof(WORD)*dwFace );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		A	A	B	B		C	o	l	l	i	s	i	o	n
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxAABBCollision::CleanUp()
{
	SAFE_DELETE ( m_pTree );

	m_mapIndexTex.clear();
}

void DxAABBCollision::MakeAABBCollision( DxFrameMesh* pFrameMesh )
{
	CleanUp();

	D3DXVECTOR3*	pVertex = NULL;
	WORD*			pIndex = NULL;
	DWORD			dwFace = 0;

	NSCOLLISIONMAP::ExportCollision ( pFrameMesh, pVertex, pIndex, dwFace, m_mapIndexTex );
	NSCOLLISIONMAP::MakeCollisionMap ( m_pTree, pVertex, pIndex, dwFace );

	SAFE_DELETE_ARRAY ( pVertex );
	SAFE_DELETE_ARRAY ( pIndex );
}

BOOL DxAABBCollision::IsCollisionLine ( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, WORD& wTexIndex )
{
	BOOL bColl = FALSE;
	NSCOLLISIONMAP::CollisionLine ( m_pTree, vP1, vP2, bColl, vColl, vNor, wTexIndex );

	if ( bColl )	return TRUE;
	else			return FALSE;
}

void DxAABBCollision::Save ( CSerialFile& SFile )
{
	SFile << m_vMax;
	SFile << m_vMin;

	int nSize = (int)m_mapIndexTex.size();

	SFile << nSize;

	MAPINDEXTEX_ITER iter = m_mapIndexTex.begin();
	for( ; iter!=m_mapIndexTex.end(); ++iter )
	{
		SFile << (*iter).first;
		SFile << (*iter).second;
	}

	NSCOLLISIONMAP::SaveTree ( SFile, m_pTree );
}

void DxAABBCollision::Load ( CSerialFile& SFile )
{
	CleanUp();

	SFile >> m_vMax;
	SFile >> m_vMin;

	int nSize;
	SFile >> nSize;

	DWORD nCount;
	std::string	szName;
	for ( int i=0; i<nSize; ++i )
	{
		SFile >> nCount;
		SFile >> szName;

		m_mapIndexTex.insert( std::make_pair(nCount,szName.c_str()) );	// 삽입
	}

	NSCOLLISIONMAP::LoadTree ( SFile, m_pTree );
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										N	S		C	O	L	L	I	S	I	O	N	M	A	P	
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSCOLLISIONMAP
{
	BOOL IsFaceFront ( D3DXVECTOR3* pSrc, const D3DXPLANE& sPlane )
	{
		int nCount = 0;
		float fResult;
		for ( DWORD i=0; i<3; ++i )
		{
			fResult = D3DXPlaneDotCoord ( &sPlane, &pSrc[i] );
			if ( fResult > 0.f )		++nCount;
		}
		
		if ( nCount>=2 )	return TRUE;
		else				return FALSE;
	}

	void DivideFace ( D3DXVECTOR3* pSrc, D3DXVECTOR3* pLeft, D3DXVECTOR3* pRight, WORD* pSrcIndex, WORD* pIndexL, WORD* pIndexR, 
						DWORD dwFace, DWORD& dwLeft, DWORD& dwRight, const D3DXPLANE& sPlane )
	{
		dwLeft = 0;
		dwRight = 0;

		for ( DWORD i=0; i<dwFace; ++i )
		{
			// Note : 면이 앞에 있다.		 면 단위 계산
			if ( IsFaceFront ( &pSrc[i*3], sPlane ) )
			{
				pIndexL[dwLeft] = pSrcIndex[i];

				pLeft[(dwLeft*3)+0] = pSrc[(i*3)+0];
				pLeft[(dwLeft*3)+1] = pSrc[(i*3)+1];
				pLeft[(dwLeft*3)+2] = pSrc[(i*3)+2];
				++dwLeft;
			}
			else
			{
				pIndexR[dwRight] = pSrcIndex[i];

				pRight[(dwRight*3)+0] = pSrc[(i*3)+0];
				pRight[(dwRight*3)+1] = pSrc[(i*3)+1];
				pRight[(dwRight*3)+2] = pSrc[(i*3)+2];
				++dwRight;
			}
		}
	}

	DWORD GetDividePOINT ( D3DXVECTOR3* pSrc, D3DXVECTOR3* pLeft, D3DXVECTOR3* pRight, WORD* pSrcIndex, WORD* pIndexL, WORD* pIndexR, 
							DWORD dwFace, DWORD& dwLeft, DWORD& dwRight, const D3DXPLANE& sPlane )
	{
		DivideFace ( pSrc, pLeft, pRight, pSrcIndex, pIndexL, pIndexR, dwFace, dwLeft, dwRight, sPlane );

		if ( dwLeft>=dwRight )	return dwLeft-dwRight;
		else					return dwRight-dwLeft;
	}

	void OptimizeDivide ( DxFaceTree* pTree, D3DXVECTOR3* pLeft, D3DXVECTOR3* pRight, WORD* pIndexL, WORD* pIndexR, DWORD& dwLeft, DWORD& dwRight )
	{
		DWORD	dwPOINT = (DWORD)INT_MAX;

		DWORD	dwLeftFace = 0;
		DWORD	dwRightFace = 0;
		D3DXVECTOR3* pLeftVertex = new D3DXVECTOR3[pTree->dwFace*3];
		D3DXVECTOR3* pRightVertex = new D3DXVECTOR3[pTree->dwFace*3];
		WORD*		pLeftIndex = new WORD[pTree->dwFace];
		WORD*		pRightIndex = new WORD[pTree->dwFace];

		D3DXPLANE sPlane;
		D3DXVECTOR3 vCenter = pTree->vMin;
		vCenter.x += (pTree->fDisX*0.5f);
		vCenter.y += (pTree->fDisY*0.5f);
		vCenter.z += (pTree->fDisZ*0.5f);

		if ( (pTree->fDisY>0.f) )
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.x += 1.f;
			v2.z += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
			DWORD dwDivePoint = GetDividePOINT( pTree->pVertex, 
												pLeftVertex, 
												pRightVertex, 
												pTree->pIndex, 
												pLeftIndex, 
												pRightIndex,
												pTree->dwFace, 
												dwLeftFace, 
												dwRightFace, 
												sPlane );
			if ( dwPOINT > dwDivePoint )
			{
				memcpy ( pLeft, pLeftVertex, sizeof(D3DXVECTOR3)*dwLeftFace*3 );
				memcpy ( pRight, pRightVertex, sizeof(D3DXVECTOR3)*dwRightFace*3 );
				memcpy ( pIndexL, pLeftIndex, sizeof(WORD)*dwLeftFace );
				memcpy ( pIndexR, pRightIndex, sizeof(WORD)*dwRightFace );
				dwLeft = dwLeftFace;
				dwRight = dwRightFace;

				dwPOINT = dwDivePoint;

				if ( dwPOINT <= 1 )	goto _RETURN;
			}
		}
		if ( (pTree->fDisZ>0.f) )
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.x += 1.f;
			v2.y += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
			DWORD dwDivePoint = GetDividePOINT( pTree->pVertex, 
												pLeftVertex, 
												pRightVertex, 
												pTree->pIndex, 
												pLeftIndex, 
												pRightIndex,
												pTree->dwFace, 
												dwLeftFace, 
												dwRightFace, 
												sPlane );
			if ( dwPOINT > dwDivePoint )
			{
				memcpy ( pLeft, pLeftVertex, sizeof(D3DXVECTOR3)*dwLeftFace*3 );
				memcpy ( pRight, pRightVertex, sizeof(D3DXVECTOR3)*dwRightFace*3 );
				memcpy ( pIndexL, pLeftIndex, sizeof(WORD)*dwLeftFace );
				memcpy ( pIndexR, pRightIndex, sizeof(WORD)*dwRightFace );
				dwLeft = dwLeftFace;
				dwRight = dwRightFace;

				dwPOINT = dwDivePoint;

				if ( dwPOINT <= 1 )	goto _RETURN;
			}
		}
		if ( (pTree->fDisX>0.f) )
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.z += 1.f;
			v2.y += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
			DWORD dwDivePoint = GetDividePOINT( pTree->pVertex, 
												pLeftVertex, 
												pRightVertex, 
												pTree->pIndex, 
												pLeftIndex, 
												pRightIndex,
												pTree->dwFace, 
												dwLeftFace, 
												dwRightFace, 
												sPlane );
			if ( dwPOINT > dwDivePoint )
			{
				memcpy ( pLeft, pLeftVertex, sizeof(D3DXVECTOR3)*dwLeftFace*3 );
				memcpy ( pRight, pRightVertex, sizeof(D3DXVECTOR3)*dwRightFace*3 );
				memcpy ( pIndexL, pLeftIndex, sizeof(WORD)*dwLeftFace );
				memcpy ( pIndexR, pRightIndex, sizeof(WORD)*dwRightFace );
				dwLeft = dwLeftFace;
				dwRight = dwRightFace;

				dwPOINT = dwDivePoint;

				if ( dwPOINT <= 1 )	goto _RETURN;
			}
		}

_RETURN:
		SAFE_DELETE_ARRAY ( pLeftVertex );
		SAFE_DELETE_ARRAY ( pRightVertex );
		SAFE_DELETE_ARRAY ( pLeftIndex );
		SAFE_DELETE_ARRAY ( pRightIndex );
	}

	void MakeCollisionMap ( DxFaceTree* pTree )
	{
		// Note : 이건 완료된 데이터
		if ( pTree->dwFace<=1 )	return;


		// Note : 분할을 하자.
		DWORD	dwLeftFace = 0;
		DWORD	dwRightFace = 0;
		D3DXVECTOR3* pLeftVertex = new D3DXVECTOR3[pTree->dwFace*3];
		D3DXVECTOR3* pRightVertex = new D3DXVECTOR3[pTree->dwFace*3];
		WORD*		pLeftIndex = new WORD[pTree->dwFace];
		WORD*		pRightIndex = new WORD[pTree->dwFace];
		OptimizeDivide ( pTree, pLeftVertex, pRightVertex, pLeftIndex, pRightIndex, dwLeftFace, dwRightFace );

		// Note : 더 이상 분할이 안된다. 더 하면 오류만 생긴다.
		if ( !dwLeftFace || !dwRightFace )
		{
			SAFE_DELETE_ARRAY ( pLeftVertex );
			SAFE_DELETE_ARRAY ( pRightVertex );
			SAFE_DELETE_ARRAY ( pLeftIndex );
			SAFE_DELETE_ARRAY ( pRightIndex );
			return;
		}

		// Note : Left 와 Right 의 박스 체크
		D3DXVECTOR3 vMax_L = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMin_L = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( DWORD i=0; i<dwLeftFace*3; ++i )
		{
			if ( vMax_L.x < pLeftVertex[i].x )	vMax_L.x = pLeftVertex[i].x;
			if ( vMax_L.y < pLeftVertex[i].y )	vMax_L.y = pLeftVertex[i].y;
			if ( vMax_L.z < pLeftVertex[i].z )	vMax_L.z = pLeftVertex[i].z;

			if ( vMin_L.x > pLeftVertex[i].x )	vMin_L.x = pLeftVertex[i].x;
			if ( vMin_L.y > pLeftVertex[i].y )	vMin_L.y = pLeftVertex[i].y;
			if ( vMin_L.z > pLeftVertex[i].z )	vMin_L.z = pLeftVertex[i].z;
		}

		D3DXVECTOR3 vMax_R = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMin_R = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( DWORD i=0; i<dwRightFace*3; ++i )
		{
			if ( vMax_R.x < pRightVertex[i].x )	vMax_R.x = pRightVertex[i].x;
			if ( vMax_R.y < pRightVertex[i].y )	vMax_R.y = pRightVertex[i].y;
			if ( vMax_R.z < pRightVertex[i].z )	vMax_R.z = pRightVertex[i].z;

			if ( vMin_R.x > pRightVertex[i].x )	vMin_R.x = pRightVertex[i].x;
			if ( vMin_R.y > pRightVertex[i].y )	vMin_R.y = pRightVertex[i].y;
			if ( vMin_R.z > pRightVertex[i].z )	vMin_R.z = pRightVertex[i].z;
		}

		// Note : 현재 것과 분할 된 것의 AABB 박스가 같다면 분할 하지 말자.
		if ( (vMax_L.x==vMax_R.x) && (vMax_L.y==vMax_R.y) && (vMax_L.z==vMax_R.z) &&
			(vMin_L.x==vMin_R.x) && (vMin_L.y==vMin_R.y) && (vMin_L.z==vMin_R.z) )
		{
			//if ( dwLeftFace )	SAFE_DELETE_ARRAY ( pLeftVertex );
			//if ( dwRightFace )	SAFE_DELETE_ARRAY ( pRightVertex );
			SAFE_DELETE_ARRAY ( pLeftVertex );
			SAFE_DELETE_ARRAY ( pRightVertex );
			SAFE_DELETE_ARRAY ( pLeftIndex );
			SAFE_DELETE_ARRAY ( pRightIndex );
			return;
		}

		// Note : 필요 없어진것 삭제
		SAFE_DELETE_ARRAY ( pTree->pVertex );
		SAFE_DELETE_ARRAY ( pTree->pIndex );
		pTree->dwFace = 0;

		// Note : Left 생성
		pTree->pLeftChild = new DxFaceTree;
		pTree->pLeftChild->vMax = vMax_L;
		pTree->pLeftChild->vMin = vMin_L;
		pTree->pLeftChild->fDisX = vMax_L.x - vMin_L.x;
		pTree->pLeftChild->fDisY = vMax_L.y - vMin_L.y;
		pTree->pLeftChild->fDisZ = vMax_L.z - vMin_L.z;
		pTree->pLeftChild->pVertex = pLeftVertex;
		pTree->pLeftChild->pIndex = pLeftIndex;
		pTree->pLeftChild->dwFace = dwLeftFace;

		MakeCollisionMap ( pTree->pLeftChild );
		
		// Note : Right 생성
		pTree->pRightChild = new DxFaceTree;
		pTree->pRightChild->vMax = vMax_R;
		pTree->pRightChild->vMin = vMin_R;
		pTree->pRightChild->fDisX = vMax_R.x - vMin_R.x;
		pTree->pRightChild->fDisY = vMax_R.y - vMin_R.y;
		pTree->pRightChild->fDisZ = vMax_R.z - vMin_R.z;
		pTree->pRightChild->pVertex = pRightVertex;
		pTree->pRightChild->pIndex = pRightIndex;
		pTree->pRightChild->dwFace = dwRightFace;

		MakeCollisionMap ( pTree->pRightChild );
	}

	void MakeCollisionMap ( PDXFACETREE& pTree, D3DXVECTOR3* pVertex, WORD* pIndex, DWORD dwFace )
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

		pTree = new DxFaceTree;
		pTree->vMax = vMax;
		pTree->vMin = vMin;
		pTree->fDisX = vMax.x - vMin.x;
		pTree->fDisY = vMax.y - vMin.y;
		pTree->fDisZ = vMax.z - vMin.z;
		pTree->pVertex = new D3DXVECTOR3[dwFace*3];
		for ( DWORD i=0; i<dwFace*3; ++i )
		{
			pTree->pVertex[i] = pVertex[i];
		}
		pTree->pIndex = new WORD[dwFace];
		for ( DWORD i=0; i<dwFace; ++i )
		{
			pTree->pIndex[i] = pIndex[i];
		}
		pTree->dwFace = dwFace;

		MakeCollisionMap ( pTree );
	}




	void MakeStaticMesh2 ( VERTEX* pVertices, DWORD* pIndices, DWORD dwFaceCount, D3DXMATRIX& matWorld, LPD3DXVECTOR3& pVertex, LPWORD& pIndex, DWORD& dwFaces, int nIndex )
	{
		if ( !dwFaces )
		{
			dwFaces = dwFaceCount;
			SAFE_DELETE_ARRAY ( pVertex );
			pVertex = new D3DXVECTOR3[dwFaces*3];
			pIndex = new WORD[dwFaces];

			DWORD dwCount=0;

			for ( DWORD i=0; i<dwFaceCount; ++i )
			{
				pIndex[i] = nIndex;		// 텍스쳐 종류 Set

				D3DXVec3TransformCoord ( &pVertex[dwCount], &pVertices[pIndices[(i*3)+0]].vPos, &matWorld );
				++dwCount;

				D3DXVec3TransformCoord ( &pVertex[dwCount], &pVertices[pIndices[(i*3)+1]].vPos, &matWorld );
				++dwCount;

				D3DXVec3TransformCoord ( &pVertex[dwCount], &pVertices[pIndices[(i*3)+2]].vPos, &matWorld );
				++dwCount;
			}
		}
		else
		{
			// Note : 기존 데이터 백업.
			DWORD		dwTempFaces = dwFaces;
			D3DXVECTOR3* pTempVertex = new D3DXVECTOR3[dwFaces*3];
			memcpy ( pTempVertex, pVertex, sizeof(D3DXVECTOR3)*dwFaces*3 );

			WORD* pTempIndex = new WORD[dwFaces];
			memcpy ( pTempIndex, pIndex, sizeof(WORD)*dwFaces );

			// Note : 데이터 백업한 것 가져오기.
			dwFaces += dwFaceCount;
			SAFE_DELETE_ARRAY ( pVertex );
			pVertex = new D3DXVECTOR3[dwFaces*3];
			memcpy ( pVertex, pTempVertex, sizeof(D3DXVECTOR3)*dwTempFaces*3 );
			SAFE_DELETE_ARRAY ( pTempVertex );

			SAFE_DELETE_ARRAY ( pIndex );
			pIndex = new WORD[dwFaces];
			memcpy ( pIndex, pTempIndex, sizeof(WORD)*dwTempFaces );
			SAFE_DELETE_ARRAY ( pTempIndex );

			// Note : 데이터 추가 삽입.
			DWORD dwCountIndex=dwTempFaces;
			DWORD dwCount=dwTempFaces*3;

			for ( DWORD i=0; i<dwFaceCount; ++i )
			{
				pIndex[dwCountIndex] = nIndex;		// 텍스쳐 종류 Set
				++dwCountIndex;

				D3DXVec3TransformCoord ( &pVertex[dwCount], &pVertices[pIndices[(i*3)+0]].vPos, &matWorld );
				++dwCount;

				D3DXVec3TransformCoord ( &pVertex[dwCount], &pVertices[pIndices[(i*3)+1]].vPos, &matWorld );
				++dwCount;

				D3DXVec3TransformCoord ( &pVertex[dwCount], &pVertices[pIndices[(i*3)+2]].vPos, &matWorld );
				++dwCount;
			}
		}
	}

	void MakeStaticMesh1 ( DxMeshes* pMesh, D3DXMATRIX& matWorld, LPD3DXVECTOR3& pVertex, LPWORD& pIndex, DWORD& dwFace, MAPTEXINDEX& mapTexIndex )
	{
		DWORD dwFaces	= pMesh->m_pLocalMesh->GetNumFaces();

		DWORD*	pIndices = new DWORD[dwFaces*3];
		if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
		{
			DWORD*	pIndex32;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
			memcpy ( pIndices, pIndex32, sizeof(DWORD)*dwFaces*3 );
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}
		else
		{
			WORD*	pIndex16;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
			for ( DWORD i=0; i<dwFaces*3; ++i )
			{
				pIndices[i] = (DWORD)pIndex16[i];
			}
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}

		VERTEX*	pVertices;
		pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );

		DWORD dwAttibTableSize;
		pMesh->m_pLocalMesh->GetAttributeTable ( NULL, &dwAttibTableSize );
		LPD3DXATTRIBUTERANGE pAttribTable = NULL;
		pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
		pMesh->m_pLocalMesh->GetAttributeTable ( pAttribTable, &dwAttibTableSize );
		for ( DWORD i=0; i<dwAttibTableSize; ++i )
		{
			char szName[MAX_PATH] = "";
			DWORD dwAttribID = pAttribTable[i].AttribId;
			StringCchCopy( szName, MAX_PATH, pMesh->strTextureFiles[dwAttribID].GetString() );

			MAPTEXINDEX_ITER iter = mapTexIndex.find( szName );
			if ( iter != mapTexIndex.end() )
			{
				MakeStaticMesh2( pVertices,
								pIndices + (pAttribTable[i].FaceStart*3), 
								pAttribTable[i].FaceCount, 
								matWorld, 
								pVertex,
								pIndex,
								dwFace,
								(*iter).second );
			}
			else
			{
				MakeStaticMesh2( pVertices,
								pIndices + (pAttribTable[i].FaceStart*3), 
								pAttribTable[i].FaceCount, 
								matWorld, 
								pVertex,
								pIndex,
								dwFace,
								0 );
			}
		}
		pMesh->m_pLocalMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY ( pAttribTable );
		SAFE_DELETE_ARRAY ( pIndices );
	}

	void MakeStaticMesh ( DxFrame *pframeCur, LPD3DXVECTOR3& pVertex, LPWORD& pIndex, DWORD& dwFace, MAPTEXINDEX& mapTexIndex )
	{
		DxMeshes* pMesh = pframeCur->pmsMeshs;
		while ( pMesh )
		{
			MakeStaticMesh1 ( pMesh, pframeCur->matCombined, pVertex, pIndex, dwFace, mapTexIndex );
			pMesh = pMesh->pMeshNext;
		}

		//	Note : 자식들을 재귀적으로 모두 탐색.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			MakeStaticMesh ( pframeChild, pVertex, pIndex, dwFace, mapTexIndex );
			pframeChild = pframeChild->pframeSibling;
		}
	}

	void CheckIndexTex ( DxFrame *pframeCur, MAPTEXINDEX& mapTexIndex )
	{
		DxMeshes* pMesh = pframeCur->pmsMeshs;
		while ( pMesh )
		{
			VERTEX*	pVertices;
			pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );

			DWORD dwAttibTableSize;
			pMesh->m_pLocalMesh->GetAttributeTable ( NULL, &dwAttibTableSize );

			LPD3DXATTRIBUTERANGE pAttribTable = NULL;
			pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
			pMesh->m_pLocalMesh->GetAttributeTable ( pAttribTable, &dwAttibTableSize );
			for ( DWORD i=0; i<dwAttibTableSize; ++i )
			{
				char szName[MAX_PATH] = "";
				DWORD dwAttribID = pAttribTable[i].AttribId;
				StringCchCopy( szName, MAX_PATH, pMesh->strTextureFiles[dwAttribID].GetString() );

				DWORD dwSize = (DWORD)mapTexIndex.size();
				mapTexIndex.insert ( std::make_pair(szName,dwSize) );	// 삽입
			}

			pMesh->m_pLocalMesh->UnlockVertexBuffer();

			pMesh = pMesh->pMeshNext;
		}

		//	Note : 자식들을 재귀적으로 모두 탐색.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			CheckIndexTex ( pframeChild, mapTexIndex );
			pframeChild = pframeChild->pframeSibling;
		}
	}

	void ExportCollision ( DxFrameMesh* pFrameMesh, LPD3DXVECTOR3& pVertex, LPWORD& pIndex, DWORD& dwFace, MAPINDEXTEX& mapIndexTex )
	{
		SAFE_DELETE_ARRAY ( pVertex );
		dwFace = 0;

		DxFrame* pFrameRoot = pFrameMesh->GetFrameRoot();
		if ( pFrameRoot )
		{
			// Note : 텍스쳐 정보 알아오기	
			MAPTEXINDEX mapTexIndex;
			CheckIndexTex ( pFrameRoot, mapTexIndex );

			// Note : 정점 생성
			MakeStaticMesh ( pFrameRoot, pVertex, pIndex, dwFace, mapTexIndex );

			// Note : Convert ( Tex,Index -> Index,Tex )
			mapIndexTex.clear();
			MAPTEXINDEX_ITER iter = mapTexIndex.begin();
			for ( ; iter!=mapTexIndex.end(); ++iter )
			{
				mapIndexTex.insert( std::make_pair( (*iter).second, (*iter).first.c_str() ) );	// 삽입
			}

			// Note : 임시 데이터 삭제
			mapTexIndex.clear();
		}
	}

	BOOL IntersectTriangle ( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, 
							D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v )
	{
		// Find vectors for two edges sharing vert0
		D3DXVECTOR3 edge1 = v1 - v0;
		D3DXVECTOR3 edge2 = v2 - v0;

		// Begin calculating determinant - also used to calculate U parameter
		D3DXVECTOR3 pvec;
		D3DXVec3Cross( &pvec, &dir, &edge2 );

		// If determinant is near zero, ray lies in plane of triangle
		FLOAT det = D3DXVec3Dot( &edge1, &pvec );

		D3DXVECTOR3 tvec;
		if( det > 0 )
		{
			tvec = orig - v0;
		}
		else
		{
			tvec = v0 - orig;
			det = -det;
		}

		if( det < 0.000001f )
			return FALSE;

		// Calculate U parameter and test bounds
		*u = D3DXVec3Dot( &tvec, &pvec );
		if( *u < 0.0f || *u > det )
			return FALSE;

		// Prepare to test V parameter
		D3DXVECTOR3 qvec;
		D3DXVec3Cross( &qvec, &tvec, &edge1 );

		// Calculate V parameter and test bounds
		*v = D3DXVec3Dot( &dir, &qvec );
		if( *v < 0.0f || *u + *v > det )
			return FALSE;

		// Calculate t, scale parameters, ray intersects triangle
		*t = D3DXVec3Dot( &edge2, &qvec );
		FLOAT fInvDet = 1.0f / det;
		*t *= fInvDet;
		*u *= fInvDet;
		*v *= fInvDet;

		return TRUE;
	}

	void CollisionLine ( DxFaceTree* pTree, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, BOOL& bColl, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, WORD& wTexIndex )
	{
		if ( !pTree )	return;

		// Note : AABB 박스와 선의 충돌처리 하자.
		if ( !COLLISION::IsCollisionLineToAABB ( vStart, vEnd, pTree->vMax, pTree->vMin ) )	return;

		if ( pTree->pVertex )
		{
			float u,v,t;
			D3DXVECTOR3 vEdge1, vEdge2;
			D3DXVECTOR3 vDir = vEnd - vStart;
			D3DXVec3Normalize ( &vDir, &vDir );
			for ( DWORD i=0; i<pTree->dwFace; ++i )
			{
				// Note : 제일 가까운 점을 찾아야 한다. 수정 요망
				if ( IntersectTriangle ( vStart, vDir, pTree->pVertex[i*3+0], pTree->pVertex[i*3+1], pTree->pVertex[i*3+2], &t, &u, &v ) )
				{
					bColl = TRUE;

					vEdge1 = pTree->pVertex[i*3+1] -  pTree->pVertex[i*3+0];
					vEdge2 = pTree->pVertex[i*3+2] -  pTree->pVertex[i*3+0];
					vColl = pTree->pVertex[i*3+0] + (vEdge1*u) + (vEdge2*v);

					wTexIndex = pTree->pIndex[i];

					return;
				}
			}
			return;
		}

		BOOL			bColl_1 = FALSE;
		D3DXVECTOR3		vColl_1	= D3DXVECTOR3 ( 0.f,0.f,0.f );
		D3DXVECTOR3		vNor_1	= D3DXVECTOR3 ( 0.f,1.f,0.f );
		BOOL			bColl_2 = FALSE;
		D3DXVECTOR3		vColl_2	= D3DXVECTOR3 ( 0.f,0.f,0.f );
		D3DXVECTOR3		vNor_2	= D3DXVECTOR3 ( 0.f,1.f,0.f );
		
		if ( pTree->pLeftChild )	CollisionLine ( pTree->pLeftChild, vStart, vEnd, bColl_1, vColl_1, vNor_1, wTexIndex );
		if ( pTree->pRightChild )	CollisionLine ( pTree->pRightChild, vStart, vEnd, bColl_2, vColl_2, vNor_2, wTexIndex );

		// 데이터를 정리하여서 넘긴다.
		if ( bColl_1 && bColl_2 )
		{
			D3DXVECTOR3 vLength_1 = vColl_1-vStart;
			D3DXVECTOR3 vLength_2 = vColl_2-vStart;
			float		fLength_1 = D3DXVec3Length ( &vLength_1 );
			float		fLength_2 = D3DXVec3Length ( &vLength_2 );

			if ( fLength_1 <= fLength_2 )
			{
				bColl = TRUE;
				vColl = vColl_1;
				vNor = vNor_1;
			}
			else
			{
				bColl = TRUE;
				vColl = vColl_2;
				vNor = vNor_2;
			}
		}
		else if ( bColl_1 )
		{
			bColl = bColl_1;
			vColl = vColl_1;
			vNor = vNor_1;
		}
		else if ( bColl_2 )
		{
			bColl = bColl_2;
			vColl = vColl_2;
			vNor = vNor_2;
		}
		else
		{
			bColl = FALSE;
			vColl = D3DXVECTOR3 ( 0.f,0.f,0.f );
			vNor = D3DXVECTOR3 ( 0.f,1.f,0.f );
		}
	}


	void SaveTree ( CSerialFile& SFile, DxFaceTree* pTree )
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

		SaveTree ( SFile, pTree->pLeftChild );
		SaveTree ( SFile, pTree->pRightChild );
	}

	void LoadTree ( CSerialFile& SFile, PDXFACETREE& pTree )
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			pTree = new DxFaceTree;
			pTree->Load ( SFile );
		}
		else
		{
			return;
		}

		LoadTree ( SFile, pTree->pLeftChild );
		LoadTree ( SFile, pTree->pRightChild );
	}
};