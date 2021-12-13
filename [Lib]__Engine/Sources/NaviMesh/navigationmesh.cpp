#include "pch.h"

#include "navigationmesh.h"
#include "navigationpath.h"
#include "./collision.h"
#include "./DxViewPort.h"
#include "./DxLandMan.h"
#include "./SerialFile.h"
#include "DxLightMan.h"
#include <GASSERT.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL NavigationMesh::GetAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	if ( m_pAABBTreeRoot )
	{
		vMax = m_pAABBTreeRoot->vMax;
		vMin = m_pAABBTreeRoot->vMin;
		
		return TRUE;
	}

	return FALSE;
}

D3DXVECTOR3 NavigationMesh::SnapPointToCell(DWORD CellID, const D3DXVECTOR3& Point)
{
	D3DXVECTOR3 PointOut = Point;
	NavigationCell *pCell = GetCell ( CellID );
	if ( !pCell )	return D3DXVECTOR3(0,0,0);

	if (!pCell->IsPointInCellCollumn(PointOut))
	{
		pCell->ForcePointToCellCollumn(PointOut);
	}

	pCell->MapVectorHeightToCell(PointOut);
	return (PointOut);
}

D3DXVECTOR3 NavigationMesh::SnapPointToMesh(DWORD *CellOutID, const D3DXVECTOR3& Point)
{ 
	D3DXVECTOR3 PointOut = Point;
	*CellOutID = FindClosestCell(PointOut);	 

	return (SnapPointToCell(*CellOutID, PointOut));
}

DWORD		NavigationMesh::FindClosestCell(const D3DXVECTOR3& Point)const
{
	float ClosestDistance = 3.4E+38f;
	float ClosestHeight = 3.4E+38f;
	bool FoundHomeCell = false;
	float ThisDistance;
	NavigationCell* ClosestCell=0;

	CELL_ARRAY::const_iterator	CellIter = m_CellArray.begin();
	for(;CellIter != m_CellArray.end(); ++CellIter)
	{
		NavigationCell* pCell = *CellIter;

		if (pCell->IsPointInCellCollumn(Point))
		{
			D3DXVECTOR3 NewPosition(Point);
			pCell->MapVectorHeightToCell(NewPosition);

			ThisDistance = (float)fabs(NewPosition.y - Point.y);

			if (FoundHomeCell)
			{
				if (ThisDistance < ClosestHeight)
				{
					ClosestCell = pCell;
					ClosestHeight = ThisDistance;
				}
			}
			else
			{
				ClosestCell = pCell;
				ClosestHeight = ThisDistance;
				FoundHomeCell = true;
			}
		}

		if (!FoundHomeCell)
		{
			D3DXVECTOR2 Start(pCell->CenterPoint().x, pCell->CenterPoint().z);
			D3DXVECTOR2 End(Point.x, Point.z);
			Line2D MotionPath(Start, End);
			NavigationCell* NextCell;
			NavigationCell::CELL_SIDE WallHit;
			D3DXVECTOR2 PointOfIntersection;

			NavigationCell::PATH_RESULT Result = pCell->ClassifyPathToCell(MotionPath, &NextCell, WallHit, &PointOfIntersection);

			if (Result == NavigationCell::EXITING_CELL)
			{
				D3DXVECTOR3 ClosestPoint3D(PointOfIntersection.x,0.0f,PointOfIntersection.y);
				pCell->MapVectorHeightToCell(ClosestPoint3D);

				ClosestPoint3D -= Point;

				ThisDistance = (float)sqrt ( ClosestPoint3D.x * ClosestPoint3D.x +
						ClosestPoint3D.y * ClosestPoint3D.y +
						ClosestPoint3D.z * ClosestPoint3D.z ); 
				if (ThisDistance<ClosestDistance)
				{
					ClosestDistance=ThisDistance;
					ClosestCell = pCell;
				}
			}
		}
	}

	if ( !ClosestCell )
	{
		GASSERT(0&&"ClosestCell==NULL");
		return 0;
	}

	return ClosestCell->CellID();
}

bool NavigationMesh::BuildNavigationPath(NavigationPath& NavPath,
		DWORD StartID, const D3DXVECTOR3& StartPos, DWORD EndID, const D3DXVECTOR3& EndPos, float* pfPathDist )
{
	NavigationCell* StartCell = GetCell ( StartID );
	NavigationCell* EndCell = GetCell ( EndID );
	if ( !StartCell )	return false;
	if ( !EndCell )		return false;

	bool FoundPath = false;

	++m_PathSession;

	m_NavHeap.Setup(m_PathSession, StartPos);

	//	We are doing a reverse search, from EndCell to StartCell.
	//	Push our EndCell onto the Heap at the first cell to be processed
	//	<--	길찾기 최초값 세팅
	//	<**	길찾기는 목표지점부터 시작되어, 시작점을 찾는 방법으로 진행된다.
	//		여기서 QueryForPath에 0라는 값을 설정해 줌으로써
	//		최조 값을 넣어준다.
	//		QueryForPath에 보면 이거 설정하는게 한 눈에 들어온다.
	EndCell->QueryForPath(&m_NavHeap, 0, 0);
	//	**>
	//	-->

	//	process the heap until empty, or a path is found
	//	<--	길찾기 시~작~!!
	while(m_NavHeap.NotEmpty() && !FoundPath)
	{
		NavigationNode ThisNode;

		// pop the top cell (the open cell with the lowest cost) off the Heap
		m_NavHeap.GetTop(ThisNode);

		if ( ThisNode.cell->ArrivalWall() == -1 )
		{			
			return FoundPath;
		}

		// if this cell is our StartCell, we are done
		if(ThisNode.cell == StartCell)
		{
			FoundPath = true;
		}
		else
		{
			// Process the Cell, Adding it's neighbors to the Heap as needed
			ThisNode.cell->ProcessCell(&m_NavHeap);
		}
	}
	//	-->	길찾기 시~작~!!

	// if we found a path, build a waypoint list
	// out of the cells on the path
	if (FoundPath)
	{
		NavigationCell* TestCell = StartCell;
		NavigationCell* CellB = NULL;		
		NavigationCell* NextCell = NULL;		
		D3DXVECTOR3 NewWayPoint;

		// Setup the Path object, clearing out any old data
		DWORD	StartID = StartCell->CellID();
		DWORD	EndID	= EndCell->CellID();
		NavPath.Setup(this, StartPos, StartID, EndPos, EndID);

		if ( pfPathDist && ( StartID == EndID ) )
		{
			D3DXVECTOR3	vDist = StartPos - EndPos;
			(*pfPathDist) = D3DXVec3Length ( &vDist );
		}
		else
		{
			// Step through each cell linked by our A* algorythm 
			// from StartCell to EndCell
			float	CorrectY = 0.0f;
			D3DXVECTOR3	PrevWayPoint = StartPos;
			while (TestCell && TestCell != EndCell)
			{
				// add the link point of the cell as a way point (the exit wall's center)
				int LinkWall = TestCell->ArrivalWall();			

				NewWayPoint = TestCell->WallMidpoint(LinkWall);
				NewWayPoint = SnapPointToCell(TestCell->CellID(), NewWayPoint); // just to be sure

				if ( TestCell->Normal().y <= 0.0001f )
				{
					NewWayPoint.y = CorrectY;
				}
				else
				{
					CorrectY = NewWayPoint.y;
				}

				if ( pfPathDist )
				{
					D3DXVECTOR3	vDist = PrevWayPoint - NewWayPoint;					
					(*pfPathDist) += D3DXVec3Length ( &vDist );
				}

				NavPath.AddWayPoint(NewWayPoint, TestCell->CellID());

				// and on to the next cell
				TestCell = TestCell->Link(LinkWall);
			}
		}

		// cap the end of the path.
		NavPath.EndPath();
		return(true);
	}
	return(false);
}

void NavigationMesh::ResolveMotionOnMesh(const D3DXVECTOR3& StartPos, DWORD StartID, D3DXVECTOR3& EndPos, DWORD *EndID)
{
	// create a 2D motion path from our Start and End positions, tossing out their Y values to project them 
	// down to the XZ plane.
	Line2D MotionPath(D3DXVECTOR2(StartPos.x,StartPos.z), D3DXVECTOR2(EndPos.x,EndPos.z));

	// these three will hold the results of our tests against the cell walls
	NavigationCell::PATH_RESULT Result = NavigationCell::NO_RELATIONSHIP;
	NavigationCell::CELL_SIDE WallNumber;
	D3DXVECTOR2 PointOfIntersection;
	NavigationCell* NextCell;	

	// TestCell is the cell we are currently examining.	
	NavigationCell* TestCell = GetCell(StartID);
	if ( !TestCell )	return;

	//
	// Keep testing until we find our ending cell or stop moving due to friction
	//
	BOOL	bDiff = FALSE;
	BOOL	bFirstTimeSame = FALSE;
	bDiff = (int(MotionPath.EndPointA().x) != int(MotionPath.EndPointB().x)) ||
			(int(MotionPath.EndPointA().y) != int(MotionPath.EndPointB().y));
	
	//////////////////////////////////////////////////////////////////////////////
	//	주의!!!!! 지우지 마세요!!!!
	//	Note	:	처음부터 시작지점과 끝점이 같다고 하는 것은,
	//				페이스의 노말 y가 0또는 그 이하인 경우가 포함된다.
	//				이것은 다음 셀의 좌표를 그대로 둬야 정상작동을 하게 된다.	
	if ( !bDiff )
	{
		bFirstTimeSame = TRUE;
	}
	//////////////////////////////////////////////////////////////////////////////
	int i = 0;
	for ( i = 0; i < 100; i++ ) 
	{	
		if ( Result == NavigationCell::ENDING_CELL )
		{
			break;
		}

		if ( !bDiff )
		{
			break;
		}

		Result = TestCell->ClassifyPathToCell(MotionPath, &NextCell, WallNumber, &PointOfIntersection);

		if (Result == NavigationCell::EXITING_CELL)
		{
			if(NextCell)
			{
				MotionPath.SetEndPointA(PointOfIntersection);
				TestCell = NextCell;
			}
			else
			{
				// we have hit a solid wall. Resolve the collision and correct our path.
				MotionPath.SetEndPointA(PointOfIntersection);
				TestCell->ProjectPathOnCellWall(WallNumber, MotionPath);

				// add some friction to the new MotionPath since we are scraping against a wall.
				// we do this by reducing the magnatude of our motion by 10%
				D3DXVECTOR2 Direction = MotionPath.EndPointB() - MotionPath.EndPointA();
				Direction *= 0.98f;
				MotionPath.SetEndPointB(MotionPath.EndPointA() + Direction);
			}
		}
		else if (Result == NavigationCell::NO_RELATIONSHIP)
		{
			D3DXVECTOR2 NewOrigin = MotionPath.EndPointA();
			TestCell->ForcePointToCellCollumn(NewOrigin);
			MotionPath.SetEndPointA(NewOrigin);
		}
		bDiff = (int(MotionPath.EndPointA().x) != int(MotionPath.EndPointB().x)) ||
				(int(MotionPath.EndPointA().y) != int(MotionPath.EndPointB().y));		
	}

	if ( 100 <= i )
	{
		GASSERT ( "최대 테스트 회수 초과 ResolveMotionOnMesh" );
	}

	// we now have our new host cell
	*EndID = TestCell->CellID();

	// Update the new control point position, 
	// solving for Y using the Plane member of the NavigationCell
	EndPos.x = MotionPath.EndPointB().x;
	EndPos.z = MotionPath.EndPointB().y;

	///////////////////////////////////////////////////////////////////
	//	Note	:	처음부터 같은 경우가 아니면,
	//				y 값을 계산해서 EndPos에 넣어준다.
	if ( !bFirstTimeSame )
	{
		TestCell->MapVectorHeightToCell(EndPos);
	}
}


//:	LineOfSightTest
//----------------------------------------------------------------------------------------
//
// Test to see if two points on the mesh can view each other
//
//-------------------------------------------------------------------------------------://
bool NavigationMesh::LineOfSightTest(DWORD StartID, const D3DXVECTOR3& StartPos, DWORD EndID, const D3DXVECTOR3& EndPos)
{
	NavigationCell* StartCell	=	GetCell(StartID);
	NavigationCell* EndCell		=	GetCell(EndID);
	if ( !StartCell )		return false;
	if ( !EndCell )			return false;

	Line2D MotionPath(D3DXVECTOR2(StartPos.x,StartPos.z), D3DXVECTOR2(EndPos.x,EndPos.z));
	
	NavigationCell* pProcessCell = StartCell;
	NavigationCell *pNextCell = NULL;
	NavigationCell::CELL_SIDE WallNumber;
	NavigationCell::PATH_RESULT Result;

	while(1)
	{	
		pNextCell = NULL;
		Result = pProcessCell->ClassifyPathToCell(MotionPath, &pNextCell, WallNumber, 0);

		// Before Code
		/*if ( Result != NavigationCell::EXITING_CELL ) return(true);
		if (!pNextCell) return(false);
		pProcessCell = pNextCell;*/

		// Modify Code [Author:서광수 2006.12.06]  Begin ====>
		if ( Result == NavigationCell::EXITING_CELL ) 
		{
			if ( !pNextCell ) return false;
			pProcessCell = pNextCell;
			continue;
		}
		else if ( Result == NavigationCell::ENDING_CELL && pProcessCell->CellID() != EndCell->CellID() ) return false;
		return true;
		// Modify Code [Author:서광수 2006.12.06]  <==== End
	}
}

//:	LinkCells
//----------------------------------------------------------------------------------------
//
// Link all the cells that are in our pool
//
//-------------------------------------------------------------------------------------://
void NavigationMesh::LinkCells()
{
	CELL_ARRAY::iterator IterA = m_CellArray.begin();

	DWORD	*pCollisionCellID = new DWORD [ TotalCells() ];
	DWORD	CollisionIDCount = 0;

	while (IterA != m_CellArray.end())
	{
		NavigationCell* pCellA = *IterA;
		
		//	<--	현재 삼각형의 최대 최소 지점을 구한다.
		D3DXVECTOR3		vMax, vMin;
		vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( int i = 0; i < 3; i++ )
		{
			if ( vMax.x < m_pNaviVertex[pCellA->Vertex(i)].x ) vMax.x = m_pNaviVertex[pCellA->Vertex(i)].x + 5.0f;
			if ( vMax.y < m_pNaviVertex[pCellA->Vertex(i)].y ) vMax.y = m_pNaviVertex[pCellA->Vertex(i)].y + 5.0f;
			if ( vMax.z < m_pNaviVertex[pCellA->Vertex(i)].z ) vMax.z = m_pNaviVertex[pCellA->Vertex(i)].z + 5.0f;

			if ( vMin.x > m_pNaviVertex[pCellA->Vertex(i)].x ) vMin.x = m_pNaviVertex[pCellA->Vertex(i)].x - 5.0f;
			if ( vMin.y > m_pNaviVertex[pCellA->Vertex(i)].y ) vMin.y = m_pNaviVertex[pCellA->Vertex(i)].y - 5.0f;
			if ( vMin.z > m_pNaviVertex[pCellA->Vertex(i)].z ) vMin.z = m_pNaviVertex[pCellA->Vertex(i)].z - 5.0f;
		}
		//	-->

		//	<--	이 셀이 부딪히는 모듈 들고 오기
		//	<**	이 셀이 가지는 바운딩 박스를 좀 크게 만들고
		//		그 리턴 값에서 링크걸 놈을 찾아 링크 건다.
		//	**>
		CollisionIDCount = 0;
		GetAllCollisionCell ( pCollisionCellID, CollisionIDCount, vMax, vMin, m_pAABBTreeRoot );
		//	-->
		
		int i = 0;
		while ( i < int(CollisionIDCount) )
		{
			NavigationCell* pCellB = m_pDACell[pCollisionCellID[i]];

			if (pCellA->CellID() != pCellB->CellID())
			{
				if (!pCellA->Link(NavigationCell::SIDE_AB) && pCellB->RequestLink(pCellA->Vertex(0), pCellA->Vertex(1), pCellA, m_pNaviVertex))
				{
					pCellA->SetLink(NavigationCell::SIDE_AB, pCellB);
				}
				else if (!pCellA->Link(NavigationCell::SIDE_BC) && pCellB->RequestLink(pCellA->Vertex(1), pCellA->Vertex(2), pCellA, m_pNaviVertex))
				{
					pCellA->SetLink(NavigationCell::SIDE_BC, pCellB);
				}
				else if (!pCellA->Link(NavigationCell::SIDE_CA) && pCellB->RequestLink(pCellA->Vertex(2), pCellA->Vertex(0), pCellA, m_pNaviVertex))
				{
					pCellA->SetLink(NavigationCell::SIDE_CA, pCellB);
				}				
			}

			i++;
		}

		++IterA;
        ExportProgress::CurPos++;
	}

	SAFE_DELETE_ARRAY ( pCollisionCellID );
}

//:	Clear
//----------------------------------------------------------------------------------------
//
//	Delete all cells associated with this mesh 
//
//-------------------------------------------------------------------------------------://
void NavigationMesh::Clear()
{
	CELL_ARRAY::iterator	CellIter = m_CellArray.begin();
	for(;CellIter != m_CellArray.end(); ++CellIter)
	{
		NavigationCell* Cell = *CellIter;
		delete Cell;
	}

	m_CellArray.clear();
	SAFE_DELETE(m_pAABBTreeRoot);

	SAFE_DELETE_ARRAY ( m_pErrorID );
	m_nErrorID = 0;

	DelNaviVertexIndexData();
	DeleteVBIB();
}

BOOL	NavigationMesh::CheckIntegrity ( char *szDebugFullDir )
{
	NavigationCell	*pCellA, *pCellB, *pBLinkToA;
	DWORD			AID, BToAID;
	BOOL			bResult = TRUE;
	BOOL			bLinkBroken = TRUE;

	//	<--	절대 경로 설정
	const	char	CheckIntegrityFileName[] = "CellLinkIntegrity.bin";
    char	szFullPathName[MAX_PATH] = "";
	StringCchCopy( szFullPathName, MAX_PATH, szDebugFullDir );
	StringCchCat( szFullPathName, MAX_PATH, CheckIntegrityFileName );
	//	-->	절대 경로 설정

	DWORD		ErrorCount = 0;
	CSerialFile	SFile;	
	if ( SFile.OpenFile ( FOT_WRITE, szFullPathName ) )
	{
		SFile << ErrorCount;
		SNaviIntegrityLog	IntegrityLog;
		
		for ( int i = 0; i < TotalCells(); i++ )
		{
			//	<--	주석 절대 지우지 마세요.
			//		헷갈려 죽슴미다.
			//	<**	자신의 셀을 A에 담음
			pCellA = m_pDACell[i];		
			AID = pCellA->CellID();
			for ( int j = 0; j < 3; j++ )
			{
				//	<**	A는 세개의 링크를 가지므로.. 그것을 B에 담음
				pCellB = pCellA->Link ( j );
				if ( pCellB )
				{
					for ( int k = 0; k < 3; k++ )
					{
						//	<**	B의 링크도 세개의 링크를 가지는데...
						//		B에서 A로 가는 링크가 있는지 체크함
						//		이때.. 존재하지 않으면, 무결성이 유지되지 않는것이다.
						//		서로 바라봐야하는데, 한쪽만 본다는 것은 문제가 있는 것이다.
						pBLinkToA = pCellB->Link ( k );
						
						if ( !pBLinkToA ) continue;

						BToAID = pBLinkToA->CellID();
						if ( AID == BToAID )
						{
							bLinkBroken = FALSE;
							break;
						}
					}
					if ( bLinkBroken )
					{
						memset ( IntegrityLog.CellID, -1, sizeof ( DWORD ) * 10 );
						IntegrityLog.CellID[0] = AID;
						IntegrityLog.CellID[1] = BToAID;
						StringCchPrintf( IntegrityLog.StrLog, 256, "[%d] 링크가 중첩됩니다. %d -> %d", ErrorCount, AID, BToAID );
						SFile.WriteBuffer ( &IntegrityLog, sizeof ( SNaviIntegrityLog ) );
						ErrorCount++;
						bResult = FALSE;
					}
					else
					{
						bLinkBroken = TRUE;
					}
				}
			}

			if ( pCellA->Normal().y < 0.001f )
			{
				memset ( IntegrityLog.CellID, -1, sizeof ( DWORD ) * 10 );
				IntegrityLog.CellID[0] = AID;				
				StringCchPrintf( IntegrityLog.StrLog, 256, "[%d] y의 노말이 0보다 작습니다.%d", ErrorCount, pCellA->CellID() );
				SFile.WriteBuffer ( &IntegrityLog, sizeof ( SNaviIntegrityLog ) );
				ErrorCount++;
				bResult = FALSE;
			}
		}
        
		DWORD	BackOffset = SFile.GetfTell ();
		SFile.SetOffSet ( 0 );
        SFile << ErrorCount;


		//	<--	섬 체크
		SFile.SetOffSet ( BackOffset );
		DWORD	RegionCount = 0;
		SFile << RegionCount;

		BOOL	bPath = FALSE;
		const	DWORD	NOTUSED = -1;
		DWORD	*pGroup = new DWORD [ TotalCells() ];
		memset ( pGroup, NOTUSED, sizeof ( DWORD ) * TotalCells() );
		
        DWORD	EndCellID = TotalCells() - 1;
		NavigationCell *pEndCell = m_pDACell [ EndCellID ];
		for ( DWORD StartCellID = 0; StartCellID <= EndCellID; StartCellID++ )
		{
			if ( pGroup[ StartCellID ] != NOTUSED )
			{
				continue;
			}
			CheckLink ( pGroup, StartCellID, RegionCount );
			RegionCount++;
		}

		if ( 1 < RegionCount )
		{
			bResult = FALSE;
		}

		SFile.WriteBuffer ( pGroup, sizeof ( DWORD ) * TotalCells() );
		SFile.SetOffSet ( BackOffset );
		SFile << RegionCount;		
		SFile.CloseFile ();

		SAFE_DELETE_ARRAY ( pGroup );
		//	-->	섬 체크
	}

	return bResult;
}

void	NavigationMesh::GotoErrorPosition ( DWORD	*pIDs, DWORD	nIDs )
{
	if ( m_nErrorID )
	{
		m_nErrorID = 0;
        SAFE_DELETE_ARRAY ( m_pErrorID );
	}
	m_nErrorID = nIDs;
	m_pErrorID = new DWORD [ m_nErrorID ];
	memcpy ( m_pErrorID, pIDs, sizeof ( DWORD ) * m_nErrorID );	

	NavigationCell *pCell = GetCell ( m_pErrorID[0] );
	if ( !pCell )		return;

	D3DXVECTOR3	vJump = pCell->CenterPoint();

	DxViewPort::GetInstance().CameraJump ( vJump );	
}

HRESULT NavigationMesh::AddCell(const DWORD& PointA, const DWORD& PointB, const DWORD& PointC, const DWORD& CellID)
{
	HRESULT	hr = E_FAIL;

	NavigationCell* NewCell = new NavigationCell;


	//	<--	셀을 만들지 못하는 오류 반환
	hr = NewCell->Initialize ( m_pNaviVertex, PointA, PointB, PointC, CellID );
	if ( FAILED ( hr ) )
	{
		return E_FAIL;
	}
	//	-->	셀을 만들지 못하는 오류 반환

	//	<--	직접 접근 위한 셀 설정
	m_pDACell[CellID] = NewCell;
	//	-->	직접 접근 위한 셀 설정

	m_CellArray.push_back(NewCell);
	
	ExportProgress::CurPos++;

	return S_OK;
}


HRESULT	NavigationMesh::CreateNaviMesh ( DxFrame *pNaviFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{	
	if ( !pNaviFrame )
	{
		return E_FAIL;
	}

	//	<-- 변수설명
	//		*루프를 위한 의미없는 변수*
	ULONG			i = 0;
	//	--> 변수설명
	
	DWORD		*pIndices = NULL;	 
	D3DXVECTOR3	*pVertices = NULL;	 

	LPD3DXMESH	pMeshData = pNaviFrame->pmsMeshs->m_pSysMemMesh;

	ULONG		NumOfIndices = pMeshData->GetNumFaces() * 3;
	
	//	<--	인덱스 버퍼를 뜯어옴
	//		*인덱스버퍼가 WORD인지 DWORD인지를 구분하여 뜯음*
	if ( pMeshData->GetOptions() & D3DXMESH_32BIT )
	{
		DWORD	*pLockIndices = NULL;

		pIndices = new DWORD [ NumOfIndices ];
		memset ( pIndices, 0, sizeof ( DWORD ) * NumOfIndices );

		pMeshData->LockIndexBuffer ( D3DLOCK_READONLY, (void**)&pLockIndices );
		for ( i = 0; i < NumOfIndices; i++ )
		{
			pIndices[i] = pLockIndices[i];
		}		
		pMeshData->UnlockIndexBuffer();
	}
	else
	{
		WORD	*pLockIndices = NULL;		

		pIndices = new DWORD [ NumOfIndices ];
		memset ( pIndices, 0, sizeof ( DWORD ) * NumOfIndices );
		
		pMeshData->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pLockIndices );
		for ( i = 0; i < NumOfIndices; i++ )
		{
			pIndices[i] = (WORD)pLockIndices[i];
		}		
		pMeshData->UnlockIndexBuffer();
	}
	//	-->	인덱스 버퍼를 뜯어옴
	
	//	<--	버텍스 버퍼를 뜯어옴	
	PBYTE	pLockVertices = NULL;
	ULONG	NumOfVertices = pMeshData->GetNumVertices();

	pVertices = new D3DXVECTOR3[NumOfVertices];
	memset ( pVertices, 0, sizeof ( D3DXVECTOR3 ) * NumOfVertices );

	pMeshData->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pLockVertices );

	PBYTE pbCur;
	D3DXVECTOR3 vDist;
	D3DXVECTOR3 *pvCur;
	DWORD dwFVF = pMeshData->GetFVF();
	DWORD fvfsize = D3DXGetFVFVertexSize ( dwFVF );
	UINT iPoint;
	for ( iPoint=0, pbCur = pLockVertices; iPoint < NumOfVertices; iPoint++, pbCur += fvfsize )
	{
		pvCur = (D3DXVECTOR3*)pbCur;
		D3DXVec3TransformCoord ( &vDist, pvCur, &pNaviFrame->matCombined );
		pVertices[iPoint] = vDist;
	}
	pMeshData->UnlockVertexBuffer();

	//	-->	버텍스 버퍼를 뜯어옴

//	<--	NavigationMesh 데이타를 세팅	
	ULONG	NumOfFaces = NumOfIndices / 3;
	//	<--	NaviVertex 설정
	SetNaviVertexIndexData ( pVertices, NumOfVertices, NumOfFaces );
	//	-->	NaviVertex 설정
	for ( i = 0; i < NumOfFaces; i++ )
	{
		DWORD	vertA, vertB, vertC;

		vertA = pIndices[i*3+0];
		vertB = pIndices[i*3+1];
		vertC = pIndices[i*3+2];

		if ((vertA != vertB) && (vertB != vertC) && (vertC != vertA))
		{
            AddCell(vertA, vertB, vertC, i);			
		}
		else
		{			
			SAFE_DELETE_ARRAY ( pIndices );
			SAFE_DELETE_ARRAY ( pVertices );

			return E_FAIL;
		}
	}
//	-->	NavigationMesh 데이타를 세팅

	SAFE_DELETE_ARRAY ( pIndices );
	SAFE_DELETE_ARRAY ( pVertices );

	if ( FAILED ( CreateVBIB ( pd3dDevice, pMeshData ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT NavigationMesh::Render(LPDIRECT3DDEVICEQ			pd3dDevice)
{	
	if ( !TotalCells() )	return E_FAIL;
	if ( !m_pVB || !m_pIB )	return E_FAIL;
	
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ); 
	
	DWORD dwFill;
	pd3dDevice->GetRenderState(D3DRS_FILLMODE, &dwFill);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	DWORD dwLight;
	pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwLight);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE );

	DWORD dwColorOP, dwColorARG2;
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2,	&dwColorARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

	HRESULT hr;
	hr = pd3dDevice->SetStreamSource ( 0, m_pVB, 0, D3DXGetFVFVertexSize(DIFFUSEVERTEX::FVF) );
	if ( FAILED ( hr ) )
	{
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwFill);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
		return E_FAIL;
	}
	
	hr = pd3dDevice->SetFVF ( DIFFUSEVERTEX::FVF );
	if ( FAILED ( hr ) )
	{
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwFill);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
		return E_FAIL;
	}

	hr = pd3dDevice->SetIndices ( m_pIB );
	if ( FAILED ( hr ) )
	{
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwFill);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
		return E_FAIL;
	}
	
	hr = pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_nNaviVertex, 0, TotalCells () );
	if ( FAILED ( hr ) )
	{
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwFill);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
		return E_FAIL;
	}

	if ( m_nErrorID )
	{
		DWORD dwFillError;
		pd3dDevice->GetRenderState(D3DRS_FILLMODE, &dwFillError);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwFill);

		WORD			*pERRORINDEX = new WORD [ m_nErrorID * 3 ];
		DIFFUSEVERTEX	*pERRORVERTEX = new DIFFUSEVERTEX [ m_nErrorID * 3 ];

		for ( WORD i = 0; i < m_nErrorID; i++ )
		{
            pERRORINDEX[(i*3)+0] = (i*3)+0;
			pERRORINDEX[(i*3)+1] = (i*3)+1;
			pERRORINDEX[(i*3)+2] = (i*3)+2;

			pERRORVERTEX[(i*3)+0].vPos = m_pNaviVertex[m_pDACell[m_pErrorID[i]]->Vertex(0)];
			pERRORVERTEX[(i*3)+1].vPos = m_pNaviVertex[m_pDACell[m_pErrorID[i]]->Vertex(1)];
			pERRORVERTEX[(i*3)+2].vPos = m_pNaviVertex[m_pDACell[m_pErrorID[i]]->Vertex(2)];

			if ( m_nErrorID == 2 )
			{
				if ( i % 2 )
				{
					pERRORVERTEX[(i*3)+0].Diffuse = D3DCOLOR_XRGB(255,0,0);
					pERRORVERTEX[(i*3)+1].Diffuse = D3DCOLOR_XRGB(255,0,0);
					pERRORVERTEX[(i*3)+2].Diffuse = D3DCOLOR_XRGB(255,0,0);
				}
				else
				{
					pERRORVERTEX[(i*3)+0].Diffuse = D3DCOLOR_XRGB(0,0,255);
					pERRORVERTEX[(i*3)+1].Diffuse = D3DCOLOR_XRGB(0,0,255);
					pERRORVERTEX[(i*3)+2].Diffuse = D3DCOLOR_XRGB(0,0,255);
				}
			}
			else
			{			
				pERRORVERTEX[(i*3)+0].Diffuse = D3DCOLOR_XRGB(255,0,0);
				pERRORVERTEX[(i*3)+1].Diffuse = D3DCOLOR_XRGB(255,0,0);
				pERRORVERTEX[(i*3)+2].Diffuse = D3DCOLOR_XRGB(255,0,0);
			}
		}

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ); 

		pd3dDevice->SetFVF( DIFFUSEVERTEX::FVF );
		pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, m_nErrorID * 3, m_nErrorID,
			pERRORINDEX, D3DFMT_INDEX16, pERRORVERTEX, D3DXGetFVFVertexSize(DIFFUSEVERTEX::FVF) );

        SAFE_DELETE_ARRAY ( pERRORINDEX );
		SAFE_DELETE_ARRAY ( pERRORVERTEX );
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwFillError);
	}


	pd3dDevice->SetRenderState(D3DRS_FILLMODE, dwFill);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	dwColorARG2 );

	return S_OK;
}

HRESULT	NavigationMesh::SetNaviVertexIndexData ( D3DXVECTOR3 *pNaviVertex, int nNaviVertex, int nDACell )
{
	if ( !pNaviVertex || nNaviVertex < 1 || nDACell < 1 )
	{
		return E_FAIL;
	}

	long	FloorFloatX = 0;
	long	FloorFloatY = 0;
	long	FloorFloatZ = 0;
	for ( int i = 0; i < nNaviVertex; i++ )
	{
		FloorFloatX = long ( pNaviVertex[i].x * 1000.f );
		FloorFloatY = long ( pNaviVertex[i].y * 1000.f );
		FloorFloatZ = long ( pNaviVertex[i].z * 1000.f );

		pNaviVertex[i].x = float ( FloorFloatX / 1000.f );
		pNaviVertex[i].y = float ( FloorFloatY / 1000.f );
		pNaviVertex[i].z = float ( FloorFloatZ / 1000.f );		
	}

	m_nNaviVertex = nNaviVertex;
	m_pNaviVertex = new D3DXVECTOR3 [ m_nNaviVertex ];

	memset ( m_pNaviVertex, 0, sizeof ( D3DXVECTOR3 ) * m_nNaviVertex );
	memcpy ( m_pNaviVertex, pNaviVertex, sizeof ( D3DXVECTOR3 ) * m_nNaviVertex );

	m_pDACell = new NavigationCell* [ nDACell ];
	memset ( m_pDACell, NULL, sizeof ( NavigationCell* ) * nDACell );

	return S_OK;
}

void	NavigationMesh::CheckLink ( DWORD *pUsedCell, DWORD StartCellID, DWORD GroupID )
{
	static	const	DWORD	NOTUSED = -1;

	DWORD	*pCellIDStack = NULL;
	pCellIDStack = new DWORD [TotalCells()];
	DWORD	StackCount = 1;

	pCellIDStack[0] = NOTUSED;
	pCellIDStack[1] = StartCellID;

    while ( StackCount )
	{
		//	<--	Pop
		DWORD CellID = pCellIDStack[StackCount];
		pUsedCell[CellID] = GroupID;
		NavigationCell *pStartCell = m_pDACell[CellID];
		StackCount--;
		//	-->	Pop

		//	<--	Push
		if ( pStartCell->Link(0) )
		{
			DWORD	TmpCellID = pStartCell->Link(0)->CellID();
			if ( pUsedCell[TmpCellID] == NOTUSED )
			{	
				StackCount++;
				pCellIDStack[StackCount] = TmpCellID;				
			}
		}
		if ( pStartCell->Link(1) )
		{
			DWORD	TmpCellID = pStartCell->Link(1)->CellID();
			if ( pUsedCell[TmpCellID] == NOTUSED )
			{
				StackCount++;
				pCellIDStack[StackCount] = TmpCellID;			
			}
		}
		if ( pStartCell->Link(2) )
		{
			DWORD	TmpCellID = pStartCell->Link(2)->CellID();
			if ( pUsedCell[TmpCellID] == NOTUSED )
			{
				StackCount++;
				pCellIDStack[StackCount] = TmpCellID;				
			}
		}
		//	-->	Push
	}
	SAFE_DELETE_ARRAY ( pCellIDStack );

}

void	NavigationMesh::Update(float elapsedTime)
{
}

void	NavigationMesh::DelNaviVertexIndexData ()
{
	SAFE_DELETE_ARRAY ( m_pNaviVertex );
	SAFE_DELETE_ARRAY ( m_pDACell );	

	m_nNaviVertex = 0;
}

HRESULT	NavigationMesh::CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshData )
{	
	if ( FAILED ( pd3dDevice->CreateVertexBuffer( m_nNaviVertex * sizeof(DIFFUSEVERTEX),
		0, DIFFUSEVERTEX::FVF, D3DPOOL_MANAGED, &m_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	DIFFUSEVERTEX	*pLockVertices = NULL;
	m_pVB->Lock ( 0, 0, (VOID**)&pLockVertices, NULL );	
	for ( int i = 0; i < m_nNaviVertex; i++ )
	{
		pLockVertices[i].vPos = m_pNaviVertex[i];
		pLockVertices[i].Diffuse = D3DCOLOR_XRGB(255,255,255);
	}
	m_pVB->Unlock();


	if ( pMeshData->GetOptions() & D3DXMESH_32BIT )
	{	
		if ( FAILED ( pd3dDevice->CreateIndexBuffer ( TotalCells () * 3 * sizeof(DWORD),
			0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIB, NULL ) ) )
		{
			return E_FAIL;
		}

		DWORD *pLockIndices = NULL;
		m_pIB->Lock ( 0, 0, (VOID**)&pLockIndices, NULL );
		for ( int i = 0; i < TotalCells(); i++ )
		{
			pLockIndices[i*3 + 0] = m_pDACell[i]->Vertex(0);
			pLockIndices[i*3 + 1] = m_pDACell[i]->Vertex(1);
			pLockIndices[i*3 + 2] = m_pDACell[i]->Vertex(2);
		}
		m_pIB->Unlock();
	}
	else
	{
		if ( FAILED ( pd3dDevice->CreateIndexBuffer( TotalCells () * 3 * sizeof(WORD),
			0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL ) ) )
		{
			return E_FAIL;
		}

		WORD *pLockIndices = NULL;
		m_pIB->Lock ( 0, 0, (VOID**)&pLockIndices, NULL );
		for ( int i = 0; i < TotalCells(); i++ )
		{
			pLockIndices[i*3 + 0] = (WORD)m_pDACell[i]->Vertex(0);
			pLockIndices[i*3 + 1] = (WORD)m_pDACell[i]->Vertex(1);
			pLockIndices[i*3 + 2] = (WORD)m_pDACell[i]->Vertex(2);
		}
		m_pIB->Unlock();
	}

	return S_OK;
}

HRESULT		NavigationMesh::DeleteVBIB ( )
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );

	return S_OK;
}