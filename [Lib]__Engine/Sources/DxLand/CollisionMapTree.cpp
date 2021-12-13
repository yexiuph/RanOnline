#include "pch.h"
#include "CollisionMap.h"
#include "CollisionMapCell.h"
#include "collision.h"

#include "DxLandMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CCollisionMap::GetSizeNode ( D3DXMATRIX &matComb, DWORD	*pCellIndex, DWORD nCellIndex,
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
		CCollisionMapCell* Cell = &m_pMapCells[pCellIndex[i]];

		D3DXVec3TransformCoord ( &vDist,
			&m_pVertices[Cell->GetIndex(CCollisionMapCell::VERT_A)], &matComb );

		if ( vDist.x > vMax.x )	vMax.x = vDist.x;
		if ( vDist.y > vMax.y )	vMax.y = vDist.y;
		if ( vDist.z > vMax.z )	vMax.z = vDist.z;

		if ( vDist.x < vMin.x )	vMin.x = vDist.x;
		if ( vDist.y < vMin.y )	vMin.y = vDist.y;
		if ( vDist.z < vMin.z )	vMin.z = vDist.z;

		D3DXVec3TransformCoord ( &vDist,
			&m_pVertices[Cell->GetIndex(CCollisionMapCell::VERT_B)], &matComb );

		if ( vDist.x > vMax.x )	vMax.x = vDist.x;
		if ( vDist.y > vMax.y )	vMax.y = vDist.y;
		if ( vDist.z > vMax.z )	vMax.z = vDist.z;

		if ( vDist.x < vMin.x )	vMin.x = vDist.x;
		if ( vDist.y < vMin.y )	vMin.y = vDist.y;
		if ( vDist.z < vMin.z )	vMin.z = vDist.z;

		D3DXVec3TransformCoord ( &vDist,
			&m_pVertices[Cell->GetIndex(CCollisionMapCell::VERT_C)], &matComb );

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

HRESULT CCollisionMap::GetCenterDistNode ( D3DXMATRIX &matComb, DWORD	*pCellIndex, DWORD nCellIndex,
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
		CCollisionMapCell* Cell = &m_pMapCells[pCellIndex[i]];

		D3DXVECTOR3 vCenter;

		D3DXVec3TransformCoord ( &vDist,
			&m_pVertices[Cell->GetIndex(CCollisionMapCell::VERT_A)], &matComb );
		vCenter = vDist;

		D3DXVec3TransformCoord ( &vDist,
			&m_pVertices[Cell->GetIndex(CCollisionMapCell::VERT_B)], &matComb );
		vCenter += vDist;

		D3DXVec3TransformCoord ( &vDist,
			&m_pVertices[Cell->GetIndex(CCollisionMapCell::VERT_C)], &matComb );		
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

BOOL CCollisionMap::IsWithInTriangle ( D3DXMATRIX &matComb,
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

HRESULT CCollisionMap::MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb,
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

		CCollisionMapCell *Cell = &m_pMapCells[pCellIndex[0]];
		pNode->dwFace = Cell->GetCellID();

		//	진행상황
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
			CCollisionMapCell *pCell = &m_pMapCells [ pCellIndex[j] ];
			
			LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_A)];
			LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_B)];
			LPD3DXVECTOR3 pvT3 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_C)];

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
                CCollisionMapCell *pCell = &m_pMapCells [ pCellIndex[j] ];

				LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_A)];
				LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_B)];
				LPD3DXVECTOR3 pvT3 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_C)];

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

void CCollisionMap::IsCollision ( DxAABBNode *pAABBCur, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, BOOL bEditor, const BOOL bFrontColl )
{
	D3DXVECTOR3 vNewP1=vP1, vNewP2=vP2;

	if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
	{
		if ( pAABBCur->dwFace != AABB_NONINDEX)
		{	
			CCollisionMapCell	*pCell = &m_pMapCells[ pAABBCur->dwFace ];

			LPD3DXVECTOR3 pvT0 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_A)];
			LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_B)];
			LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) &m_pVertices[pCell->GetIndex(CCollisionMapCell::VERT_C)];

			//	Note : 삼각형과 선분의 충돌 검사. 검사시 넘기는 선분은 aabb 영역으로 축소된 것을 넘기지 않고
			//		원 선분을 넘김. ( aabb 영역으로 축소되면서 충돌 체크에 문제가 발생. )
			//
			D3DXVECTOR3 vNewCollision;
			if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &COLLISION::vColTestStart, &COLLISION::vColTestEnd, 
													&vNewCollision, NULL, bFrontColl ) )
			{
				//	충돌 정보 수집
				SCollisionCell CollisionCell;
				CollisionCell.dwID = pAABBCur->dwFace;

				if ( bEditor ) CollisionCell.pFrame = m_ppDxFrame[pAABBCur->dwFace];

				CollisionCell.vCollision = vNewCollision;
				m_CollisionContainer.AddCell ( CollisionCell );
			}
		}
		else
		{
			if ( pAABBCur->pLeftChild )
				IsCollision ( pAABBCur->pLeftChild, vNewP1, vNewP2, bEditor, bFrontColl );

			if ( pAABBCur->pRightChild )
				IsCollision ( pAABBCur->pRightChild, vNewP1, vNewP2, bEditor, bFrontColl );
		}
	}
}

BOOL CCollisionMap::IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, BOOL bEditor, const DxFrame* pFrame, char* pName, const BOOL bFrontColl )
{
	if ( !m_pAABBTreeRoot )	
	{
		if ( m_pFrameMesh )
		{
			return m_pFrameMesh->IsCollision( vP1, vP2, pName, bFrontColl );
		}
	}
	else
	{
		m_CollisionContainer.CleanUp ();
		m_CollisionContainer.SetCollisionVertex ( vP1, vP2 );

		COLLISION::vColTestStart	= vP1;
		COLLISION::vColTestEnd		= vP2;

		IsCollision( m_pAABBTreeRoot, vP1, vP2, bEditor, bFrontColl );
		
		m_pCollisionCell = m_CollisionContainer.GetMinDistFromV0 ( pFrame );

		////////////////////////////////////////////////////////
		//	옮겨야 하는 코드
		if ( bEditor )
		{
			if ( m_pCollisionCell )
			{
				m_pFrameMesh->SetCollisionDetectedFrame ( m_pCollisionCell->pFrame );
			}
			else
			{
				m_pFrameMesh->SetCollisionDetectedFrame ( NULL );
			}
		}
		////////////////////////////////////////////////////////

		return (m_pCollisionCell) ? TRUE : FALSE;
	}

	return FALSE;
}

DxFrame* const CCollisionMap::GetCollisionDetectedFrame ()
{
	if ( !m_pAABBTreeRoot )	
	{
		if ( m_pFrameMesh )
			return m_pFrameMesh->GetCollisionDetectedFrame();
	}
	else
	{
		return (m_pCollisionCell)?m_pCollisionCell->pFrame:NULL;
	}

	return NULL;
}

void CCollisionMap::SetCollisionDetectedFrame ( DxFrame* pDxFrame )
{
	if ( !m_pAABBTreeRoot )	
	{
		if ( m_pFrameMesh )
			return m_pFrameMesh->SetCollisionDetectedFrame( pDxFrame );
	}
	else
	{
		if ( m_pFrameMesh )
			return m_pFrameMesh->SetCollisionDetectedFrame( pDxFrame );
	}
}

const D3DXVECTOR3 CCollisionMap::GetCollisionPos ()
{
	if ( !m_pAABBTreeRoot )	
	{
		if ( m_pFrameMesh )
			return m_pFrameMesh->GetCollisionPos();
		else
			return D3DXVECTOR3(0,0,0);
	}
	else
	{	
		if ( m_pCollisionCell )
			return m_pCollisionCell->vCollision;
		else
			return m_CollisionContainer.GetVertexV1 ();		
	}
}

HRESULT	CCollisionMap::MakeAABBTree ( )
{
	HRESULT hr = S_OK;
	
	D3DXVECTOR3 vMax, vMin;

	DWORD	*pCellIndex = new DWORD [ m_nMapCells ];
	
	for ( DWORD i = 0; i < m_nMapCells; i++ )
	{
		pCellIndex[i] = m_pMapCells[i].GetCellID ();
	}

	D3DXMATRIX	matComb;
	D3DXMatrixIdentity ( &matComb );
	hr = GetSizeNode ( matComb, pCellIndex, m_nMapCells, vMax, vMin );

	DxAABBNode *pNode = new DxAABBNode;
	pNode->fMaxX = vMax.x;
	pNode->fMaxY = vMax.y;
	pNode->fMaxZ = vMax.z;
	pNode->fMinX = vMin.x;
	pNode->fMinY = vMin.y;
	pNode->fMinZ = vMin.z;

	hr = MakeAABBNode ( pNode, matComb, pCellIndex, m_nMapCells, vMax, vMin );

	m_pAABBTreeRoot = pNode;

	SAFE_DELETE_ARRAY ( pCellIndex );

	return hr;
}