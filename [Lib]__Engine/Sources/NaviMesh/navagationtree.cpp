#include "pch.h"
#include "navigationmesh.h"
#include "./DxLandMan.h"

#include "./collision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT NavigationMesh::GetSizeNode ( D3DXMATRIX &matComb, DWORD	*pCellIndex, DWORD nCellIndex,
								D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	if ( !pCellIndex || nCellIndex < 1 )
	{
		return E_FAIL;
	}

	vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	D3DXVECTOR3 vDist;    

	for ( DWORD i = 0; i < nCellIndex; i++ )
	{
		NavigationCell* Cell = m_pDACell[pCellIndex[i]];

		D3DXVec3TransformCoord ( &vDist, &m_pNaviVertex[Cell->Vertex(0)], &matComb );

		if ( vDist.x > vMax.x )	vMax.x = vDist.x;
		if ( vDist.y > vMax.y )	vMax.y = vDist.y;
		if ( vDist.z > vMax.z )	vMax.z = vDist.z;

		if ( vDist.x < vMin.x )	vMin.x = vDist.x;
		if ( vDist.y < vMin.y )	vMin.y = vDist.y;
		if ( vDist.z < vMin.z )	vMin.z = vDist.z;

		D3DXVec3TransformCoord ( &vDist, &m_pNaviVertex[Cell->Vertex(1)], &matComb );

		if ( vDist.x > vMax.x )	vMax.x = vDist.x;
		if ( vDist.y > vMax.y )	vMax.y = vDist.y;
		if ( vDist.z > vMax.z )	vMax.z = vDist.z;

		if ( vDist.x < vMin.x )	vMin.x = vDist.x;
		if ( vDist.y < vMin.y )	vMin.y = vDist.y;
		if ( vDist.z < vMin.z )	vMin.z = vDist.z;

		D3DXVec3TransformCoord ( &vDist, &m_pNaviVertex[Cell->Vertex(2)], &matComb );

		if ( vDist.x > vMax.x )	vMax.x = vDist.x;
		if ( vDist.y > vMax.y )	vMax.y = vDist.y;
		if ( vDist.z > vMax.z )	vMax.z = vDist.z;

		if ( vDist.x < vMin.x )	vMin.x = vDist.x;
		if ( vDist.y < vMin.y )	vMin.y = vDist.y;
		if ( vDist.z < vMin.z )	vMin.z = vDist.z;
	}

	//	Note : 충돌처리시 유격 필요.
	//
	vMax.x += 0.0001f;
	vMin.x -= 0.0001f;
	vMax.y += 0.0001f;
	vMin.y -= 0.0001f;
	vMax.z += 0.0001f;
	vMin.z -= 0.0001f;

	return S_OK;
}

HRESULT NavigationMesh::GetCenterDistNode ( D3DXMATRIX &matComb, DWORD	*pCellIndex, DWORD nCellIndex,
								D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	if ( !pCellIndex || nCellIndex < 1 )
	{
		return E_FAIL;
	}

	vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	D3DXVECTOR3 vDist;

	for ( DWORD i = 0; i < nCellIndex; i++ )
	{
		NavigationCell* Cell = m_pDACell[pCellIndex[i]];

		D3DXVECTOR3 vCenter;

		D3DXVec3TransformCoord ( &vDist, &m_pNaviVertex[Cell->Vertex(0)], &matComb );
		vCenter = vDist;
		D3DXVec3TransformCoord ( &vDist, &m_pNaviVertex[Cell->Vertex(1)], &matComb );
		vCenter += vDist;
		D3DXVec3TransformCoord ( &vDist, &m_pNaviVertex[Cell->Vertex(2)], &matComb );
		vCenter += vDist;

		vCenter /= 3;

		if ( vDist.x > vMax.x )	vMax.x = vCenter.x;
		if ( vDist.y > vMax.y )	vMax.y = vCenter.y;
		if ( vDist.z > vMax.z )	vMax.z = vCenter.z;

		if ( vDist.x < vMin.x )	vMin.x = vCenter.x;
		if ( vDist.y < vMin.y )	vMin.y = vCenter.y;
		if ( vDist.z < vMin.z )	vMin.z = vCenter.z;
	}

	//	Note : 충돌 처리시 유격 필요.
	//
	vMax.x += 0.0001f;
	vMin.x -= 0.0001f;
	vMax.y += 0.0001f;
	vMin.y -= 0.0001f;
	vMax.z += 0.0001f;
	vMin.z -= 0.0001f;

	return S_OK;
}

BOOL NavigationMesh::IsWithInTriangle ( D3DXMATRIX &matComb,
						LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3,
						float fDivision, DWORD dwAxis )
{
	D3DXVECTOR3 vDist;
	D3DXVECTOR3 vCenter;

	D3DXVec3TransformCoord ( &vDist, pvT1, &matComb );
	vCenter = vDist;
	D3DXVec3TransformCoord ( &vDist, pvT2, &matComb );
	vCenter += vDist;
	D3DXVec3TransformCoord ( &vDist, pvT3, &matComb );
	vCenter += vDist;

	vCenter /= 3;

	switch ( dwAxis )
	{
	case 0:
		if (vCenter.x <= fDivision )	return TRUE;
		break;
	
	case 1:
		if (vCenter.y <= fDivision )	return TRUE;
		break;
	
	case 2:
		if (vCenter.z <= fDivision )	return TRUE;
		break;
	};

	return FALSE;
}

static void CalcAxisTable ( float SizeX, float SizeY, float SizeZ, PDWORD pdwAxis )
{
	if ( SizeX >= SizeY && SizeX >= SizeZ )
	{
		pdwAxis[0] = 0;

		if ( SizeY >= SizeZ )
		{
			pdwAxis[1] = 1;
			pdwAxis[2] = 2;
		}
		else
		{
			pdwAxis[1] = 2;
			pdwAxis[2] = 1;
		}
	}
	else if ( SizeY >= SizeX && SizeY >= SizeZ )
	{
		pdwAxis[0] = 1;

		if ( SizeX >= SizeZ )
		{
			pdwAxis[1] = 0;
			pdwAxis[2] = 2;
		}
		else
		{
			pdwAxis[1] = 2;
			pdwAxis[2] = 0;
		}
	}
	else
	{
		pdwAxis[0] = 2;

		if ( SizeX >= SizeY )
		{
			pdwAxis[1] = 0;
			pdwAxis[2] = 1;
		}
		else
		{
			pdwAxis[1] = 1;
			pdwAxis[2] = 0;
		}
	}
}

HRESULT NavigationMesh::MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb,
									DWORD	*pCellIndex, DWORD nCellIndex,
									D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr = S_OK;

	//	Note : 페이스가 하나이라면.
	//	
	if ( nCellIndex == 1 )
	{
		pNode->fMaxX = vMax.x;
		pNode->fMaxY = vMax.y;
		pNode->fMaxZ = vMax.z;
		pNode->fMinX = vMin.x;
		pNode->fMinY = vMin.y;
		pNode->fMinZ = vMin.z;

		NavigationCell *Cell = m_pDACell[pCellIndex[0]];
		pNode->dwFace = Cell->CellID();
        
		ExportProgress::CurPos++;
		return S_OK;
	}

	//	Note : 두개의 부분 집합으로 나눌 공간 분할.
	//
	DWORD dwAxis[3];
	float fDivision;
	float SizeX = vMax.x - vMin.x;
	float SizeY = vMax.y - vMin.y;
	float SizeZ = vMax.z - vMin.z;

	CalcAxisTable ( SizeX, SizeY, SizeZ, dwAxis );

	//	Note : 분할된 공간에 값을 넣는다.
	//	
	DWORD nNumLeftCell = 0, nNumRightCell = 0;

	D3DXVECTOR3 vB1Max, vB1Min;
	D3DXVECTOR3 vB2Max, vB2Min;

	DWORD	i = 0;
	DWORD	j = 0;
	DWORD	*pLeftCellIndex = new DWORD [ nCellIndex ];
	DWORD	*pRightCellIndex = new DWORD [ nCellIndex ];

	for ( i = 0; i < 3; i++ )
	{
		nNumLeftCell = 0;
		nNumRightCell = 0;

		switch ( dwAxis[i] )
		{
		case 0:
			fDivision = vMin.x+SizeX/2;
			break;

		case 1:
			fDivision = vMin.y+SizeY/2;
			break;

		case 2:
			fDivision = vMin.z+SizeZ/2;
			break;
		};
		
		for ( j = 0; j < nCellIndex; j++ )
		{
			NavigationCell *pCell = m_pDACell [ pCellIndex[j] ];
			
			LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(0)];
			LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(1)];
			LPD3DXVECTOR3 pvT3 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(2)];

			if ( IsWithInTriangle(matComb,pvT1,pvT2,pvT3,fDivision,dwAxis[i]) )
			{
				pLeftCellIndex[nNumLeftCell] = pCellIndex[j];
				nNumLeftCell++;
			}
			else
			{
				pRightCellIndex[nNumRightCell] = pCellIndex[j];
				nNumRightCell++;
			}
		}
		
		if ( nNumLeftCell != 0 && nNumRightCell != 0 )
		{
			break;
		}
	}

	//	Note : 분할된 AABB중 한곳이 비여버리면 삼각형의 중심차로 분할.
	//		삼각형의 중심이 한쪽 사면에 편중되었을 경우에...
	//
	if ( nNumLeftCell == 0 || nNumRightCell == 0 )
	{
		D3DXVECTOR3 vCMax, vCMin;

		GetCenterDistNode ( matComb, pCellIndex, nCellIndex, vCMax, vCMin );

		SizeX = vCMax.x - vCMin.x;
		SizeY = vCMax.y - vCMin.y;
		SizeZ = vCMax.z - vCMin.z;

		CalcAxisTable ( SizeX, SizeY, SizeZ, dwAxis );

		for ( i=0; i<3; i++ )
		{
			nNumLeftCell = 0;
			nNumRightCell = 0;

			switch ( dwAxis[i] )
			{
			case 0:
				fDivision = vCMin.x+SizeX/2;
				break;

			case 1:
				fDivision = vCMin.y+SizeY/2;
				break;

			case 2:
				fDivision = vCMin.z+SizeZ/2;
				break;
			};

			for ( j = 0; j < nCellIndex; j++ )
			{
                NavigationCell *pCell = m_pDACell [ pCellIndex[j] ];

				LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(0)];
				LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(1)];
				LPD3DXVECTOR3 pvT3 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(2)];

				if ( IsWithInTriangle(matComb,pvT1,pvT2,pvT3,fDivision,dwAxis[i]) )
				{
					pLeftCellIndex[nNumLeftCell] = pCellIndex[j];
					nNumLeftCell++;
				}
				else
				{
					pRightCellIndex[nNumRightCell] = pCellIndex[j];
					nNumRightCell++;
				}
			}
			
			if ( nNumLeftCell != 0 && nNumRightCell != 0 )
				break;
		}
	}

	//	Note : 분할된 AABB중 한곳이 비여 버리면 임의로 분할.
	//		여기에 걸릴 가능성은 거의 없다. 최후의 보루.
	//
	if ( nNumLeftCell == 0 || nNumRightCell == 0 )
	{
		nNumLeftCell = 0;
		nNumRightCell = 0;

		for ( j = 0; j < (nCellIndex/2); j++ )
		{
			pLeftCellIndex[nNumLeftCell] = pCellIndex[j];
			nNumLeftCell++;
		}
		
		for ( j = WORD ( nCellIndex / 2 ); j < nCellIndex; j++ )
		{
			pRightCellIndex[nNumRightCell] = pCellIndex[j];
			nNumRightCell++;
		}
	}

	//	Note : 분할 AABB 자식으로.
	//
	if ( nNumLeftCell )
	{
		hr = GetSizeNode ( matComb, pLeftCellIndex, nNumLeftCell, vB1Max, vB1Min );
		if ( FAILED ( hr ) )
			goto e_failed;

		DxAABBNode *pNodeChild = new DxAABBNode;
		pNodeChild->fMaxX = vB1Max.x;
		pNodeChild->fMaxY = vB1Max.y;
		pNodeChild->fMaxZ = vB1Max.z;
		pNodeChild->fMinX = vB1Min.x;
		pNodeChild->fMinY = vB1Min.y;
		pNodeChild->fMinZ = vB1Min.z;

		pNode->pLeftChild = pNodeChild;

		hr = MakeAABBNode ( pNodeChild, matComb, pLeftCellIndex, nNumLeftCell, vB1Max, vB1Min );
		if ( FAILED ( hr ) )
			goto e_failed;
	}

	if ( nNumRightCell )
	{
		hr = GetSizeNode ( matComb, pRightCellIndex, nNumRightCell, vB2Max, vB2Min );
		if ( FAILED ( hr ) )
			goto e_failed;

		DxAABBNode *pNodeChild = new DxAABBNode;
		pNodeChild->fMaxX = vB2Max.x;
		pNodeChild->fMaxY = vB2Max.y;
		pNodeChild->fMaxZ = vB2Max.z;
		pNodeChild->fMinX = vB2Min.x;
		pNodeChild->fMinY = vB2Min.y;
		pNodeChild->fMinZ = vB2Min.z;

		pNode->pRightChild = pNodeChild;

		hr = MakeAABBNode ( pNodeChild, matComb, pRightCellIndex, nNumRightCell, vB2Max, vB2Min );
		if ( FAILED ( hr ) )
			goto e_failed;
	}

e_failed:
	SAFE_DELETE_ARRAY(pLeftCellIndex);
	SAFE_DELETE_ARRAY(pRightCellIndex);

	return hr;
}

static BOOL s_bCollision;
void NavigationMesh::IsCollision ( DxAABBNode *pAABBCur, D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, DWORD &CollisionID )
{
	D3DXVECTOR3 vNewP1=vP1, vNewP2=vP2;

	if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
	{
		if ( pAABBCur->dwFace != AABB_NONINDEX)
		{	
			NavigationCell	*pCell = m_pDACell[ pAABBCur->dwFace ];

			LPD3DXVECTOR3 pvT0 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(0)];
			LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(1)];
			LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) &m_pNaviVertex[pCell->Vertex(2)];

			D3DXVECTOR3 vNewCollision;
			if ( COLLISION::IsLineTriangleCollision ( pvT0, pvT1, pvT2, &vNewP1, &vNewP2, &vNewCollision ) )
			{
				D3DXVECTOR3 vDxVect;
				vDxVect = COLLISION::vColTestStart - vNewCollision;
				float fNewLength = D3DXVec3LengthSq ( &vDxVect );

				vDxVect = COLLISION::vColTestStart - vCollision;
				float fOldLength = D3DXVec3LengthSq ( &vDxVect );

				if ( fNewLength < fOldLength )
				{
					vCollision = vNewCollision;
					CollisionID = pAABBCur->dwFace;
					s_bCollision = TRUE;
				}
			}
		}
		else
		{
			if ( pAABBCur->pLeftChild )
				IsCollision ( pAABBCur->pLeftChild, vNewP1, vNewP2, vCollision, CollisionID );

			if ( pAABBCur->pRightChild )
				IsCollision ( pAABBCur->pRightChild, vNewP1, vNewP2, vCollision, CollisionID );
		}
	}
}

void NavigationMesh::IsCollision ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2,
								D3DXVECTOR3 &vCollision, DWORD &CollisionID, BOOL &bCollision )
{
	bCollision = FALSE;
	if ( !m_pAABBTreeRoot )	return;

	s_bCollision = FALSE;
	CollisionID = -1;

	vCollision = vPoint2;
	COLLISION::vColTestStart	= vPoint1;
	COLLISION::vColTestEnd		= vPoint2;
	IsCollision ( m_pAABBTreeRoot, vPoint1, vPoint2, vCollision, CollisionID );

	bCollision = s_bCollision;
}


HRESULT	NavigationMesh::MakeAABBTree ( )
{
	HRESULT hr = S_OK;
	
	D3DXVECTOR3 vMax, vMin;

	DWORD	*pCellIndex = new DWORD [ TotalCells() ];
	
	for ( int i = 0; i < TotalCells(); i++ )
	{
		pCellIndex[i] = m_pDACell[i]->CellID();
	}

	D3DXMATRIX	matComb;
	D3DXMatrixIdentity ( &matComb );
	hr = GetSizeNode ( matComb, pCellIndex, TotalCells(), vMax, vMin );

	DxAABBNode *pNode = new DxAABBNode;
	pNode->fMaxX = vMax.x;
	pNode->fMaxY = vMax.y;
	pNode->fMaxZ = vMax.z;
	pNode->fMinX = vMin.x;
	pNode->fMinY = vMin.y;
	pNode->fMinZ = vMin.z;

	hr = MakeAABBNode ( pNode, matComb, pCellIndex, TotalCells(), vMax, vMin );

	m_pAABBTreeRoot = pNode;

	SAFE_DELETE_ARRAY ( pCellIndex );

	return hr;
}

void	NavigationMesh::GetAllCollisionCell ( DWORD *pCollisionCellID, DWORD& CollisionIDCount, D3DXVECTOR3& vAMax, D3DXVECTOR3& vAMin, DxAABBNode *pAABBCur )
{
	D3DXVECTOR3 vBMax, vBMin;

	vBMax.x = pAABBCur->fMaxX;
	vBMax.y = pAABBCur->fMaxY;
	vBMax.z = pAABBCur->fMaxZ;

	vBMin.x = pAABBCur->fMinX;
	vBMin.y = pAABBCur->fMinY;
	vBMin.z = pAABBCur->fMinZ;

	if ( COLLISION::IsCollisionAABBToAABB ( vAMax, vAMin, vBMax, vBMin ) )
	{
		if ( pAABBCur->dwFace != AABB_NONINDEX)
		{
			pCollisionCellID[CollisionIDCount] = pAABBCur->dwFace;
			CollisionIDCount++;
		}
		else
		{
			if ( pAABBCur->pLeftChild )
				GetAllCollisionCell ( pCollisionCellID, CollisionIDCount, vAMax, vAMin, pAABBCur->pLeftChild );

			if ( pAABBCur->pRightChild )
				GetAllCollisionCell ( pCollisionCellID, CollisionIDCount, vAMax, vAMin, pAABBCur->pRightChild );				
		}
	}
}